/*
 * aphelion_styling.h
 *
 * Pure C implementation of the Aphelion styling primitives.
 *
 * This library reproduces, in portable C99, the token and QSS generation
 * logic found in the Python package ``aphelion_styling``:
 *
 *   * theme token storage (``theme_tokens.py``)
 *   * built-in colour presets (``theme_tokens.py``)
 *   * runtime QSS builders (``theme_engine.py``, ``property_style.py``)
 *   * the named static QSS catalog (``theme.py``, ``component_styles.py``)
 *
 * The icon factory (``icons.py``) is deliberately excluded: it draws glyphs
 * with QPainter and therefore belongs in the Qt layer, not in a dependency-
 * free C library.
 *
 * There is no allocation here beyond what the caller provides. Every builder
 * writes into a caller-owned buffer and returns the number of bytes written
 * (excluding the terminating NUL), or the number of bytes that *would* have
 * been written when the buffer is too small.
 */

#ifndef APHELION_STYLING_H
#define APHELION_STYLING_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------ */
/* Export decoration                                                  */
/* ------------------------------------------------------------------ */

#if defined(_WIN32)
  #if defined(STYLING_STATIC)
    #define STYLING_API
  #elif defined(STYLING_BUILD)
    #define STYLING_API __declspec(dllexport)
  #else
    #define STYLING_API __declspec(dllimport)
  #endif
#else
  #define STYLING_API __attribute__((visibility("default")))
#endif

/* ------------------------------------------------------------------ */
/* Sizing limits                                                      */
/* ------------------------------------------------------------------ */

/* Longest theme id / display name we will ever hold. */
#define STYLING_MAX_ID_LEN 64

/* Longest colour string (e.g. "#c8c8c8") plus NUL headroom. */
#define STYLING_MAX_COLOR_LEN 32

/* Upper bound on the number of node colour overrides per theme. */
#define STYLING_MAX_NODE_COLORS 64

/* ------------------------------------------------------------------ */
/* Token data                                                         */
/* ------------------------------------------------------------------ */

typedef struct {
    char name[STYLING_MAX_ID_LEN];
    int rgb[3];
} styling_node_color;

typedef struct {
    char theme_id[STYLING_MAX_ID_LEN];
    char display_name[STYLING_MAX_ID_LEN];

    /* String colour tokens (CSS ``#rrggbb`` values). */
    char window_bg[STYLING_MAX_COLOR_LEN];
    char panel_bg[STYLING_MAX_COLOR_LEN];
    char surface_bg[STYLING_MAX_COLOR_LEN];
    char surface_elevated[STYLING_MAX_COLOR_LEN];
    char text_primary[STYLING_MAX_COLOR_LEN];
    char text_secondary[STYLING_MAX_COLOR_LEN];
    char text_muted[STYLING_MAX_COLOR_LEN];
    char border[STYLING_MAX_COLOR_LEN];
    char border_subtle[STYLING_MAX_COLOR_LEN];
    char accent[STYLING_MAX_COLOR_LEN];
    char accent_hover[STYLING_MAX_COLOR_LEN];
    char accent_pressed[STYLING_MAX_COLOR_LEN];
    char accent_text[STYLING_MAX_COLOR_LEN];
    char menu_bg[STYLING_MAX_COLOR_LEN];
    char menu_selected[STYLING_MAX_COLOR_LEN];
    char scrollbar_track[STYLING_MAX_COLOR_LEN];
    char scrollbar_handle[STYLING_MAX_COLOR_LEN];

    /* Integer RGB tokens (node graph rendering). */
    int graph_bg_rgb[3];
    int selection_rgb[3];
    int socket_input_rgb[3];
    int socket_output_rgb[3];
    int wire_active_rgb[3];
    int node_body_rgb[3];
    int node_body_hover_rgb[3];
    int node_border_rgb[3];

    /* Per-node colour overrides (name -> RGB). */
    int node_color_count;
    styling_node_color node_colors[STYLING_MAX_NODE_COLORS];
} styling_tokens;

/* ------------------------------------------------------------------ */
/* Built-in presets                                                   */
/* ------------------------------------------------------------------ */

/* Initialise ``out`` with the default Aphelion dark theme. */
STYLING_API void styling_aphelion_dark(styling_tokens *out);

/* Cool blue variant with deeper backgrounds. */
STYLING_API void styling_midnight_blue(styling_tokens *out);

/* Neutral gray theme with a muted steel accent. */
STYLING_API void styling_graphite(styling_tokens *out);

/* Warm editorial palette inspired by grading suites. */
STYLING_API void styling_warm_studio(styling_tokens *out);

/* ------------------------------------------------------------------ */
/* Runtime QSS builders                                               */
/* ------------------------------------------------------------------ */

/*
 * Each builder renders one stylesheet from ``tokens`` into ``out``.
 * ``cap`` is the total capacity of ``out`` in bytes (including the NUL).
 *
 * Returns the number of bytes written excluding the terminating NUL. When
 * the buffer is too small, the output is truncated and the required length
 * is returned, so callers can size the buffer correctly on a second pass.
 */
STYLING_API int styling_build_main(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_dock(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_menubar(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_context_menu(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_timeline(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_properties(const styling_tokens *t, char *out, size_t cap);
STYLING_API int styling_build_preferences(const styling_tokens *t, char *out, size_t cap);

/* ------------------------------------------------------------------ */
/* Static named QSS catalog                                           */
/* ------------------------------------------------------------------ */

/* Returned strings are static storage and must not be freed. */
STYLING_API const char *styling_dark_theme(void);
STYLING_API const char *styling_shortcuts_dialog_style(void);
STYLING_API const char *styling_launcher_style(void);
STYLING_API const char *styling_bootloader_style(void);
STYLING_API const char *styling_node_search_style(void);
STYLING_API const char *styling_context_menu_style(void);
STYLING_API const char *styling_dock_style(void);
STYLING_API const char *styling_menubar_style(void);
STYLING_API const char *styling_timeline_style(void);
STYLING_API const char *styling_keyframes_panel_style(void);
STYLING_API const char *styling_log_viewer_style(void);
STYLING_API const char *styling_toolbar_style(void);
STYLING_API const char *styling_pin_bar_dialog_style(void);
STYLING_API const char *styling_status_bar_style(void);
STYLING_API const char *styling_plugin_surface_style(void);
STYLING_API const char *styling_plugin_dialog_style(void);
STYLING_API const char *styling_about_dialog_style(void);
STYLING_API const char *styling_project_settings_style(void);
STYLING_API const char *styling_export_dialog_style(void);
STYLING_API const char *styling_tracking_dialog_style(void);
STYLING_API const char *styling_media_pool_style(void);
STYLING_API const char *styling_selection_bar_style(void);
STYLING_API const char *styling_custom_node_dialog_style(void);

/* Resolved properties panel style for the default dark preset (this is the
 * value of the Python module's ``PROPERTIES_STYLE`` constant). */
STYLING_API int styling_default_properties_style(char *out, size_t cap);

#ifdef __cplusplus
}
#endif

#endif /* APHELION_STYLING_H */
