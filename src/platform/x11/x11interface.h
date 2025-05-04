#ifndef X11_INTERNAL
#define X11_INTERNAL

#include "x11state.h"

//// * X11 HIGH LEVEL PLATFORM INTERFACE * ////

int _x11_initialize_platform();
void _glx_create_opengl_context(X11State *x11_state);
void _glx_make_context_current(X11State *x11_state);
void _x11_create_window(X11State *x11_state);
void _x11_post_window_creation(X11State *x11_state);
void _x11_show_window(X11State *x11_state);
void _x11_clear_window(X11State *x11_state);
void _x11_set_title(X11State *x11_state, const char *title);
void _x11_close_window(X11State *x11_state);
void _x11_pump_event(X11State *x11_state);
void _x11_wait_for_event(X11State *x11_state);
void _x11_platform_state_clean_up(X11State *x11_state);

#endif // X11_INTERNAL
