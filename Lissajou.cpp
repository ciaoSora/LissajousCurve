#include "Lissajou.h"
#include <cmath>
#include <cstdio>

#pragma comment(lib, "winmm.lib")

extern LPDIRECT3DDEVICE9 d3ddev;
extern const int WIDTH;
extern const int HEIGHT;

static LPD3DXLINE pLine;

/* position of the ORIGIN under screen coordinate*/
float ORIGIN_X = WIDTH / 3, ORIGIN_Y = HEIGHT / 2;

/* FPR = Frames Per Round */
static int PERIOD = 5000, FPS = 100;
static int FPR = PERIOD * FPS / 1000;
static D3DXVECTOR2 * data;

/* parameters for A * cos(omega * t + phi) , DPF = Dots Per Frame  */
static float A1 = WIDTH * 0.25f, A2 = HEIGHT * 0.45f;
static int omega1 = 3, omega2 = 5;
const float STD_BASE = sqrt(A1 * A1 + A2 * A2);
const int STD_DPF = 60;
static int DPF = sqrt(A1 * A1 * omega1 * omega1 + A2 * A2 * omega2 * omega2) * STD_DPF / STD_BASE;

static bool paused;

void convert(D3DXVECTOR2 * a, int n) {
	for (int i = 0; i < n; ++i) {
		a[i].x = a[i].x + ORIGIN_X;
		a[i].y = HEIGHT - (a[i].y + ORIGIN_Y);
	}
}

void calculate(D3DXVECTOR2 * a, int n, float delta) {
	double t;
	for (int i = 0; i < n; ++i) {
		t = i * D3DX_PI * 2 / n;
		a[i].x = A1 * cos(omega1 * t + delta);
		a[i].y = A2 * cos(omega2 * t);
	}
	a[n] = a[0];
}

void updateData() {
	if (data) {
		delete[] data;
	}
	data = new D3DXVECTOR2[FPR * (DPF + 1)];
	for (int i = 0; i < FPR; ++i) {
		calculate(data + i * (DPF + 1), DPF, i * D3DX_PI * 2 / FPR);
		convert(data + i * (DPF + 1), DPF + 1);
	}
}

void Lissajou::Init() {
	D3DXCreateLine(d3ddev, &pLine);
	pLine->SetAntialias(true);

	updateData();
}

void Lissajou::DrawBackground() {
	
}

void Lissajou::DrawLissajou() {
	static DWORD last = timeGetTime(), curt;
	static int frame_id = 0;
	static int acc = 0;

	curt = timeGetTime();
	/*char s[30];
	sprintf_s(s, "%d\n", curt);
	::OutputDebugString(s);*/
	if (!paused) {
		acc += (curt - last);
	}
	last = curt;
	
	frame_id = (frame_id + acc * FPS / 1000) % FPR;
	acc %= (1000 / FPS);

	pLine->SetWidth(5.0f);
	pLine->Draw(data + frame_id * (DPF + 1), DPF + 1, D3DCOLOR_XRGB(0, 255, 0));
}

void Lissajou::Clean() {
	pLine->Release();
	if (data) {
		delete[] data;
		data = NULL;
	}
}

void Lissajou::SetPeriod(int new_period) {
	PERIOD = new_period;
	FPR = FPR = PERIOD * FPS / 1000;
	updateData();
}

void Lissajou::SetFPS(int new_fps) {
	FPS = new_fps;
	FPR = FPR = PERIOD * FPS / 1000;
}

void Lissajou::SetOmega(int omg1, int omg2) {
	if (omg1 <= 0 || omg2 <= 0) {
		return;
	}
	omega1 = omg1;
	omega2 = omg2;
	DPF = sqrt(A1 * A1 * omega1 * omega1 + A2 * A2 * omega2 * omega2) * STD_DPF / STD_BASE;
}

void Lissajou::Pause() {
	paused = true;
}

void Lissajou::Play() {
	paused = false;
}

bool Lissajou::isPausing() {
	return paused;
}
