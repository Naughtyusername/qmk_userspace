// Copyright 2024 splitkb.com (support@splitkb.com)
// Copyright 2025 naughtyusername
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Halcyon Corne TFT Display — Cyberpunk-neon theme
//
// Layout (135×240 display, portrait):
//   y=5    Layer name (text, colored per layer)
//   y=40   WPM counter ("XXX wpm", magenta)
//   y=75   Lock indicators (Cap Num Scr, cyan)
//   y=140  Tux pixel art (100×100, cyan/magenta recolor)
//
// Idle animation: Game of Life fills the full display after 30s,
// snaps back to info display on any input.

#include "halcyon.h"
#include "hlc_tft_display.h"
#include "naughtyusername.h"

#include "hardware/structs/rosc.h"
#include <stdio.h>

// Font
#include "graphics/fonts/Retron2000-27.qff.h"
#include "graphics/fonts/Retron2000-underline-27.qff.h"

// Tux image (mono4 palette — 4-shade grayscale, recolorable)
#include "graphics/tux_100.qgf.h"

// ==========================================================================
// Layer names and colors
// ==========================================================================
// These must match the enum userspace_layers order in naughtyusername.h.
// If you add/remove/reorder layers, update both arrays.

static const char *layer_names[] = {
    "BASE",    // _BASE
    "VIM",     // _VIM
    "LOWER",   // _LOWER
    "RAISE",   // _RAISE
    "FUNC",    // _FUNCTION
    "ADJUST",  // _ADJUST
    "GAMING",  // _GAMING
    "GAME2",   // _GAMING2
    "ROGUE",   // _ROGUELIKE
    "SYS",     // _SYS
    "MOUSE",   // _MOUSE
};

#define NUM_LAYERS (sizeof(layer_names) / sizeof(layer_names[0]))

// HSV color per layer — cyberpunk palette cycling through cyan/blue/purple/magenta
typedef struct { uint8_t h, s, v; } display_hsv_t;

static const display_hsv_t layer_colors[] = {
    {128, 255, 255},  // BASE     - Cyan (home base, calm)
    {170, 255, 255},  // VIM      - Blue
    {191, 255, 255},  // LOWER    - Purple
    {213, 255, 255},  // RAISE    - Magenta
    {148, 255, 255},  // FUNC     - Teal
    {200, 255, 255},  // ADJUST   - Blue-purple
    {85,  255, 255},  // GAMING   - Green (stands out from work layers)
    {85,  200, 200},  // GAMING2  - Green (dimmer variant)
    {43,  255, 255},  // ROGUE    - Yellow/amber
    {0,   255, 255},  // SYS      - Red (danger zone)
    {213, 200, 255},  // MOUSE    - Magenta (lighter)
};

// Lock indicator labels — single letters to fit cleanly across 135px
static const char *lock_labels[] = { "C", "N", "S" };

// ==========================================================================
// Display objects and state
// ==========================================================================

static painter_font_handle_t font;
static painter_font_handle_t font_underline;

static uint8_t lcd_surface_fb[SURFACE_REQUIRED_BUFFER_BYTE_SIZE(135, 240, 16)];

painter_device_t lcd;
painter_device_t lcd_surface;

// State tracking — only redraw what changed
static led_t last_led_state = {0};
static layer_state_t last_layer = 0;
static uint8_t last_wpm = 255;  // Impossible initial value forces first draw
static bool tux_drawn = false;
static bool idle_mode = false;
static bool fonts_loaded = false;
static bool display_dirty = true; // Only flush surface to LCD when something changed

// ==========================================================================
// Layout constants
// ==========================================================================

#define LAYER_NAME_Y  8
#define WPM_Y         42
#define LOCK_Y        76
#define TUX_X         17    // (135 - 100) / 2 = 17.5, rounded down
#define TUX_Y         112   // Tight after locks, leaves 28px bottom padding
#define IDLE_TIMEOUT  30000 // 30 seconds of no input → Game of Life

// ==========================================================================
// Game of Life — idle animation
// ==========================================================================
// Grid fills the entire 135×240 display. Cells are 4px + 1px outline = 5px.
// 135/5 = 27 columns, 240/5 = 48 rows. Colors cycle through layer palette.

#define GRID_WIDTH  27
#define GRID_HEIGHT 48
#define CELL_SIZE   4
#define OUTLINE_SIZE 1
#define INITIAL_ALIVE_PROBABILITY 0.2

static int color_value = 0;

static bool grid[GRID_HEIGHT][GRID_WIDTH];
static bool new_grid[GRID_HEIGHT][GRID_WIDTH];
static bool changed_grid[GRID_HEIGHT][GRID_WIDTH];

static uint32_t get_random_32bit(void) {
    uint32_t random_value = 0;
    for (int i = 0; i < 32; i++) {
        wait_ms(1);
        random_value = (random_value << 1) | (rosc_hw->randombit & 1);
    }
    return random_value;
}

static void init_grid(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = (rand() < INITIAL_ALIVE_PROBABILITY * RAND_MAX);
            changed_grid[y][x] = true;
        }
    }
}

