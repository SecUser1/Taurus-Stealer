#pragma once
#include "BitmapType.h"

struct ScreenshotRoutine
{
	vector<string>& screens;
};

namespace Screenshot
{
	void Get(vector<string>& screens);
}

