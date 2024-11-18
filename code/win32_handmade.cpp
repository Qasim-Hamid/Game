#include <windows.h>

LRESULT CALLBACK WindowProc(
	HWND Window,
	UINT Message,
	WPARAM WParam,
	LPARAM LParam)
{
    LRESULT Result = 0;

	switch(Message)
	{
        case WM_SIZE:
            {
                OutputDebugStringA("WM_SIZE\n");
            } break;

        case WM_DESTROY:
            {
                OutputDebugStringA("WM_DESTROY\n");
            } break;

        case WM_CLOSE:
            {
                OutputDebugStringA("WM_CLOSE\n");
            } break;

        case WM_ACTIVATEAPP:
            {
                OutputDebugStringA("WM_ACTIVATEAPP\n");
            } break;

        case WM_PAINT:
            {
                PAINTSTRUCT Paint;
                HDC DeviceContext = BeginPaint(Window, &Paint);
                int X = Paint.rcPaint.left;
                int Y = Paint.rcPaint.top;
                int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
                int Width = Paint.rcPaint.right - Paint.rcPaint.left;
                static DWORD Operation = WHITENESS;
                PatBlt(DeviceContext, X, Y, Width, Height, Operation);
                if(Operation == WHITENESS)
                {
                    Operation = BLACKNESS;
                }
                else 
                {
                    Operation = WHITENESS;
                }
                EndPaint(Window, &Paint);
            } break;

        default:
            {
                /* OutputDebugStringA("default\n") */
                Result =  DefWindowProc(Window, Message, WParam, LParam); 
                //^-- Windows default callback
            } break;
	}

    return(Result);
}

int CALLBACK 
WinMain(HINSTANCE Instance, 
        HINSTANCE PrevInstance,
        PSTR CommandLine, 
        int ShowCode)
{
    WNDCLASSA WindowClass = {};

    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = WindowProc; //Handles messages coming from windows
	WindowClass.hInstance = Instance;
    /* WindowClass.hIcon; */
	WindowClass.lpszClassName = "HandmadeHeroWindowClass";

	if (RegisterClassA(&WindowClass))
    {
        HWND WindowHandle =
        CreateWindowExA(
            0,
            WindowClass.lpszClassName,
            "HandMade Hero",
            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            0,
            0,
            Instance,
            0);
        if(WindowHandle)
        {
            //Loop through windows messages that are queud up
			MSG Message;
            for(;;)
            {
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
                if (MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                else
                {
                    break;
                }
            }
		} 
        else
        {
            //Error handling
        }
	}
    else
    {
        //Error handling
    }
	return(0);
}
