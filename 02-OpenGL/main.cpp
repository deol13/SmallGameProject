//--------------------------------------------------------------------------------------
// BTH - Stefan Petersson 2014.
//--------------------------------------------------------------------------------------
#include <windows.h>
#include <fstream>
#include <ctime>
#include <gl/glew.h>
#include <gl/GL.h>
#include "Render.h"
#include "lua.hpp"
#include "GuiManager.h"
#include <cstdio>
#include <sstream>
#include "GameState.h"
#include "MapEditor.h"
//#include <vld.h> 
#define GLM_FORCE_RADIANS
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include "Audio.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

HWND InitWindow( HINSTANCE hInstance );
LRESULT CALLBACK WndProc( HWND, UINT, WPARAM, LPARAM );
HGLRC CreateOpenGLContext( HWND wndHandle );

GLuint gVertexBuffer = 0;
GLuint gVertexAttribute = 0;
GLuint gShaderProgram = 0;
GLuint bth_tex = 0;

bool isQuitting = false;
bool mDepthTest = false;
bool mapEditInit = false;
bool playMapEditor = false;

const double FPSLOCK = 60.0;
int FPScount = 0;
clock_t start = clock();
clock_t currentFrame;

enum State { GAMESTATE, MENUSTATE, MAPEDITSTATE };

