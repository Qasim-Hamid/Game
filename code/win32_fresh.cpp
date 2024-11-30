#include <stdio.h>
#include <windows.h>

bool Running = true;

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
            } break;
 
        case WM_CLOSE:
            {
            // Fix program not exiting after being destroyed
                OutputDebugStringA("WM_CLOSE\n");
                DestroyWindow(Window);
                Running = false;
            } break;

        case WM_PAINT:
            {
                PAINTSTRUCT ps;
                HDC DC = BeginPaint(Window, &ps);
                int X = ps.rcPaint.left;
                int Y = ps.rcPaint.top;
                int Height = ps.rcPaint.bottom - ps.rcPaint.top;
                int Width = ps.rcPaint.right - ps.rcPaint.left;
                static DWORD Operation = WHITENESS; //Only initializes the first time
                PatBlt(DC, X, Y, Width, Height, Operation);
                if (Operation == WHITENESS) {
                    Operation = BLACKNESS;
                }
                else {
                    Operation = WHITENESS;
                }
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
            while(Running)
            {
                MSG *msg;
                if (PeekMessageA(msg, Window, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(msg);
                    DispatchMessageA(msg);
                }
            }
        }
        
    }

    return 0;
}
