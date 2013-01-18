/*

Nick McKinney
Anywhere Piano App
Fall 2012
Capstone


  This is the basic window-wrapper, implemented using
	Win32 methods and functionalities.  The window is
	created, as well as a pointer to the rendering class,
	and methods of CGFxOpenGL are called to draw and 
	instantiate the piano keys objects.


*/



#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
/*
	Basic OpenGL and glut header files are included
	as well as the windows header file which is used
	to create the window in which the piano will be
	drawn.
*/
#include <Windows.h>
#include<GLee.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include <glut.h>

/*
	The following section of code deals with the mouse
	events and the states of the mouse click.
*/
typedef void (*EGENMOUSEFUNC)(int,int,float,float);
enum EGENMOUSESTATE{
STATE_NOME = 0,
STATE_CLICK = 1,
STATE_DOUBLECLICK = 2,
STATE_HOLD_DOWN_BUTTON = 3,
STATE_HOLD_UP_BUTTON = 4,
STATE_DOWN_BUTTON = 5,
STATE_UP_BUTTON = 6
};

/*
	Compiler directives that tell VS2010 to find
	these files and utilize them during the compiling
	process.
*/
#pragma comment(lib, "OPENGL32.lib")
#pragma comment(lib, "GLU32.lib")
#pragma comment(lib, "glut.lib")
#pragma comment(lib, "glut32.lib")

//header file for the rendering class
#include "CGFxOpenGL.h"
//values to be used during the creation of the window
bool exiting = false;
long windowWidth = 800;
long windowHeight = 600;
long windowBits = 32;
bool fullscreen = false;
HDC hDC;					//device content

/*
	Below are the variables the mouse event function 
	will use to determine the position of the cursor
	and set the state of the mouse click.
*/
POINT cursorLocation;
POINT curPos;
void setMouseFunc(EGENMOUSEFUNC mouseFunc);
void (*_eGenMouseFunc)(int,int,float,float);
int _TMP_MOUSEBUTTON;
int _TMP_MOUSESTATE;
int mouse_click_to_hold_interval = 0;


//pointer to the rendering class
CGFxOpenGL *g_glRender = NULL;

/*
	Here, the details of the screen are defined.
	The SetupPixelFormat uses the device content
	to first instantiate the device context; then,
	it allows the programmer to specify what types
	of details will be added to the window on which
	the primitives/objects will be rendered.
*/
void SetupPixelFormat(HDC hDC)
{
	int pixelFormat;

	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		32,
		0, 0, 0, 0, 0, 0,	//color bits
		0,					//no alpha buffer
		0,					//alpha bits
		0,					//no accumulation buffer
		0, 0, 0, 0,			//accumulation bits
		24,					//depth buffer
		0,					//no stencil buffer
		0,					//no aux buffer
		PFD_MAIN_PLANE,
		0,
		0, 0, 0,			//no layer, visible, damage marks
	};

	pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);
}

/*
	This is where the window for rendering is created.  It uses
	the device content as well as the pixelformat, rendering content,
	and messages provided by the user through interactions with the
	screen and i/o devices.  The initial switch statement sets up a
	block of commands to determine the appropriate action when a user
	event has been detected.  The message, of course, determines the
	programs interactions with the user.
*/
LRESULT CALLBACK MainWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static HDC hDC;
	static HGLRC hRC;
	int height, width;
	
	//dispatch msgs
	switch(uMsg)
	{
	case WM_CREATE:
		hDC = GetDC(hWnd);
		SetupPixelFormat(hDC);
		hRC = wglCreateContext(hDC);
		wglMakeCurrent(hDC, hRC);
		break;

	case WM_DESTROY:
	case WM_QUIT:
	case WM_CLOSE:
		//in all three cases, deselect rendering context and delete it
		wglMakeCurrent(hDC, NULL);
		wglDeleteContext(hRC);

		PostQuitMessage(0);
		break;
	//sets size of window through a call to the rendering class
	case WM_SIZE:
		height = HIWORD(lParam);
		width = LOWORD(lParam);

		g_glRender->SetupProjection(width, height);

		break;

	case WM_KEYDOWN:
		int fwKeys;
		LPARAM keyData;
		fwKeys = (int)wParam;		//virtual key code
		keyData = lParam;			//key data

		//checks to see which key was pressed; here, only concerned about the ESC key
		switch(fwKeys)
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		default:
			break;
		}

		break;
