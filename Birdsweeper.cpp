#include <windows.h>
#include <stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

global_variable bool running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int bytesPerPixel = 4;

internal void RenderWeirdGradient(int xOffset, int yOffset) {
    int width = BitmapWidth;
    int height = BitmapHeight;
    
    int pitch = width*bytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    for (int y = 0; y < BitmapHeight; ++y) {
        uint32 *pixel = (uint32 *)Row;
        for (int x = 0; x < BitmapWidth; ++x) {
            uint8 blue = (x + xOffset);
            uint8 green = (y + yOffset);
            
            *pixel++ = ((green << 8) | blue);
        }

        Row += pitch;
    }
}

internal void Win32ResizeDIBSection(int width, int height) {
    // bulletproof this. maybe don't free first, free after, then free first if that fails

    if (BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);    
    }

    BitmapWidth = width;
    BitmapHeight = height;
    
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (width * height) * bytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // TODO probably clear this to black
}

internal void Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int x, int y, int width, int height) {
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(DeviceContext,
                  /*
                  x, y, width, height,
                  x, y, width, height,
                  */
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS,
                  SRCCOPY);
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam,
 LPARAM LParam) {
    LRESULT Result = 0;
    
    switch (Message) {
        case WM_SIZE:{
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int width = ClientRect.right - ClientRect.left;
            int height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(width, height);
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

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            
            Win32UpdateWindow(DeviceContext, &ClientRect, x, y, width, height);
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
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    HINSTANCE hInstance = Instance;
    //WindowClass. hIcon;
    //HCURSOR   hCursor;
    WindowClass.lpszClassName = "BirdsweeperWindowClass";

    if(RegisterClass(&WindowClass)) {
        HWND Window = CreateWindowExA(
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
        if (Window) {
            int xOffset = 0;
            int yOffset = 0;
            
            running = true;
            while(running) {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE)) {
                    if (Message.message == WM_QUIT) {
                        running = false;
                    }
                    
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }

                RenderWeirdGradient(xOffset, yOffset);
                
                HDC DeviceContext = GetDC(Window);;
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);
                
                ++xOffset;
            }
        } else {
            // TODO 
        }
    } else {
        // TODO
    }
  
  return(0);
}
