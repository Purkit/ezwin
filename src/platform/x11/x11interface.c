
#include "x11interface.h"
#include "../../utility/log/log.h"
#include <X11/Xlib.h>

X11GlobalState _x11_global_state = {};

static int __x11_connect_to_xserver() {
    _x11_global_state.connection = XOpenDisplay(nullptr);
    if (_x11_global_state.connection == nullptr)
        return -1;
    return 1;
}
static void __x11_setup_screen() {
    _x11_global_state.defaultScreen =
        DefaultScreenOfDisplay(_x11_global_state.connection);
    _x11_global_state.defaultScreen_ID =
        DefaultScreen(_x11_global_state.connection);
}

int _x11_initialize_platform() {
    if (_x11_global_state.initialize) {
        return 0;
    }

    if (__x11_connect_to_xserver() == -1) {
        EZ_LOG_FATAL("[X11] Failed to connect to X Server!");
        return -1;
    }

    __x11_setup_screen();

    _x11_global_state.initialize = true;

    return 1;
}

static void __glx_create_fbc(X11State *x11_state) {

    GLint glxAttribs[] = {
        GLX_X_RENDERABLE,
        True,
        GLX_DRAWABLE_TYPE,
        GLX_WINDOW_BIT,
        GLX_RENDER_TYPE,
        GLX_RGBA_BIT,
        GLX_X_VISUAL_TYPE,
        GLX_TRUE_COLOR,
        GLX_RED_SIZE,
        8,
        GLX_GREEN_SIZE,
        8,
        GLX_BLUE_SIZE,
        8,
        GLX_ALPHA_SIZE,
        8,
        GLX_DEPTH_SIZE,
        24,
        GLX_STENCIL_SIZE,
        8,
        GLX_DOUBLEBUFFER,
        True,
        None,
    };

    int fbcount;
    GLXFBConfig *fbc = glXChooseFBConfig(_x11_global_state.connection,
                                         _x11_global_state.defaultScreen_ID,
                                         glxAttribs, &fbcount);
    if (fbc == 0) {
        EZ_LOG_FATAL("Failed to retrieve framebuffer.\n");
        return;
    }
    EZ_LOG_DEBUG("Found %i matching framebuffers.\n", fbcount);

    // Pick the FB config/visual with the most samples per pixel
    EZ_LOG_DEBUG("Getting best XVisualInfo\n");
    int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;
    for (int i = 0; i < fbcount; ++i) {
        XVisualInfo *vi =
            glXGetVisualFromFBConfig(_x11_global_state.connection, fbc[i]);
        if (vi != 0) {
            int samp_buf, samples;
            glXGetFBConfigAttrib(_x11_global_state.connection, fbc[i],
                                 GLX_SAMPLE_BUFFERS, &samp_buf);
            glXGetFBConfigAttrib(_x11_global_state.connection, fbc[i],
                                 GLX_SAMPLES, &samples);

            if (best_fbc < 0 || (samp_buf && samples > best_num_samp)) {
                best_fbc      = i;
                best_num_samp = samples;
            }

            if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
                worst_fbc = i;

            worst_num_samp = samples;
        }

        XFree(vi);
    }

    x11_state->bestFbc = fbc[best_fbc];
    XFree(fbc);
}

static void __glx_create_visual(X11State *x11_state) {

    x11_state->visual = glXGetVisualFromFBConfig(_x11_global_state.connection,
                                                 x11_state->bestFbc);
    if (x11_state->visual == 0) {
        EZ_LOG_FATAL("[GLX] Failed to generate visual !\n");
        return;
    }

    if (_x11_global_state.defaultScreen_ID != x11_state->visual->screen) {
        EZ_LOG_FATAL("ScreenID does not match visual->screen.");
        return;
    }
}