//==========================
//MOUSE EVENT
//==========================
	case WM_MOUSEMOVE:
		curPos.x = LOWORD(lParam);
		curPos.y = HIWORD(lParam);
		cursorLocation = curPos;
		
		switch(wParam)
		{
			case MK_LBUTTON:
				if(_eGenMouseFunc)
				{
				_TMP_MOUSESTATE = STATE_HOLD_DOWN_BUTTON;
				_TMP_MOUSEBUTTON = MK_LBUTTON;
				_eGenMouseFunc(_TMP_MOUSEBUTTON,_TMP_MOUSESTATE,curPos.x,curPos.y);
				}
			break;

			case MK_RBUTTON:
				if(_eGenMouseFunc)
				{
				_TMP_MOUSESTATE = STATE_HOLD_DOWN_BUTTON;
				_TMP_MOUSEBUTTON = MK_RBUTTON;
				_eGenMouseFunc(_TMP_MOUSEBUTTON,_TMP_MOUSESTATE,curPos.x,curPos.y);
				}
			break;
		}
		break;
//====================
//MOUSE DOWN
//====================
	case WM_RBUTTONDOWN:
		curPos.x = LOWORD(lParam);
		curPos.y = HIWORD(lParam);
		_TMP_MOUSESTATE = STATE_DOWN_BUTTON || STATE_CLICK;
		_TMP_MOUSEBUTTON = MK_RBUTTON;
		
		if(_eGenMouseFunc)
			_eGenMouseFunc(_TMP_MOUSEBUTTON,_TMP_MOUSESTATE,curPos.x,curPos.y);
		g_glRender->Render(curPos.x, curPos.y, 1);

	break;

	case WM_LBUTTONDOWN:
		curPos.x = LOWORD(lParam);
		curPos.y = HIWORD(lParam);
		_TMP_MOUSESTATE = STATE_DOWN_BUTTON || STATE_CLICK;
		_TMP_MOUSEBUTTON = MK_LBUTTON;
		
		if(_eGenMouseFunc)
			_eGenMouseFunc(_TMP_MOUSEBUTTON,_TMP_MOUSESTATE ,curPos.x,curPos.y);
		g_glRender->Render(curPos.x, curPos.y, 1);

	break;

//====================
//MOUSE UP
//====================
	case WM_RBUTTONUP :
		curPos.x = LOWORD(lParam);
		curPos.y = HIWORD(lParam);
		if ( !_TMP_MOUSESTATE ==STATE_HOLD_DOWN_BUTTON)
			_TMP_MOUSESTATE = STATE_UP_BUTTON;
		else{
			_TMP_MOUSESTATE = STATE_HOLD_UP_BUTTON;
			_TMP_MOUSEBUTTON = MK_RBUTTON;}
		if(_eGenMouseFunc)
			_eGenMouseFunc(_TMP_MOUSEBUTTON , _TMP_MOUSESTATE ,curPos.x,curPos.y);
		g_glRender->Render(curPos.x, curPos.y, 0);
		
		_TMP_MOUSESTATE = STATE_NOME;
		_TMP_MOUSEBUTTON = 0;
	break;

	case WM_LBUTTONUP :
		curPos.x = LOWORD(lParam);
		curPos.y = HIWORD(lParam);
		if ( !_TMP_MOUSESTATE ==STATE_HOLD_DOWN_BUTTON)
			_TMP_MOUSESTATE = STATE_UP_BUTTON;
		else{
			_TMP_MOUSESTATE = STATE_HOLD_UP_BUTTON;
			_TMP_MOUSEBUTTON = MK_LBUTTON;}
		if(_eGenMouseFunc)
			_eGenMouseFunc(_TMP_MOUSEBUTTON , _TMP_MOUSESTATE ,curPos.x,curPos.y);
		  g_glRender->Render(curPos.x, curPos.y, 0);

		_TMP_MOUSESTATE = STATE_NOME;
		_TMP_MOUSEBUTTON = 0;
	break;
	
	default:
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void setMouseFunc(EGENMOUSEFUNC mouseFunc)
{
	_eGenMouseFunc = mouseFunc;
	return;
}