void SetViewport() {
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

}
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow ) {
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	MSG msg = { 0 };
	HWND wndHandle = InitWindow( hInstance ); //1. Skapa f�nster
	if( wndHandle ) {
		HDC hDC = GetDC( wndHandle );
		HGLRC hRC = CreateOpenGLContext( wndHandle ); //2. Skapa och koppla OpenGL context

		glewInit(); //3. Initiera The OpenGL Extension Wrangler Library (GLEW)

		glDepthMask( GL_TRUE );
		glDepthFunc( GL_LEQUAL );

		SetViewport(); //4. S�tt viewport

		State playState = MENUSTATE;
		bool initState = false;
		bool continueState = false;

		GameState* gameState = new GameState(WINDOW_WIDTH, WINDOW_HEIGHT);
		MapEditor* mapEdit = new MapEditor();

		GuiManager* mGUI = new GuiManager(WINDOW_WIDTH, WINDOW_HEIGHT);
		mGUI->startMenuR();
		mGUI->checkContinueButton();

		ShowWindow( wndHandle, nCmdShow );
		//init music
		Audio::getAudio().init(1.0f, 1.0f, 1.0f, true, true, true);
		Audio::getAudio().playMusic(0);

		start = std::clock();

		while (WM_QUIT != msg.message && !isQuitting) {
			currentFrame = std::clock();
			switch( playState ) {
			case MENUSTATE:
				//Play the music
				Audio::getAudio().playMusic(0);
				Audio::getAudio().update(1);

				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					if (msg.message == WM_LBUTTONDOWN)
					{
						POINT newMpos;
						GetCursorPos(&newMpos);
						ScreenToClient(wndHandle, &newMpos);
						float screenX = (newMpos.x * 2.0f / WINDOW_WIDTH) -1.0f;
						float screenY = -(newMpos.y * 2.0f / WINDOW_HEIGHT) +1.0f;
						int tmp = mGUI->mouseClick(screenX, screenY);

						if (tmp == 1) //New game
						{
							playState = GAMESTATE;
							initState = true;
							mGUI->state = 1;
						}
						else if (tmp == 2) //Map creation
						{
							playState = MAPEDITSTATE;
							initState = false;
							mGUI->state = 1;
						}
						else if (tmp == 3) //Exit
						{
							isQuitting = true;
						}
						else if (tmp == 4) //Continue
						{
							playState = GAMESTATE;
							continueState = true;
							mGUI->state = 1;
						}
					}
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				glDisable(GL_DEPTH_TEST);
				mGUI->update();

				break;
			case GAMESTATE:
				if(initState)
				{
					gameState->init(WINDOW_WIDTH, WINDOW_HEIGHT, playMapEditor);
					initState = false;
					if (playMapEditor)
					{
						gameState->setMapEditorMap();
					}
				}
				else if (continueState)
				{
					gameState->continueInit(WINDOW_WIDTH, WINDOW_HEIGHT);
					continueState = false;
				}

				glEnable(GL_DEPTH_TEST);

				if( PeekMessage( &msg, nullptr, 0, 0, PM_REMOVE ) ) 
				{
					if (gameState->guiState() < 3 && gameState->getShopState() == 0)
					{
						switch (msg.message) {
						case  WM_LBUTTONDOWN:
						{
							POINT newMpos;
							GetCursorPos(&newMpos);
							ScreenToClient(wndHandle, &newMpos);
							float posX = 2 * newMpos.x / (float)WINDOW_WIDTH - 1;
							float posY = 2 * newMpos.y / (float)WINDOW_HEIGHT - 1;
							gameState->leftMouseClick(posX, posY);
							break;
						}
						case WM_KEYDOWN:
						{
							WPARAM param = msg.wParam;
							if(param == VK_SPACE)
							{
								gameState->playerAttack();
							} else 
							{
								char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);
								gameState->keyDown(c);
							}
							break;
						}

						case WM_KEYUP:
						{
							WPARAM param = msg.wParam;
							char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);
							gameState->keyUp(c);
							break;
						}
						}
						//glEnable(GL_DEPTH_TEST);
						gameState->update();
						glDisable(GL_DEPTH_TEST);
						gameState->uiUpdate();
						glEnable(GL_DEPTH_TEST);
					}
					else
					{
						switch (msg.message) {
						case WM_LBUTTONDOWN:
						{
							POINT newMpos;
							GetCursorPos(&newMpos);
							ScreenToClient(wndHandle, &newMpos);
							float screenX = (newMpos.x * 2.0f / WINDOW_WIDTH) - 1.0f;
							float screenY = -(newMpos.y * 2.0f / WINDOW_HEIGHT) + 1.0f;
							int tmp = 0;
							tmp = gameState->screenClickesOn(screenX, screenY);

							if (tmp == 1)
								gameState->maxHeal();
							else if (tmp == 2)
							{
								playState = MENUSTATE;
								gameState->clean();
								mGUI->checkContinueButton();
							}
							else if (tmp == 5) //Cost restart
							{
								gameState->cleanedOnDefeat();
								gameState->costLoad();
							}
							else if (tmp == 6) //Free restart
							{
								gameState->cleanedOnDefeat();
								gameState->freeLoad();
							}
							break;
						}
						}
						glDisable(GL_DEPTH_TEST);
						if (playState != MENUSTATE)
							gameState->uiUpdate();
						glEnable(GL_DEPTH_TEST);
					}
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					//glEnable(GL_DEPTH_TEST);
					gameState->update();
					glDisable(GL_DEPTH_TEST);
					gameState->uiUpdate();
					glEnable(GL_DEPTH_TEST);
				}
				
				//Should switch back to main menu
				/*if(gameState->getState() == 1)
				{
					gameState->clean();
					initState = true;
					playState = MENUSTATE;
				}*/
				while( std::clock() - currentFrame < CLOCKS_PER_SEC / FPSLOCK ) {}			//comment out for unlimited frames
				break;
			case MAPEDITSTATE:
				if (!mapEditInit)
				{
					mapEdit->loadStart();
					mapEditInit = true;
				}
				if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
				{
					switch (msg.message) {
					case WM_LBUTTONDOWN:
					{
						POINT newMpos;
						GetCursorPos(&newMpos);
						ScreenToClient(wndHandle, &newMpos);

						int tmp = mapEdit->mouseClick(newMpos.x, newMpos.y);
						if (tmp == 2) //Back
						{
							playState = MENUSTATE;
							mapEdit->clean(true);
						}
						if (tmp == 4) //Back
						{
							playMapEditor = true;
							playState = GAMESTATE;
							initState = true;
						}
						break;
					}
					case WM_RBUTTONDOWN:
					{
						POINT newMpos;
						GetCursorPos(&newMpos);
						ScreenToClient(wndHandle, &newMpos);

						mapEdit->removeAnObject(newMpos.x, newMpos.y);
						break;
					}
					case WM_KEYDOWN:
					{
						WPARAM param = msg.wParam;
						char c = MapVirtualKey(param, MAPVK_VK_TO_CHAR);

						mapEdit->selectObject(c);
						break;
					}
					}

					if (mapEditInit)
						mapEdit->update();
				}
				else
					mapEdit->update();

				TranslateMessage(&msg);
				DispatchMessage(&msg);
				break;
			default:

				break;
			}

			SwapBuffers( hDC ); //10. V�xla front- och back-buffer
			FPScount++;
			
			if( ( std::clock() - start ) / ( double )CLOCKS_PER_SEC > 1 ) {
				start = std::clock();
				std::string s = std::to_string( FPScount );
				FPScount = 0;
				std::wstring stemp = std::wstring( s.begin(), s.end() );
				LPCWSTR sw = stemp.c_str();
				SetWindowText( wndHandle, sw );
			}
			
		}

		delete mGUI;
		delete gameState;
		wglMakeCurrent( NULL, NULL );
		ReleaseDC( wndHandle, hDC );
		wglDeleteContext( hRC );
		DestroyWindow( wndHandle );
	}
	return ( int )msg.wParam;
}

