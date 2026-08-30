# FIX

## Bugs

- [ ] Win32Window.cpp: clear GWLP_USERDATA in FreeWin32Window and null-check the window ptr in WndProc dispatch — messages queued for an already-destroyed HWND land on a recycled pool slot

## Win32 correctness

- [ ] Set DPI awareness (SetProcessDpiAwarenessContext, PER_MONITOR_AWARE_V2) before the first window; switch AdjustWindowRect -> AdjustWindowRectExForDpi; handle WM_DPICHANGED. Until then every client size and MonitorInfo.dpi/scale_factor is wrong on HiDPI displays
- [ ] Focus loss leaves keys/buttons stuck "held": on FocusChanged{focused=false} clear held/pressed/released state (or synthesize releases for everything down)
- [ ] No SetCapture/ReleaseCapture on mouse buttons — press inside, release outside = permanently stuck button

## API

- [ ] Document PollEvents lifetime + threading: returned buffer is valid only until the next PollEvents call; all platform calls must be on the main thread

## Build

- [ ] Root CMakeLists: Atreus links AtreusPlatform via both `$<TARGET_OBJECTS:AtreusPlatform>` and target_link_libraries — drop the TARGET_OBJECTS line, the link already pulls objects + usage requirements
- [ ] Add NOMINMAX and /W4 globally — /W4 would have caught the WriteFile bug and the int/u32 sign-compare in AllocWin32Window/FreeWin32Window