/*
	Main, in this program, is a more broad instantiation of the
	window.  The global pointer now becomes an object of the
	CGFxOpenGL rendering class; the generalities of the window
	are established here, whereas above the details were specified.
	Most of this code is provided by the Windows.h file, therefore
	this section deals with setting values.  Device content and
	rendering content are set, and appropriate CGFxOpenGL methods
	are called to draw the piano and/or set up the camera.
*/
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEX windowClass;		//window class
	HWND		hwnd;			//window handle
	MSG			msg;			//message
	DWORD		dwExStyle;		//window extended style
	DWORD		dwStyle;		//window style
	RECT		windowRect;		

	g_glRender = new CGFxOpenGL;

	windowRect.left=(long)0;
	windowRect.right=(long)windowWidth;		//set right val to requested width
	windowRect.top=(long)0;
	windowRect.bottom=(long)windowHeight;	//set bottom val to requesetd height

	//fill out window class struct
	windowClass.cbSize		=sizeof(WNDCLASSEX);
	windowClass.style		=CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc	=MainWindowProc;
	windowClass.cbClsExtra	=0;
	windowClass.cbWndExtra	=0;
	windowClass.hInstance	=hInstance;
	windowClass.hIcon		=LoadIcon(NULL, IDI_APPLICATION);	//default icon
	windowClass.hCursor		=LoadCursor(NULL, IDC_ARROW);		//default cursor
	windowClass.hbrBackground=NULL;
	windowClass.lpszMenuName=NULL;
	windowClass.lpszClassName= L"Capstone";
	windowClass.hIconSm		=LoadIcon(NULL, IDI_WINLOGO);

	//register window class
	if(!RegisterClassEx(&windowClass))
		return 0;

	if(fullscreen)
	{
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPaperWidth =windowWidth;
		dmScreenSettings.dmPanningHeight = windowHeight;
		dmScreenSettings.dmBitsPerPel = windowBits;		//bits per pixel
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		if (ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
		{
			//setting display mode failed, switch to windowed
			MessageBox(NULL, L"Failed", NULL, MB_OK);
			fullscreen = FALSE;
		}
	}

	if(fullscreen)
	{
		dwExStyle = WS_EX_APPWINDOW;
		dwStyle = WS_POPUP;
		ShowCursor(FALSE);
	}
	else
	{
		dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
		dwStyle = WS_OVERLAPPEDWINDOW;
	}

	//adjust window to true requested size
	AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

	//class registered, create window
	hwnd = CreateWindowEx(NULL,		//extended style
			L"Capstone",				//class name
			L"Portable Piano",			//app name
			dwStyle | WS_CLIPCHILDREN |
			WS_CLIPSIBLINGS,
			0,0,						//x,y coordinate
			windowRect.right - windowRect.left,
			windowRect.bottom - windowRect.top,
			NULL,					//handle to parent
			NULL,					//handle to menu
			hInstance,				//app instance
			NULL);					//no extras

	hDC = GetDC(hwnd);

	//check if window creation failed (hwnd = NULL)
	if(!hwnd)
		return 0;

	ShowWindow(hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	g_glRender->Init();

	/*
		This is the execution loop; as long as the user
		does not exit the application, it will continually
		run.
	*/
	while(!exiting)
	{
		g_glRender->Render(curPos.x, curPos.y, 0);
		g_glRender->Prepare();
		SwapBuffers(hDC);	
		
		while (PeekMessage (&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(!GetMessage (&msg, NULL, 0, 0))
			{
				exiting = true;
				break;
			}

			TranslateMessage (&msg);
			DispatchMessage (&msg);

		}
	}

	delete g_glRender;

	if(fullscreen)
	{
		ChangeDisplaySettings(NULL, 0);
		ShowCursor(TRUE);
	}

	return (int)msg.wParam;
	}
