#include <windows.h>


LRESULT CALLBACK
Wndproc(HWND Window,
		UINT Message,
		WPARAM WParam,
		LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{
			OutputDebugStringA("WM_SIZE");
		} break;

		case WM_DESTROY:
		{
			OutputDebugStringA("WM_DESTROY");
		} break;

		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE");
		} break;

		case WM_ACTIVATEAPP:
		{
			OutputDebugStringA("WM_ACTIVATEAPP");
		} break;

		case WM_PAINT:
		{
			PAINTSTRUCT Paint;
			HDC DeviceContext = BeginPaint(Window, &Paint);
			int X = Paint.rcPaint.left;
			int Y = Paint.rcPaint.top;
			int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
			int Width = Paint.rcPaint.right - Paint.rcPaint.left;
			static DWORD colour = WHITENESS;
			PatBlt(DeviceContext, X, Y, Width, Height, colour);
			if(colour == WHITENESS) {
				colour = BLACKNESS;
			}
			else {
				colour = WHITENESS;
			}
			EndPaint(Window, &Paint);
		} break;

		default:
		{
			/* OutputDebugStringA("Default"); */
			Result = DefWindowProc(Window, Message, WParam, LParam);
		} break;
	}

	return(Result);
}

int WINAPI 
WinMain(HINSTANCE Instance, 
		HINSTANCE hPrevInstance, 
		PSTR lpCmdLine, 
		int nCmdShow)

{
	WNDCLASSA WindowClass = {};
	WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
	WindowClass.lpfnWndProc = Wndproc;
	WindowClass.hInstance = Instance;
	/* Windowclass.hIcon; */
	WindowClass.lpszClassName = "GameWindowClass";

	if(RegisterClassA(&WindowClass))
	{
		HWND WindowHandle = 
			CreateWindowA(
				WindowClass.lpszClassName,
				"GameWindowName",
				WS_OVERLAPPEDWINDOW|WS_VISIBLE,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				CW_USEDEFAULT,
				0,
				0,
				Instance,
				0);
		if(WindowHandle != NULL) 
		{
			MSG Message;
			for(;;)
			{
				BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
				if(MessageResult > 0)
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
		}
	}
	else 
	{
	}

	return(0);
}