void _x11_create_window(X11State *x11_state) {
    __glx_create_fbc(x11_state);
    __glx_create_visual(x11_state);

    x11_state->colormap =
        XCreateColormap(_x11_global_state.connection,
                        RootWindow(_x11_global_state.connection,
                                   _x11_global_state.defaultScreen_ID),
                        x11_state->visual->visual, AllocNone);

    XSetWindowAttributes windowAttribs = {};
    windowAttribs.border_pixel     = BlackPixel(_x11_global_state.connection,
                                                _x11_global_state.defaultScreen_ID);
    windowAttribs.background_pixel = WhitePixel(
        _x11_global_state.connection, _x11_global_state.defaultScreen_ID);
    windowAttribs.override_redirect = False;
    windowAttribs.colormap          = x11_state->colormap;
    windowAttribs.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask |
                               ButtonPressMask | ButtonReleaseMask |
                               PointerMotionMask;
    /*windowAttribs.event_mask =
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask | PointerMotionMask |
        PointerMotionHintMask | Button1MotionMask | Button2MotionMask |
        Button3MotionMask | Button4MotionMask | Button5MotionMask |
        ButtonMotionMask | KeymapStateMask | ExposureMask |
        VisibilityChangeMask | StructureNotifyMask | ResizeRedirectMask |
        SubstructureNotifyMask | SubstructureRedirectMask | FocusChangeMask |
        PropertyChangeMask | ColormapChangeMask | OwnerGrabButtonMask;*/

    int width  = 1000;
    int height = 800;
    x11_state->window =
        XCreateWindow(_x11_global_state.connection,
                      RootWindow(_x11_global_state.connection,
                                 _x11_global_state.defaultScreen_ID),
                      100, 100, width, height, 0, x11_state->visual->depth,
                      InputOutput, x11_state->visual->visual,
                      CWBackPixel | CWBorderPixel | CWEventMask |
                          CWOverrideRedirect | CWColormap,
                      &windowAttribs);

    XStoreName(_x11_global_state.connection, x11_state->window, "XLIB BEST");

    XClearWindow(_x11_global_state.connection, x11_state->window);
}

void _x11_post_window_creation(X11State *x11_state) {

    // These atoms are basically window manager's message types !!
    x11_state->atoms.atomWMDeleteWindow =
        XInternAtom(_x11_global_state.connection, "WM_DELETE_WINDOW", False);
    // tell the server that I want to recieve WM_DELETE_WINDOW event from the
    // window manager as a client message !!
    XSetWMProtocols(_x11_global_state.connection, x11_state->window,
                    &x11_state->atoms.atomWMDeleteWindow, 1);

    x11_state->atoms.motifWMHints =
        XInternAtom(_x11_global_state.connection, "_MOTIF_WM_HINTS", true);
    x11_state->atoms.wmState =
        XInternAtom(_x11_global_state.connection, "_NET_WM_STATE", False);
    x11_state->atoms.wmStateFullScreen = XInternAtom(
        _x11_global_state.connection, "_NET_WM_STATE_FULLSCREEN", True);
}

void _glx_create_opengl_context(X11State *x11_state) {

    typedef GLXContext (*glXCreateContextAttribsARBProc)(
        Display *, GLXFBConfig, GLXContext, Bool, const int *);
    glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
    glXCreateContextAttribsARB =
        (glXCreateContextAttribsARBProc)glXGetProcAddressARB(
            (const GLubyte *)"glXCreateContextAttribsARB");
    if (glXCreateContextAttribsARB == 0) {
        EZ_LOG_FATAL(
            "Failed to load glXCreateContextAttribsARB(). OpenGL Context "
            "can't be obtained.");
        return;
    }

    const int OPENGL_CONTEXT_MAJOR_VERSION = 4;
    const int OPENGL_CONTEXT_MINOR_VERSION = 5;

#ifdef DEBUG_MODE
    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB,
        OPENGL_CONTEXT_MAJOR_VERSION,
        GLX_CONTEXT_MINOR_VERSION_ARB,
        OPENGL_CONTEXT_MINOR_VERSION,
        GLX_CONTEXT_FLAGS_ARB,
        GLX_CONTEXT_DEBUG_BIT_ARB,
        GLX_CONTEXT_PROFILE_MASK_ARB,
        GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None,
    };
