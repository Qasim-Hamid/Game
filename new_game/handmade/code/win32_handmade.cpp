#include <windows.h>

int WINAPI 
WinMain(HINSTANCE hInstance, 
		HINSTANCE hPrevInstance, 
		PSTR lpCmdLine, 
		int nCmdShow)
{
    MessageBox(0, "kaka", "caption_box", MB_OK|MB_ICONINFORMATION);
    return(0);
}
