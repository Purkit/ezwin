#ifndef EZWIN_API_H
#define EZWIN_API_H

#include <stdint.h>
#include "config.h"

#ifdef __cplusplus
extern "C" {
#endif

#if defined(EZWIN_IS_BUILT_AS_SHARED_LIB)
    #if defined(_WIN32)
        #define EZAPI __declspec(dllimport)
    #elif defined(__GNUC__)
        #define EZAPI
    #endif
#elif defined(EZWIN_IS_BUILT_AS_STATIC_LIB)
    #define EZAPI
#endif

//// * EZ INPUT KEY ENUMS * ////
typedef enum _KeyState { UNPRESSED = 0, PRESSED = 1 } KeyState;
typedef enum _KeyAction { KEY_PRESSED = 1, KEY_RELEASED = -1 } KeyAction;
typedef enum _ScrollDir { SCROLL_UP = 9, SCROLL_DOWN = -9 } ScrollDir;
typedef enum _KeyCode {
    EZ_KEY_UNKNOWN = 0,

    EZ_KEY_A,
    EZ_KEY_B,
    EZ_KEY_C,
    EZ_KEY_D,
    EZ_KEY_E,
    EZ_KEY_F,
    EZ_KEY_G,
    EZ_KEY_H,
    EZ_KEY_I,
    EZ_KEY_J,
    EZ_KEY_K,
    EZ_KEY_L,
    EZ_KEY_M,
    EZ_KEY_N,
    EZ_KEY_O,
    EZ_KEY_P,
    EZ_KEY_Q,
    EZ_KEY_R,
    EZ_KEY_S,
    EZ_KEY_T,
    EZ_KEY_U,
    EZ_KEY_V,
    EZ_KEY_W,
    EZ_KEY_X,
    EZ_KEY_Y,
    EZ_KEY_Z,

    EZ_KEY_0,
    EZ_KEY_1,
    EZ_KEY_2,
    EZ_KEY_3,
    EZ_KEY_4,
    EZ_KEY_5,
    EZ_KEY_6,
    EZ_KEY_7,
    EZ_KEY_8,
    EZ_KEY_9,

    EZ_KEY_F1,
    EZ_KEY_F2,
    EZ_KEY_F3,
    EZ_KEY_F4,
    EZ_KEY_F5,
    EZ_KEY_F6,
    EZ_KEY_F7,
    EZ_KEY_F8,
    EZ_KEY_F9,
    EZ_KEY_F10,
    EZ_KEY_F11,
    EZ_KEY_F12,

    EZ_KEY_SEMICOLON,     /* ; */
    EZ_KEY_EQUAL,         /* = */
    EZ_KEY_LEFT_BRACKET,  /* [ */
    EZ_KEY_BACKSLASH,     /* \ */
    EZ_KEY_RIGHT_BRACKET, /* ] */
    EZ_KEY_GRAVE_ACCENT,  /* ` */
    EZ_KEY_APOSTROPHE,
    EZ_KEY_COMMA,
    EZ_KEY_MINUS,
    EZ_KEY_PERIOD,
    EZ_KEY_SLASH,

    EZ_KEY_RIGHT_ARROW,
    EZ_KEY_LEFT_ARROW,
    EZ_KEY_DOWN_ARROW,
    EZ_KEY_UP_ARROW,

    EZ_KEY_INSERT,
    EZ_KEY_DELETE,
    EZ_KEY_HOME,
    EZ_KEY_END,
    EZ_KEY_PAGE_UP,
    EZ_KEY_PAGE_DOWN,

    EZ_KEY_ESCAPE,
    EZ_KEY_ENTER,
    EZ_KEY_TAB,
    EZ_KEY_BACKSPACE,
    EZ_KEY_SPACE,
    EZ_KEY_LEFT_SHIFT,
    EZ_KEY_LEFT_CONTROL,
    EZ_KEY_LEFT_ALT,
    EZ_KEY_LEFT_SUPER,
    EZ_KEY_RIGHT_SHIFT,
    EZ_KEY_RIGHT_CONTROL,
    EZ_KEY_RIGHT_ALT,
    EZ_KEY_RIGHT_SUPER,

    EZ_KEY_CAPS_LOCK,
    EZ_KEY_SCROLL_LOCK,
    EZ_KEY_NUM_LOCK,
    EZ_KEY_PRINT_SCREEN,
    EZ_KEY_PAUSE,

    EZ_KEY_NP_0,
    EZ_KEY_NP_1,
    EZ_KEY_NP_2,
    EZ_KEY_NP_3,
    EZ_KEY_NP_4,
    EZ_KEY_NP_5,
    EZ_KEY_NP_6,
    EZ_KEY_NP_7,
    EZ_KEY_NP_8,
    EZ_KEY_NP_9,
    EZ_KEY_NP_DECIMAL,
    EZ_KEY_NP_DIVIDE,
    EZ_KEY_NP_MULTIPLY,
    EZ_KEY_NP_SUBTRACT,
    EZ_KEY_NP_ADD,
    EZ_KEY_NP_ENTER,
    EZ_KEY_NP_EQUAL,

    EZ_KEY_COUNT
} EzKeyCode;

typedef enum _MouseButtonCode {
    EZ_MOUSE_BUTTON_UNKNOWN = 0,
    EZ_MOUSE_BUTTON_LEFT    = 1,
    EZ_MOUSE_BUTTON_RIGHT   = 2,
    EZ_MOUSE_BUTTON_MIDDLE  = 3,
    EZ_MOUSE_BUTTON_COUNT
} EzMouseButtonCode;
typedef enum _ModifierKeys {
    EZ_SHIFT,
    EZ_CONTROL,
    EZ_SUPER,
    EZ_ALT,
    EZ_CAPS_LOCK,
    EZ_NUM_LOCK
} EzModifierKeys;


//// * FUNCTION POINTER TYPES FOR CALLBACK * ////
struct EzWindow;
typedef void (*fp_resize)(EzWindow *window);
typedef void (*fp_render)(EzWindow *window, double time);
typedef void (*fp_keyEvent)(EzWindow *window, EzKeyCode key, KeyAction action);
typedef void (*fp_keyPress)(EzWindow *window, EzKeyCode key);
typedef void (*fp_keyRelease)(EzWindow *window, EzKeyCode key);

typedef void (*fp_mouseBtnEvent)(EzWindow *window, EzMouseButtonCode btn,
                                 KeyAction action);
typedef void (*fp_mouseMoveEvent)(EzWindow *window, float xpos, float ypos);
typedef void (*fp_mouseScrollEvent)(EzWindow *window, float amt, ScrollDir dir);
typedef void (*fp_windowFocusLoss)(EzWindow *window);
typedef void (*fp_windowFocusGain)(EzWindow *window);
typedef void (*fp_mouseEnter)(EzWindow *window);
typedef void (*fp_mouseLeave)(EzWindow *window);
typedef void (*fp_windowCloseBtnPress)(EzWindow *window);
typedef void (*fp_windowMaximize)(EzWindow *window);
typedef void (*fp_windowMinimize)(EzWindow *window);


//// * EZ WINDOW * ////
struct EzWindow {
    struct {
        int x;
        int y;
    } position;

    struct {
        bool limited;
        struct {
            int width;
            int height;
        } current;
        struct {
            int min_width;
            int min_height;
            int max_width;
            int max_height;
        } limits;
    } size;

    struct {
        bool resize;
        bool maximize;
        bool minimize;
        bool toggle_to_fullscreen;
        bool toggle_to_borderless;
    } can;

    struct {
        bool shouldClose;
        bool fullscreen;
        bool borderless;
        bool floating;
        bool maximized;
        bool minimized;
    } state;

    struct {

        struct {
            bool rawMotion;
            float pos_x;
            float pos_y;
            uint32_t last_click_time;
            KeyState buttonState[EZ_MOUSE_BUTTON_COUNT];
        } mouse;

        struct {
#if defined(WAYLAND_SUPPORT)
            struct {
                bool on;
                int rate;
                int delay;
            } keyRepeat;
#else
            bool autoKeyRepeat;
#endif
            uint32_t last_keyPress_time;
            uint32_t last_keyRelease_time;
            KeyState keyState[EZ_KEY_COUNT];
        } keyboard;

    } input;

    struct {
        bool visibility;
        void *image;
        float virtual_pos_x;
        float virtual_pos_y;
    } cursor;

    struct {
        fp_resize on_resize;
        fp_render on_render_signal;
        fp_keyEvent on_keyevent;
        fp_mouseBtnEvent on_mbevent;
        fp_mouseMoveEvent on_mousemove;
        fp_mouseScrollEvent on_mousescroll;
        fp_windowFocusLoss on_focusloss;
        fp_windowFocusGain on_focusgain;
        fp_mouseEnter on_mouseenter;
        fp_mouseLeave on_mouseleave;
        fp_windowCloseBtnPress on_winclosebtnclick;
        fp_windowMaximize on_maximize;
        fp_windowMinimize on_minimize;
    } callbacks;

    // X11State x11_state;
};

typedef struct EzWindow EzWindow;

struct _EzEventListeners {
    fp_resize on_resize;
    fp_render on_render_signal;
    fp_keyEvent on_keyevent;
    fp_mouseBtnEvent on_mbevent;
    fp_mouseMoveEvent on_pointermove;
    fp_mouseScrollEvent on_mousescroll;
    fp_windowFocusLoss on_focusloss;
    fp_windowFocusGain on_focusgain;
    fp_mouseEnter on_mouseenter;
    fp_mouseLeave on_mouseleave;
    fp_windowCloseBtnPress on_winclosebtnclick;
    fp_windowMaximize on_maximize;
    fp_windowMinimize on_minimize;
};

typedef struct _EzEventListeners EzEventListeners;

//// * EZ PUBLIC API * ////
int EzInitializePlatform();

// WindownManagement API:
void CreateWindowS(EzWindow *window, int width, int height, const char *title);
EzWindow *CreateWindowH(int width, int height, const char *title);
void DestroyWindow(EzWindow *window);
void PollEvents();
bool WindowShouldClose(EzWindow *window);
void SetWindowTitle(EzWindow *window, const char *title);
void SetWindowSize(EzWindow *window, int width, int height);
void GetWindowSize(EzWindow *window, int *width, int *height);
void ToggleFullScreen(EzWindow *window);
void ToggleBorderless(EzWindow *window);
void ShowCursor(EzWindow *window, bool visible);
void SetIcon(EzWindow *window, const char *iconPath);

// Input Handling API:
bool GetKeyState(EzWindow *window, EzKeyCode key);
bool GetMouseButtonState(EzWindow *window, EzMouseButtonCode button);
void GetMousePosition(EzWindow *window, float *x, float *y);

void SetKeyEventCallback(EzWindow *window, fp_keyEvent callback);
void SetMouseButtonEventCallback(EzWindow *window, fp_mouseBtnEvent callback);
void SetPointerMoveEventCallback(EzWindow *window, fp_mouseMoveEvent callback);
void SetMouseScrollEventCallback(EzWindow *window,
                                 fp_mouseScrollEvent callback);
void SetWindowReizeEventCallback(EzWindow *window, fp_resize callback);
void SetWaylandRenderSignalCallback(EzWindow *window, fp_render callback);
void SetWindowFocusLoseCallback(EzWindow *window, fp_windowFocusLoss callback);
void SetWindowFocusGainCallback(EzWindow *window, fp_windowFocusGain callback);
void SetMouseEnterCallback(EzWindow *window, fp_mouseEnter callback);
void SetMouseLeaveCallback(EzWindow *window, fp_mouseLeave callback);
void SetWindowMaximizeCallback(EzWindow *window, fp_windowMaximize callback);
void SetWindowMinimizeCallback(EzWindow *window, fp_windowMinimize callback);

void RegisterEventListeners(EzWindow *window, const EzEventListeners *listeners);

void EzFreeInternalPlatformState(EzWindow *window);
void EzFreeWindow(EzWindow *window);

// clang-format off
#define EzCreateWindow(arg, ...)          \
    _Generic(                           \
        (arg),                          \
        EzWindow *: CreateWindowS,      \
        int: CreateWindowH              \
    )                                   \
        (arg1, ##__VA_ARGS__)
// clang-format on

#ifdef __cplusplus
}
#endif

#endif // EZWIN_API_H
