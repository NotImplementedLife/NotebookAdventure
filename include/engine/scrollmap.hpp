#pragma once

#include <gba.h>
#include "background.hpp"
#include "dialog_background.hpp"
#include "camera.hpp"

class TextScrollMap
{
private:
	Background* bg[4];
	u8 scroll_speed[4]; // fixed 4.4, background's scroll relative to camera position
	                    // 0.0 = fixed bg, 1.0 = normal
	Camera* camera;
public:
	TextScrollMap();
	void run();
	void set_background(u8 i, Background* bg, u8 scroll_speed);
	
	~TextScrollMap();
	
	void scroll();
};




