#ifndef X11_PLATFORM_INTERNALS
#define X11_PLATFORM_INTERNALS

#include <GL/glx.h>
#include <X11/Xlib.h>

typedef struct _X11globalState {
    bool initialize;
    Display *connection;
    Screen *defaultScreen;
    int defaultScreen_ID;
} X11GlobalState;

typedef struct _X11State {
    XVisualInfo *visual;
    GLXFBConfig bestFbc;
    GLXContext context;
    Colormap colormap;
    Window window;
    XEvent x_event;
    struct {
        Atom atomWMDeleteWindow;
        Atom motifWMHints;
        Atom wmState;
        Atom wmStateFullScreen;
    } atoms;
} X11State;

#endif // ! X11_PLATFORM_INTERNALS
