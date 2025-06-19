#include <windows.h>
#include <stdint.h>

//TODO: Global for Now
static bool Running;
static BITMAPINFO BitmapInfo; 
static void *BitmapMemory;
static int BitmapWidth;
static int BitmapHeight;
static int BytesPerPixel = 4;

static void
RenderWeirdGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;
    //Distance from start to end of row in bytes
    int Pitch = Width*BytesPerPixel;
    //Moves by 1 byte since we are casting it to a single byte
    uint8_t *Row = (uint8_t *)BitmapMemory;
    for (int y = 0; y < BitmapHeight; ++y)
    {
        uint32_t *Pixel = (uint32_t *)Row;
        for(int x = 0; x < BitmapWidth; ++x)
        {
            /*                  0   1   2   3   
             * Pixel in memory: 00  00  00  00
             * Expected:        RR  GG  BB  xx
             * 0xRRGGBB00
             *
             * But x64 is Little Endian
             * Pixel in memory: 00  00  00  00
             * Result:          xx  BB  GG  RR
             * 0xXXBBGGRR
             *
             * But Windows wanted the memory to look like it was the in 
             * the right order when looking at in the register of the CPU
             *
             * SO THEY DEFINED THE MEMORY ORDER TO BE BACKWARDS
             * 0xXXRRGGBB
             *
             * AND SINCE IT IS LITTLE ENDIAN WE LOAD FROM THE BACK TO THE FRONT
             * Pixel in memory: 00  00  00  00
             * Result:          BB  GG  RR  xx
             *
             */

            uint8_t blue = (uint8_t)(x + XOffset);
            uint8_t green = (uint8_t)(y + YOffset);
            uint8_t red = 0;
            *Pixel++ = (red << 16 | green << 8 | blue);

        }

        Row += Pitch;
    }
}

static void
Win32ResizeDIBSection(int Width, int Height)
{

    //TODO: Free Dibsection
    if(BitmapMemory)
    {
        //PAGE_NOACCESS -> Disables access to this piece of memory, good for debugging to see if
        //a pointer is trying to access 'freed' memory 
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
	}
    BitmapWidth = Width;
    BitmapHeight = Height;

	BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
	BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    //Start buffer at top since negative
	BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
	BitmapInfo.bmiHeader.biPlanes = 1;
	BitmapInfo.bmiHeader.biBitCount = 32;
	BitmapInfo.bmiHeader.biCompression = BI_RGB;
	/* BitmapInfo.bmiHeader.biSizeImage = 0; */
	/* BitmapInfo.bmiHeader.biXPelsPerMeter = 0; */
	/* BitmapInfo.bmiHeader.biYPelsPerMeter = 0; */
	/* BitmapInfo.bmiHeader.biClrUsed = 0; */
	/* BitmapInfo.bmiHeader.biClrImportant = 0; */

    //leave 8 bits for padding since x86 suffers penalty for unaligned accessing
    //i.e 24 bits = 3 bytes -> x86 suffers since this is not every 4 bytes. 
    //So we make it 4 bytes -> 32 bits.
    //32 bits per pixel
    int BitmapMemorySize = (Width*Height)*BytesPerPixel;
	//Gives memory in pages of 4k bytes
    //MEM_COMMIT -> We want it right now, so start tracking it 
    //MEM_RESERVE -> were gonna use it, but dont track it until we actually use it
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

}

static void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
    //Converting 2d bitmap to 1d memory
    //Pitch -> Bytes to get to next row in memory
    //Stride -> 
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
	StretchDIBits(DeviceContext, 
				  /* X, Y, Width, Height, */ 
				  /* X, Y, Width, Height, */ 
                  0, 0, BitmapWidth, BitmapHeight,
                  0, 0, WindowWidth, WindowHeight,
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
            //Windows is top-down, so the height is 0 at the top,
            //and is at it's max value at the bottom
            //thus we subtract top from bottom to get the height
            //The x is left to right
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

			RECT ClientRect = {};
            GetClientRect(Window, &ClientRect);

            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
		HWND Window = 
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
		if(Window != NULL) 
		{
            int XOffset = 0;
            int YOffset = 0;
            Running = true;
            while(Running)
			{
                MSG Message;
                if (Message.message == WM_QUIT)
                {
                    Running = false;
                }

                while(PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                {
					TranslateMessage(&Message);
					DispatchMessage(&Message);
                }
                RenderWeirdGradient(XOffset, YOffset);

                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);

                ++XOffset;
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
