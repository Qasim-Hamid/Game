#include <stdio.h>
#include <windows.h>

bool Running = true;

LRESULT CALLBACK MainWndProc(
    HWND hwnd,        // handle to wnd
    UINT uMsg,        // message identifier
    WPARAM wParam,    // first message parameter
    LPARAM lParam)    // second message parameter
{ 
 
    switch (uMsg) 
    { 
        case WM_CREATE: 
            // Initialize the wnd. 
            OutputDebugStringA("WM_CREATE\n");
            return 0; 
 
        case WM_PAINT: 
            // Paint the wnd's client area. 
            return 0; 
 
        case WM_SIZE: 
            // Set the size and position of the wnd. 
            OutputDebugStringA("WM_SIZE\n");
            return 0; 
 
        case WM_DESTROY:
            OutputDebugStringA("WM_DESTROY\n");
            return 0;

        default: 
            return DefWindowProc(hwnd, uMsg, wParam, lParam); 
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
