#pragma once

#include "engine.hpp"

const int LVL_NEXT = 1;
const int LVL_MENU = 2;
const int LVL_RETRY = 3;

class PhysicalObject;

class Level : public TextScrollMap
{
private:		
	PhysicalObject* player;
	Sprite* explorer; // invisible sprite to explore surroundings
	
	Sprite* focus;
	
	Sprite* cat;
	
	Sprite* hole;
	
	const u8* blocks_data;
	
	DialogBackground* dialog;
	
	u8 completed;
	int jump_timeout=0;
public:
	Level(const u8* lvl_src);	

	void init() override;
	
	void on_frame() override;
	
	void add_spikes(s16 x,s16 y,s8 len);
	
	void add_trampoline(s16 x, s16 y);
	
	void set_focus(Sprite* spr);
	
	void on_key_down(int keys) override;
	
	void on_key_held(int keys) override;	
	
	void set_blocks_data(const u8* src);
	
	u8 get_block_data(s16 x, s16 y) const;
	
	static int dialog_controlled_jump(void* sender);
	static int game_over_dialog_finished(void* sender);
	static int level_completed_dialog_finished(void* sender);
};
