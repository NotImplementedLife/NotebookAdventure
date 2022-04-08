#include "actor_cat.hpp"

#include "cat.h"

Cat::Cat() : PhysicalObject(SIZE_16x16, 16)
{
	LOAD_GRIT_SPRITE_TILES(cat, 0x120, 128, TMP_SPRITE_PALETTE);
		
	get_visual()->set_frame(0,0x0120);		
	get_visual()->set_frame(1,0x0128);		
	get_visual()->set_frame(2,0x0130);		
	get_visual()->set_frame(3,0x0138);		
	get_visual()->set_frame(4,0x0140);		
	get_visual()->set_frame(5,0x0148);
	get_visual()->set_frame(6,0x0150);
	
	get_visual()->set_ticks(16);
	
	get_visual()->set_crt_gfx(0);	
	get_visual()->set_animation_frames(ANIM_FRAMES_0, 0, 1, 2, 3, -1);
	get_visual()->set_animation_frames(ANIM_FRAMES_1, 0, 4, 5, 6, -1);
	get_visual()->set_animation_track(ANIM_FRAMES_0);
	update_visual();		
	set_hitbox(3,0,9,16);
	set_anchor(ANCHOR_BOTTOM);
}