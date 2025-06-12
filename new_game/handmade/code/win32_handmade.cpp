#include <windows.h>

//TODO: Global for Now
static bool Running;
static BITMAPINFO BitmapInfo; 
static void *BitmapMemory;
static HBITMAP BitmapHandle;
static HDC BitmapDeviceContext;

static void
Win32ResizeDIBSection(int Width, int Height)
{

    //TODO: Free Dibsection

    if(BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }

    if(!BitmapDeviceContext) 
    {
        //TODO: recreate under other circumstances?
        BitmapDeviceContext = CreateCompatibleDC(0);
    }

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = Width;
	BitmapInfo.bmiHeader.biHeight = Height;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	/* BitmapInfo.bmiHeader.biSizeImage = 0; */
	/* BitmapInfo.bmiHeader.biXPelsPerMeter = 0; */
	/* BitmapInfo.bmiHeader.biYPelsPerMeter = 0; */
	/* BitmapInfo.bmiHeader.biClrUsed = 0; */
	/* BitmapInfo.bmiHeader.biClrImportant = 0; */

	//TODO: Confirm free operation precedence


    BitmapHandle =  CreateDIBSection(
        BitmapDeviceContext, &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        0, 0);

}

static void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
	StretchDIBits(DeviceContext, 
				  X, Y, Width, Height, 
				  X, Y, Width, Height, 
                  BitmapMemory,
                  &BitmapInfo,
				  DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK
Win32Wndproc(HWND Window,
             UINT Message,
             WPARAM WParam,
             LPARAM LParam)
{
	LRESULT Result = 0;

	switch(Message)
	{
		case WM_SIZE:
		{
			RECT ClientRect = {};
            GetClientRect(Window, &ClientRect);
			int Height = ClientRect.bottom - ClientRect.top;
			int Width = ClientRect.right - ClientRect.left;
            Win32ResizeDIBSection(Width, Height);
			OutputDebugStringA("WM_SIZE\n");
		} break;

		case WM_DESTROY:
		{
            //TODO: Handle with message to user
			OutputDebugStringA("WM_DESTROY\n");
            Running = false;
		} break;

		case WM_CLOSE:
		{
			OutputDebugStringA("WM_CLOSE\n");
            //TODO: Handle as an error - recreate window?
            Running = false;
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
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
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
	WindowClass.lpfnWndProc = Win32Wndproc;
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
            Running = true;
            while(Running)
			{
                MSG Message;
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
