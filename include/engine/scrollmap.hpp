#pragma once

#include <gba.h>
#include "background.hpp"
#include "dialog_background.hpp"
#include "camera.hpp"

class TextScrollMap
{
protected:
	Background* bg[4];
	u8 scroll_speed[4]; // fixed 4.4, background's scroll relative to camera position
	                    // 0.0 = fixed bg, 1.0 = normal
	Camera* camera;
public:
	TextScrollMap();
	
	virtual void init();
	
	void run();
	void set_background(u8 i, Background* bgi, u8 scroll_speedi);
	
	virtual void on_frame();
	
	virtual void on_key_down(int keys);
	virtual void on_key_held(int keys);
	virtual void on_key_up(int keys);
	
	Camera* get_camera() const;
	
	virtual ~TextScrollMap();
	
	void scroll();
};




