#pragma once

#include <gba.h>
#include "background.hpp"
#include "dialog_background.hpp"
#include "camera.hpp"

class TextScrollMap
{
private:
	Background* bg[4];	
	Camera* camera;
public:
	TextScrollMap();
	void run();
	
	~TextScrollMap();
	
	void scroll();
};




