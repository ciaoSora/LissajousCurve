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
static int PERIOD = 4000, FPS = 100;
static int FPR = PERIOD * FPS / 1000;
static D3DXVECTOR2 * data;
static D3DXVECTOR2 bkg[42 * 2];

/* parameters for A * cos(omega * t + phi) , DPF = Dots Per Frame  */
static float A1 = WIDTH * 0.25f, A2 = HEIGHT * 0.45f;
static int omega1 = 1, omega2 = 1;
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
	data = new D3DXVECTOR2[DPF + 1];
}

void updateBackground() {
	const int half = ((sizeof(bkg) / sizeof(D3DXVECTOR2) / 4) - 1) / 2;
	float unit = A1 / half;

	int cnt = 0;
	for (int i = -half; i <= half; ++i) {
		bkg[cnt].x = bkg[cnt + 1].x = i * unit;
		bkg[cnt].y = A2;
		bkg[cnt + 1].y = -A2;
		cnt += 2;
	}
	unit = A2 / half;
	for (int j = -half; j <= half; ++j) {
		bkg[cnt].x = A1;
		bkg[cnt + 1].x = -A1;
		bkg[cnt].y = bkg[cnt + 1].y = j * unit;
		cnt += 2;
	}

	convert(bkg, (sizeof(bkg) / sizeof(D3DXVECTOR2)));
}

void Lissajou::Init() {
	D3DXCreateLine(d3ddev, &pLine);
	pLine->SetAntialias(true);

	updateData();
	updateBackground();
	
}

void Lissajou::DrawBackground() {
	pLine->SetWidth(2.0f);
	const int half = ((sizeof(bkg) / sizeof(D3DXVECTOR2) / 4) - 1) / 2;
	for (int i = 0; i <= (half * 2 + 1) * 2; ++i) {
		pLine->Draw(bkg + 2 * i, 2, D3DCOLOR_XRGB(0, 100, 0));
	}
}

void Lissajou::DrawLissajou() {
	static float delta_phi = 0.0f;
	static DWORD last = timeGetTime(), curt;
	
	curt = timeGetTime();
	char s[30];
	sprintf_s(s, "%d\n", curt);
	OutputDebugString(s);
	if (!paused) {
		delta_phi += (curt - last) * D3DX_PI * 2 / PERIOD;
	}
	if (delta_phi > D3DX_PI * 2) {
		delta_phi -= D3DX_PI * 2;
	}
	last = curt;

	float t;
	for (int i = 0; i < DPF; ++i) {
		t = i * D3DX_PI * 2 / DPF;
		data[i].x = A1 * cos(omega1 * t + delta_phi);
		data[i].y = A2 * cos(omega2 * t);
	}
	data[DPF] = data[0];
	convert(data, DPF + 1);
	pLine->SetWidth(5.0f);
	pLine->Draw(data, DPF + 1, D3DCOLOR_XRGB(0, 255, 0));
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
