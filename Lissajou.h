#pragma once
#include "Utility.h"

namespace Lissajou {
	void Init();
	void DrawBackground();
	void DrawLissajou();
	void Clean();

	void SetPeriod(int new_period);
	void SetFPS(int new_fps);
	void SetOmega(int omg1, int omg2);
	
	void Pause();
	void Play();
	bool isPausing();
}
