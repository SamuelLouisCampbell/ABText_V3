#include "ofMain.h"
#include "ofApp.h"
#include "../resource.h"
//External code to force Dicrete Graphics Card if available. 
extern "C"
{
	__declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
}
//========================================================================
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	ofSetupOpenGL(1280,540,OF_WINDOW);			// <-------- setup the GL context

	//TODO fix Ghosting you lazy fuck.

	HWND hwnd = ofGetWin32Window();
	HICON hMyIcon = LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON1));
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hMyIcon);

	ofRunApp(new ofApp());

}