#else
    int context_attribs[] = {
        GLX_CONTEXT_MAJOR_VERSION_ARB,
        OPENGL_CONTEXT_MAJOR_VERSION,
        GLX_CONTEXT_MINOR_VERSION_ARB,
        OPENGL_CONTEXT_MINOR_VERSION,
        GLX_CONTEXT_PROFILE_MASK_ARB,
        GLX_CONTEXT_CORE_PROFILE_BIT_ARB,
        None,
    };
#endif

    x11_state->context = glXCreateContextAttribsARB(
        _x11_global_state.connection, x11_state->bestFbc, 0, true,
        context_attribs);

    XSync(_x11_global_state.connection, False);

    if (!glXIsDirect(_x11_global_state.connection, x11_state->context)) {
        EZ_LOG_DEBUG("Indirect GLX rendering context obtained.\n");
    } else {
        EZ_LOG_DEBUG("Direct GLX rendering context obtained.\n");
    }
}

void _glx_make_context_current(X11State *x11_state) {

    glXMakeCurrent(_x11_global_state.connection, x11_state->window,
                   x11_state->context);

    EZ_LOG_DEBUG("GL Vendor: %s\n", glGetString(GL_VENDOR));
    EZ_LOG_DEBUG("GL Renderer: %s\n", glGetString(GL_RENDERER));
    EZ_LOG_DEBUG("GL Version: %s\n", glGetString(GL_VERSION));
}

void _x11_show_window(X11State *x11_state) {
    /*XMapWindow(x11_state->connection, x11_state->window);*/
    XMapRaised(_x11_global_state.connection, x11_state->window);
}

void _x11_clear_window(X11State *x11_state) {
    XClearWindow(_x11_global_state.connection, x11_state->window);
}

void _x11_set_title(X11State *x11_state, const char *title) {
    XStoreName(_x11_global_state.connection, x11_state->window, title);
}
void _x11_close_window(X11State *x11_state) {
    // communicate with the window manager to tell it
    // eto send a window clos event
}

static void __handle_event(X11State *x11_state) {
    switch (x11_state->x_event.type) {
        case KeyPress: {
            EZ_LOG_DEBUG("Key Pressed!\n");
            break;
        }

        case KeyRelease: {
            EZ_LOG_DEBUG("Key Released!\n");
            break;
        }

        case ButtonPress: {
            if (x11_state->x_event.xbutton.button == 1) {
            }

            if (x11_state->x_event.xbutton.button == 2) {
            }

            if (x11_state->x_event.xbutton.button == 3) {
            }

            if (x11_state->x_event.xbutton.button == 4) {
            }

            if (x11_state->x_event.xbutton.button == 5) {
            }

            break;
        }

        case ButtonRelease: {
            break;
        }

        case MotionNotify: {
            break;
        }

        case Expose: // * Window Resize Event
        {
            break;
        }

        case ClientMessage:
            if (x11_state->x_event.xclient.data.l[0] ==
                (long)x11_state->atoms.atomWMDeleteWindow)
                /*m_shouldClose = true;*/
                break;

        case DestroyNotify: break;
    }

} // !handle_event()

void _x11_pump_event(X11State *x11_state) {
    while (XPending(_x11_global_state.connection)) {
        XNextEvent(_x11_global_state.connection, &x11_state->x_event);
        __handle_event(x11_state);
    }
}

void _x11_wait_for_event(X11State *x11_state) {
    XNextEvent(_x11_global_state.connection, &x11_state->x_event);
    __handle_event(x11_state);
}
void _x11_platform_state_clean_up(X11State *x11_state) {
    if (x11_state->context)
        glXDestroyContext(_x11_global_state.connection, x11_state->context);
    if (x11_state->visual)
        XFree(x11_state->visual);
    if (x11_state->window != 0) {
        XUnmapWindow(_x11_global_state.connection, x11_state->window);
        XDestroyWindow(_x11_global_state.connection, x11_state->window);
    }
    if (x11_state->colormap)
        XFreeColormap(_x11_global_state.connection, x11_state->colormap);
    if (_x11_global_state.connection)
        XCloseDisplay(_x11_global_state.connection);
}
