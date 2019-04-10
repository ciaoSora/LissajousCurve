#include <windows.h>
#include <windowsx.h>
#include <thread>
#include "Utility.h"
#include "Lissajou.h"
#include "UIController.h"

const char APP_NAME[] = "Lissajou";
extern const int WIDTH = 800, HEIGHT = 600;

// global declarations
LPDIRECT3D9 d3d;								// the pointer to our Direct3D interface
LPDIRECT3DDEVICE9 d3ddev;						// the pointer to the device class

												// this is the function that puts the 3D models into video RAM
void init_graphics() {
	Lissajou::Init();
}

// this function initializes and prepares Direct3D for use
void initD3D(HWND hwnd) {
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	D3DPRESENT_PARAMETERS d3dpp;

	ZeroMemory(&d3dpp, sizeof(d3dpp));

	d3dpp.Windowed = true;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = hwnd;
	d3dpp.BackBufferFormat = D3DFMT_X8R8G8B8;
	d3dpp.BackBufferWidth = WIDTH;
	d3dpp.BackBufferHeight = HEIGHT;
	d3dpp.EnableAutoDepthStencil = true;        // Manage SetDepthStencil for us
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;	// 16-bit pixel format for the z-buffer

												// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

	// Create resources
	init_graphics();

	d3ddev->SetRenderState(D3DRS_LIGHTING, false);    		// turn off the 3D lighting
															//d3ddev->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);	// both sides of the triangles
	d3ddev->SetRenderState(D3DRS_ZENABLE, true);			// turn on the z-buffer

	D3DXMATRIX matProj;
	D3DXMatrixOrthoLH(&matProj, WIDTH, HEIGHT, 0.1f, 1000.0f);
	d3ddev->SetTransform(D3DTS_PROJECTION, &matProj);

}

// this is the function used to render a single frame
void render_frame() {

	d3ddev->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	d3ddev->BeginScene();

	Lissajou::DrawBackground();
	Lissajou::DrawLissajou();

	d3ddev->EndScene();

	d3ddev->Present(NULL, NULL, NULL, NULL);
}

// this is the function that cleans up Direct3D and COM
void cleanD3D() {
	d3ddev->Release();		// close and release the 3D device
	d3d->Release();			// close and release Direct3D
	Lissajou::Clean();
}

// this is the main message handler for the program
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_KEYDOWN: {
		switch (wParam) {
		case VK_SPACE: {
			if (Lissajou::isPausing()) {
				Lissajou::Play();
			}
			else {
				Lissajou::Pause();
			}
			break;
		}
		}
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void keep_rendering() {
	while (true) {
		render_frame();
	}
}

// the entry point for any Windows program
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	HWND hwnd;
	WNDCLASSEX wc;
	MSG msg;

	memset(&wc, 0, sizeof(wc));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wc.lpszClassName = APP_NAME;
	wc.hIcon = NULL;
	wc.hIconSm = NULL;

	RegisterClassEx(&wc);

	RECT rect;
	rect.top = rect.left = 0;
	rect.right = WIDTH;
	rect.bottom = HEIGHT;
	::AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, 0);
	hwnd = CreateWindowEx(0, APP_NAME, APP_NAME, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX, 0, 0, WIDTH, HEIGHT, NULL, NULL, hInstance, NULL);

	ShowWindow(hwnd, SW_SHOW);

	// set up and initialize Direct3D
	initD3D(hwnd);
	std::thread threa(keep_rendering);

	// enter the main loop
	while (true) {
		static char s[30];
		sprintf_s(s, "%d\n", ::timeGetTime());
		::OutputDebugString(s);
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		/*else {
			render_frame();
		}*/
	}

	// clean up DirectX and COM
	cleanD3D();
#ifdef _DEBUG
	char * test_leak = new char[666];
	_CrtDumpMemoryLeaks();
#endif // _DEBUG
	return msg.wParam;
}
