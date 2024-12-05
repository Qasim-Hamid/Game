#include <stdio.h>
#include <windows.h>
#include <stdint.h>

static bool Running = true;
static int BitmapWidth;
static int BitmapHeight;
static BITMAPINFO BitmapInfo;
static void *BitmapMemory;
static int BytesPerPixel = 4;

static void GenerateBitmapImage(int XOffset, int YOffset)
{
    int Pitch = BitmapWidth*BytesPerPixel;
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for (int Y = 0;
            Y < BitmapHeight;
            Y++)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for (int X = 0;
                X < BitmapWidth;
                X++)
        {
            uint8_t blue = X + (uint8_t)XOffset;
            uint8_t green = Y + (uint8_t)YOffset;

            *Pixel++ = ((green << 8) | blue);
            /* //Blue */
            /* *Pixel = (uint8_t)X + (uint8_t)XOffset; */
            /* ++Pixel; */
            
            /* //Green */
            /* *Pixel = (uint8_t)Y + (uint8_t)YOffset; */
            /* ++Pixel; */

            /* //Red */
            /* *Pixel = 0; */
            /* ++Pixel; */

            /* //Padding */
            /* *Pixel = 0; */
            /* ++Pixel; */
        }
        Row += Pitch;
    }
}

static void Win32ResizeDIBSection(int Width, int Height) 
{
    if (BitmapMemory) {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (BitmapWidth*BitmapHeight)* BytesPerPixel; 
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    static uint8_t xoff = 0;
    static uint8_t yoff = 0;
    GenerateBitmapImage(0, 0);
}

static void Win32UpdateWindow(HDC DeviceContext, RECT WindowRect)
{
    int WindowWidth = WindowRect.right - WindowRect.left;
    int WindowHeight = WindowRect.bottom - WindowRect.top;
	StretchDIBits(
		DeviceContext,
		0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
		BitmapMemory,
		&BitmapInfo,
		DIB_RGB_COLORS,
        SRCCOPY);
}



LRESULT CALLBACK MainWndProc(
    HWND Window,        // handle to wnd
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 
    switch (uMsg) 
    { 
        case WM_CREATE: 
            // Initialize the wnd. 
            {
                OutputDebugStringA("WM_CREATE\n");
            } break;

        case WM_SIZE: 
            {
            // Set the size and position of the wnd. 
                OutputDebugStringA("WM_SIZE\n");
                RECT rect;
                GetClientRect(Window, &rect);
                int width = rect.right - rect.left;
                int height = rect.bottom - rect.top;
                Win32ResizeDIBSection(width, height);
            } break;
 
        case WM_CLOSE:
            {
                OutputDebugStringA("WM_CLOSE\n");
                //Proper way of destroying and stopping a window
                DestroyWindow(Window);
                //Game stops when running stops
                Running = false;
            } break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC DC = BeginPaint(Window, &ps);
                RECT rect;
                GetClientRect(Window, &rect);
                Win32UpdateWindow(DC, rect);
                EndPaint(Window, &ps); 
            } break;
                
        default: 
            {
                return DefWindowProc(Window, uMsg, wParam, lParam); 
            } break;
    } 
    return 0; 
} 

int WINAPI wWinMain(HINSTANCE hInstance, 
                    HINSTANCE hPrevInstance, 
                    PWSTR pCmdLine, 
                    int nCmdShow)
{
    WNDCLASSEXA wnd = {};
    wnd.cbSize = sizeof(WNDCLASSEX);
    wnd.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	wnd.lpfnWndProc = MainWndProc;
    wnd.hInstance = hInstance;
    wnd.lpszClassName = "game";

    if (RegisterClassExA(&wnd))
    {
		HWND Window = CreateWindowExA(
		0,
		wnd.lpszClassName,
		"game_wnd",
		WS_OVERLAPPEDWINDOW|WS_VISIBLE,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		0,
		0,
		hInstance,
		0);

        if (Window) {
            uint8_t xoff = 0;
            uint8_t yoff = 0;
            while(Running)
            {
                MSG msg;
                while(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessageA(&msg);
                }
                GenerateBitmapImage(xoff, yoff);
                HDC DC = GetDC(Window);
                RECT newrect;
                GetClientRect(Window, &newrect);
                Win32UpdateWindow(DC, newrect);
                xoff++;
                yoff++;
                ReleaseDC(Window, DC);
            }
        }
        
    }

    return 0;
}