static void draw_grid(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            if (changed_grid[y][x]) {
                uint16_t left   = x * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t top    = y * (CELL_SIZE + OUTLINE_SIZE);
                uint16_t right  = left + CELL_SIZE + OUTLINE_SIZE;
                uint16_t bottom = top + CELL_SIZE + OUTLINE_SIZE;

                // Black outline
                qp_rect(lcd_surface, left, top, right, bottom, 0, 0, 0, true);

                // Alive cells get the current cyberpunk color
                if (grid[y][x]) {
                    display_hsv_t c = layer_colors[color_value % NUM_LAYERS];
                    qp_rect(lcd_surface, left + OUTLINE_SIZE, top + OUTLINE_SIZE,
                            right - OUTLINE_SIZE, bottom - OUTLINE_SIZE,
                            c.h, c.s, c.v, true);
                }
            }
        }
    }
}

static void update_grid(void) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            int alive_neighbors = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    if (dy == 0 && dx == 0) continue;
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < GRID_HEIGHT && nx >= 0 && nx < GRID_WIDTH) {
                        alive_neighbors += grid[ny][nx];
                    }
                }
            }
            if (grid[y][x]) {
                new_grid[y][x] = (alive_neighbors == 2 || alive_neighbors == 3);
            } else {
                new_grid[y][x] = (alive_neighbors == 3);
            }
            changed_grid[y][x] = (grid[y][x] != new_grid[y][x]);
        }
    }
    for (int y = 0; y < GRID_HEIGHT; y++) {
        for (int x = 0; x < GRID_WIDTH; x++) {
            grid[y][x] = new_grid[y][x];
        }
    }
}

static void add_cell_cluster(void) {
    int cluster_size = 3;
    int x = rand() % (GRID_WIDTH - cluster_size);
    int y = rand() % (GRID_HEIGHT - cluster_size);
    for (int dy = 0; dy < cluster_size; dy++) {
        for (int dx = 0; dx < cluster_size; dx++) {
            grid[y + dy][x + dx] = rand() % 2;
            changed_grid[y + dy][x + dx] = true;
        }
    }
}

// ==========================================================================
// Info display — layer name, WPM, locks, Tux
// ==========================================================================
// Each draw function only redraws when its state changes, except on
// force=true (used when waking from idle mode to repaint everything).

static void draw_layer_name(bool force) {
    if (last_layer != layer_state || force) {
        // Clear just the text area (full width to handle variable-length names)
        qp_rect(lcd_surface, 0, LAYER_NAME_Y, LCD_WIDTH - 1, LAYER_NAME_Y + font->line_height, 0, 0, 0, true);

        uint8_t layer = get_highest_layer(layer_state | default_layer_state);
        const char *name = (layer < NUM_LAYERS) ? layer_names[layer] : "???";
        display_hsv_t c = (layer < NUM_LAYERS)
            ? layer_colors[layer]
            : (display_hsv_t){0, 255, 255};

        // Center the text horizontally
        int16_t text_w = qp_textwidth(font, name);
        int16_t text_x = (LCD_WIDTH - text_w) / 2;
        qp_drawtext_recolor(lcd_surface, text_x, LAYER_NAME_Y, font, name,
                            c.h, c.s, c.v, 0, 0, 0);

        last_layer = layer_state;
        display_dirty = true;
    }
}

static void draw_wpm(bool force) {
#ifdef WPM_ENABLE
    uint8_t current_wpm = get_current_wpm();
    if (current_wpm != last_wpm || force) {
        // Clear just the WPM text area
        qp_rect(lcd_surface, 0, WPM_Y, LCD_WIDTH - 1, WPM_Y + font->line_height, 0, 0, 0, true);

        char wpm_str[12];
        snprintf(wpm_str, sizeof(wpm_str), "%d wpm", current_wpm);

        int16_t text_w = qp_textwidth(font, wpm_str);
        int16_t text_x = (LCD_WIDTH - text_w) / 2;
        qp_drawtext_recolor(lcd_surface, text_x, WPM_Y, font, wpm_str,
                            HSV_WPM, 0, 0, 0);

        last_wpm = current_wpm;
        display_dirty = true;
    }
#endif
}

static void draw_locks(bool force) {
    led_t current = host_keyboard_led_state();
    if (current.raw != last_led_state.raw || force) {
        // Clear just the lock text area
        qp_rect(lcd_surface, 0, LOCK_Y, LCD_WIDTH - 1, LOCK_Y + font->line_height, 0, 0, 0, true);

        // Three lock states mapped to the three labels
        bool lock_active[] = { current.caps_lock, current.num_lock, current.scroll_lock };

        // Spread evenly across the display width (thirds)
        int16_t third = LCD_WIDTH / 3;

        for (int i = 0; i < 3; i++) {
            painter_font_handle_t f = lock_active[i] ? font_underline : font;
            int16_t tw = qp_textwidth(f, lock_labels[i]);
            int16_t tx = (third * i) + (third - tw) / 2;

            if (lock_active[i]) {
                qp_drawtext_recolor(lcd_surface, tx, LOCK_Y, f, lock_labels[i],
                                    HSV_LOCK_ON, 0, 0, 0);
            } else {
                qp_drawtext_recolor(lcd_surface, tx, LOCK_Y, f, lock_labels[i],
                                    HSV_LOCK_OFF, 0, 0, 0);
            }
        }

        last_led_state = current;
        display_dirty = true;
    }
}

