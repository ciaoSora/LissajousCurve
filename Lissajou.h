#pragma once
#include "Utility.h"

namespace Lissajou {
	void Init();
	void DrawBackground();
	void DrawLissajou();
	void Clean();

	void SetPeriod(int new_period);
	void SetFPS(int new_fps);
	void SetOmega1(int omg);
	void SetOmega2(int omg);
	int GetPeriod();
	float GetA1();
	float GetA2();
	int GetOmega1();
	int GetOmega2();
	float GetOriginX();
	float GetOriginY();
	float GetDeltaPhi();
	
	void Pause();
	void Play();
	bool isPausing();
}