HWND InitWindow( HINSTANCE hInstance ) {
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof( WNDCLASSEX );
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = L"BTH_GL_DEMO";
	if( !RegisterClassEx( &wcex ) )
		return false;

	RECT rc = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );

	HWND handle = CreateWindow(
		L"BTH_GL_DEMO",
		L"BTH OpenGL Demo",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		rc.right - rc.left,
		rc.bottom - rc.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr );

	return handle;
}

LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam ) {
	switch( message ) {
	case WM_DESTROY:
		PostQuitMessage( 0 );
		break;
	}

	return DefWindowProc( hWnd, message, wParam, lParam );
}

HGLRC CreateOpenGLContext( HWND wndHandle ) {
	//get handle to a device context (DC) for the client area
	//of a specified window or for the entire screen
	HDC hDC = GetDC( wndHandle );

	//details: http://msdn.microsoft.com/en-us/library/windows/desktop/dd318286(v=vs.85).aspx
	static  PIXELFORMATDESCRIPTOR pixelFormatDesc =
	{
		sizeof( PIXELFORMATDESCRIPTOR ),    // size of this pfd  
		1,                                // version number  
		PFD_DRAW_TO_WINDOW |              // support window  
		PFD_SUPPORT_OPENGL |              // support OpenGL  
		PFD_DOUBLEBUFFER |                // double buffered
		//PFD_DEPTH_DONTCARE,               // disable depth buffer <-- added by Stefan
		PFD_TYPE_RGBA,                    // RGBA type  
		32,                               // 32-bit color depth  
		0, 0, 0, 0, 0, 0,                 // color bits ignored  
		0,                                // no alpha buffer  
		0,                                // shift bit ignored  
		0,                                // no accumulation buffer  
		0, 0, 0, 0,                       // accum bits ignored  
		//0,                                // 0-bits for depth buffer <-- modified by Stefan      
		0,                                // no stencil buffer  
		0,                                // no auxiliary buffer  
		PFD_MAIN_PLANE,                   // main layer  
		0,                                // reserved  
		0, 0, 0                           // layer masks ignored  
	};

	//attempt to match an appropriate pixel format supported by a
	//device context to a given pixel format specification.
	int pixelFormat = ChoosePixelFormat( hDC, &pixelFormatDesc );

	//set the pixel format of the specified device context
	//to the format specified by the iPixelFormat index.
	SetPixelFormat( hDC, pixelFormat, &pixelFormatDesc );

	//create a new OpenGL rendering context, which is suitable for drawing
	//on the device referenced by hdc. The rendering context has the same
	//pixel format as the device context.
	HGLRC hRC = wglCreateContext( hDC );

	//makes a specified OpenGL rendering context the calling thread's current
	//rendering context. All subsequent OpenGL calls made by the thread are
	//drawn on the device identified by hdc. 
	wglMakeCurrent( hDC, hRC );

	return hRC;
}