#pragma once

#include <gba.h>
#include "background.hpp"
#include "dialog_background.hpp"
#include "camera.hpp"
#include "sprite.hpp"

class TextScrollMap
{
private:
	char magic[8]="ScrollM";
protected:
	Background* bg[4];
	u8 scroll_speed[4]; // fixed 4.4, background's scroll relative to camera position
	                    // 0.0 = fixed bg, 1.0 = normal
	Camera* camera;
	
	Sprite** sprites;
	u16 sprites_count, sprites_cap;	
	
	s8 lock=-1;
	u8 exit_flag=0;
public:
	TextScrollMap();
	
	bool input_locked() const;
	void lock_input(int lock_id);
	void unlock_input();
	
	virtual void init();
	
	u8 run();
	void exit(u8 code);
	void set_background(u8 i, Background* bgi, u8 scroll_speedi=0x10);
	
	virtual void on_frame();
	
	virtual void on_key_down(int keys);
	virtual void on_key_held(int keys);
	virtual void on_key_up(int keys);
	
	Camera* get_camera() const;
	
	void register_sprite(Sprite* spr);
	
	
	virtual ~TextScrollMap();
	
	void scroll();
};




