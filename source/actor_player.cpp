#include "actor_player.hpp"

#include "player.h"

Player::Player() : PhysicalObject(SIZE_32x32, 16)
{
	LOAD_GRIT_SPRITE_TILES(player, 8, 0, TMP_SPRITE_PALETTE);
		
	get_visual()->set_frame(0,0x0008);
	get_visual()->set_frame(1,0x0028);
	get_visual()->set_frame(2,0x0048);
	get_visual()->set_frame(3,0x0068);
	get_visual()->set_frame(4,0x0088);
	get_visual()->set_frame(5,0x00A8);
	get_visual()->set_frame(6,0x00C8);
	get_visual()->set_frame(7,0x00E8);
	
	get_visual()->set_ticks(16);
	
	get_visual()->set_crt_gfx(0);	
	get_visual()->set_animation_frames(ANIM_FRAMES_0, 0,1,2,3, -1);
	get_visual()->set_animation_frames(ANIM_FRAMES_1, 0,4,5,6,7, -1);
	get_visual()->set_animation_track(ANIM_FRAMES_0);
	update_visual();		
	set_hitbox(14,0,4,30);
	set_anchor(ANCHOR_BOTTOM);		
}