#pragma once

#include "engine.hpp"

class PhysicalObject;

class Level : public TextScrollMap
{
private:	
	PhysicalObject* player;
	Sprite* explorer; // invisible sprite to explore surroundings
	
	Sprite* focus;
	
	const u8* blocks_data;
	
	DialogBackground* dialog;
public:
	Level(const u8* lvl_src);

	void init() override;
	
	void on_frame() override;
	
	void add_spikes(s16 x,s16 y,s8 len);
	
	void set_focus(Sprite* spr);
	
	void on_key_down(int keys) override;
	
	void on_key_held(int keys) override;	
	
	void set_blocks_data(const u8* src);
	
	u8 get_block_data(s16 x, s16 y) const;
	
	
};