static void draw_tux(void) {
    if (!tux_drawn) {
        // Load → draw → close. The image data lives in flash (gfx_tux_100),
        // we only need the handle briefly to draw it once.
        painter_image_handle_t tux_img = qp_load_image_mem(gfx_tux_100);
        qp_drawimage_recolor(lcd_surface, TUX_X, TUX_Y, tux_img,
                             HSV_TUX_FG,   // Light pixels → Cyan
                             HSV_TUX_BG);  // Dark pixels  → Magenta
        qp_close_image(tux_img);
        tux_drawn = true;
        display_dirty = true;
    }
}

// Full redraw — called when waking from idle mode
static void force_redraw_info(void) {
    // Ensure fonts are loaded (may not be if we went idle before first active draw)
    if (!fonts_loaded) {
        font = qp_load_font_mem(font_Retron2000_27);
        font_underline = qp_load_font_mem(font_Retron2000_underline_27);
        fonts_loaded = true;
    }

    qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);

    // Invalidate all cached state so every draw function runs
    last_layer = ~layer_state;
    last_wpm = 255;
    last_led_state.raw = ~host_keyboard_led_state().raw;
    tux_drawn = false;

    draw_layer_name(true);
    draw_wpm(true);
    draw_locks(true);
    draw_tux();
    display_dirty = true;
}

// Incremental update — only redraws changed elements
static void update_info_display(void) {
    if (!fonts_loaded) {
        font = qp_load_font_mem(font_Retron2000_27);
        font_underline = qp_load_font_mem(font_Retron2000_underline_27);
        fonts_loaded = true;
    }

    draw_layer_name(false);
    draw_wpm(false);
    draw_locks(false);
    draw_tux();
}

// ==========================================================================
// Module callbacks — called from halcyon.c
// ==========================================================================

void module_suspend_power_down_kb(void) {
    qp_power(lcd, false);
}

void module_suspend_wakeup_init_kb(void) {
    qp_power(lcd, true);
}

bool module_post_init_kb(void) {
    backlight_enable();

    // Create the ST7789 SPI device and the offscreen surface buffer
    lcd = qp_st7789_make_spi_device(LCD_WIDTH, LCD_HEIGHT, LCD_CS_PIN,
                                     LCD_DC_PIN, LCD_RST_PIN,
                                     LCD_SPI_DIVISOR, LCD_SPI_MODE);
    lcd_surface = qp_make_rgb565_surface(LCD_WIDTH, LCD_HEIGHT, lcd_surface_fb);

    // Initialize LCD hardware
    qp_init(lcd, LCD_ROTATION);
    qp_set_viewport_offsets(lcd, LCD_OFFSET_X, LCD_OFFSET_Y);
    qp_clear(lcd);
    qp_rect(lcd, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
    qp_power(lcd, true);
    qp_flush(lcd);

    // Initialize the offscreen surface
    qp_init(lcd_surface, LCD_ROTATION);
    qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
    qp_surface_draw(lcd_surface, lcd, 0, 0, 0);
    qp_flush(lcd);

    if (!module_post_init_user()) { return false; }

    return true;
}

// Main display loop — called every cycle from halcyon.c housekeeping_task_kb
bool display_module_housekeeping_task_kb(bool second_display) {
    if (!display_module_housekeeping_task_user(second_display)) { return false; }

    if (!second_display) {
        // This is the master (left half) — our only display
        uint32_t idle_time = last_input_activity_elapsed();

        if (idle_time > IDLE_TIMEOUT) {
            // ---- Idle: Game of Life fills the screen ----
            if (!idle_mode) {
                idle_mode = true;
                // Clear the display before Game of Life takes over
                qp_rect(lcd_surface, 0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, 0, 0, 0, true);
                srand(get_random_32bit());
                init_grid();
                color_value = rand() % NUM_LAYERS;
            }

            static uint32_t last_gol_draw = 0;
            if (timer_elapsed32(last_gol_draw) >= 100) { // 10 fps
                draw_grid();
                update_grid();

                // Slowly cycle colors every 5 seconds while idle
                static uint32_t last_color_change = 0;
                if (timer_elapsed32(last_color_change) >= 5000) {
                    color_value = (color_value + 1) % NUM_LAYERS;
                    add_cell_cluster();
                    last_color_change = timer_read32();
                }

                last_gol_draw = timer_read32();
                display_dirty = true;
            }
        } else {
            // ---- Active: info display ----
            if (idle_mode) {
                // Just woke up — repaint everything
                idle_mode = false;
                force_redraw_info();
            } else {
                // Normal incremental update
                update_info_display();
            }
        }
    }

    // Only flush to LCD when the surface actually changed
    if (display_dirty) {
        qp_surface_draw(lcd_surface, lcd, 0, 0, true);
        qp_flush(lcd);
        display_dirty = false;
    }

    return true;
}
