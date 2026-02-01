#include <windows.h>

#define internal static
#define local_persist static
#define global_variable static

global_variable bool running;

internal void ResizeDIBSection() {

}

LRESULT CALLBACK MainWindowCallback(HWND Window, UINT Message, WPARAM WParam,
 LPARAM LParam) {
    LRESULT Result = 0;
    
    switch (Message) {
        case WM_SIZE:{
            ResizeDIBSection();
            OutputDebugStringA("WM_SIZE\n");
            } break;
        case WM_DESTROY: {
            running = false;
            OutputDebugStringA("WM_DESTROY\n");
            } break;
        case WM_CLOSE: {
            running = false;
            OutputDebugStringA("WM_CLOSE\n");
            } break;
        case WM_ACTIVATEAPP: {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
            } break;
        case WM_PAINT: {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int x = Paint.rcPaint.left;
            int y = Paint.rcPaint.top;
            int width = Paint.rcPaint.right - Paint.rcPaint.left;
            int height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            local_persist DWORD Operation = WHITENESS;
            PatBlt(DeviceContext, x, y, width, height, Operation);
            EndPaint(Window, &Paint);
            } break;
         default: {
         OutputDebugStringA("default\n");
        
         Result = DefWindowProc(Window, Message, WParam, LParam);
         } break;
    }

    return(Result);
}

int CALLBACK
WinMain(HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommmandLine, int ShowCode) {
    WNDCLASS WindowClass = {};

    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = MainWindowCallback;
    HINSTANCE hInstance = Instance;
    //WindowClass. hIcon;
    //HCURSOR   hCursor;
    WindowClass.lpszClassName = "BirdsweeperWindowClass";

    if(RegisterClass(&WindowClass)) {
        HWND WindowHandle = CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "Birdsweeper",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);
        if (WindowHandle) {
            running = true;
            while(running) {
                MSG Message;
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0) {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                } else {
                    break;
                }
            }
        } else {
            // TODO 
        }
    } else {
        // TODO
    }
  
  return(0);
}
