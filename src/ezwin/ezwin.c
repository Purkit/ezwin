#define EZ_EXPORT_SYMBOLS
#include <ezwin/ezwin.h>

EZAPI int EzInitializePlatform() { return 1; }

// WindownManagement API:
EZAPI void CreateWindowS(EzWindow *window, int width, int height,
                         const char *title) {}
EZAPI EzWindow *CreateWindowH(int width, int height, const char *title) {}
EZAPI void DestroyWindow(EzWindow *window) {}
EZAPI void PollEvents() {}
EZAPI bool WindowShouldClose(EzWindow *window) { return true; }
EZAPI void SetWindowTitle(EzWindow *window, const char *title) {}
EZAPI void SetWindowSize(EzWindow *window, int width, int height) {}
EZAPI void GetWindowSize(EzWindow *window, int *width, int *height) {}
EZAPI void ToggleFullScreen(EzWindow *window) {}
EZAPI void ToggleBorderless(EzWindow *window) {}
EZAPI void ShowCursor(EzWindow *window, bool visible) {}
EZAPI void SetIcon(EzWindow *window, const char *iconPath) {}

// Input Handling API:
EZAPI bool GetKeyState(EzWindow *window, EzKeyCode key) { return false; }
EZAPI bool GetMouseButtonState(EzWindow *window, EzMouseButtonCode button) {
    return false;
}
EZAPI void GetMousePosition(EzWindow *window, float *x, float *y) {}

EZAPI void SetKeyEventCallback(EzWindow *window, fp_keyEvent callback) {}
EZAPI void SetMouseButtonEventCallback(EzWindow *window,
                                       fp_mouseBtnEvent callback) {}
EZAPI void SetPointerMoveEventCallback(EzWindow *window,
                                       fp_mouseMoveEvent callback) {}
EZAPI void SetMouseScrollEventCallback(EzWindow *window,
                                       fp_mouseScrollEvent callback) {}
EZAPI void SetWindowReizeEventCallback(EzWindow *window, fp_resize callback) {}
EZAPI void SetWaylandRenderSignalCallback(EzWindow *window,
                                          fp_render callback) {}
EZAPI void SetWindowFocusLoseCallback(EzWindow *window,
                                      fp_windowFocusLoss callback) {}
EZAPI void SetWindowFocusGainCallback(EzWindow *window,
                                      fp_windowFocusGain callback) {}
EZAPI void SetMouseEnterCallback(EzWindow *window, fp_mouseEnter callback) {}
EZAPI void SetMouseLeaveCallback(EzWindow *window, fp_mouseLeave callback) {}
EZAPI void SetWindowMaximizeCallback(EzWindow *window,
                                     fp_windowMaximize callback) {}
EZAPI void SetWindowMinimizeCallback(EzWindow *window,
                                     fp_windowMinimize callback) {}

EZAPI void RegisterEventListeners(EzWindow *window,
                                  const EzEventListeners *listeners) {}

EZAPI void EzFreeInternalPlatformState(EzWindow *window) {}
EZAPI void EzFreeWindow(EzWindow *window) {}
