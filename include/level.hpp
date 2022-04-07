#pragma once

#include "engine.hpp"
#include "screen_msg.hpp"

extern const u16 mul_75[]; // quick multiplication table

extern const int LEVELS_COUNT;

class PhysicalObject;
class GoddessStar;

class Level : public TextScrollMap
{
public:		
	PhysicalObject* player;
	PhysicalObject* cat;
	
	Sprite* explorer; // invisible sprite to explore surroundings
	
	Sprite* focus;
	Sprite* goddess_crown;
	Sprite* goddess_icon;
	PhysicalObject* xfocus;
	GoddessStar* goddess_star;	
	
	bool goddess_mode = false;
	
	const u8* blocks_data;
	
	DialogBackground* dialog;
	
	u8 completed;
	const void* map_source;
	int jump_timeout=0;
	void update_actor(PhysicalObject* obj);
	u16 id;
	
	u16 qopt_size=0;
	u16 quick_obstacle_pos_table[64];
	
	Level(const void* lvl_map, const u8* lvl_src);
public:	
	Level() = default;
	Level(u32 level_no);

	void init() override;
	
	void on_frame() override;
	
	void on_end_frame() override;
	
	void add_spikes(s16 x,s16 y,s8 len);
	
	void add_trampoline(s16 x, s16 y);
	
	void add_obstacle(u8 id, u8 orientation, s16 x, s16 y, u16 qops_id);
	
	void add_obstacle_activator(u8 id, s16 x, s16 y);
	
	void set_focus(Sprite* spr);
	
	void on_key_down(int keys) override;
	
	void on_key_held(int keys) override;	
	
	void on_key_up(int keys) override;
	
	void on_loaded() override;
	
	void set_blocks_data(const u8* src);
	
	u8 get_block_data(s16 x, s16 y) const;
	
	u8 get_block_data_tiled(u8 x, u8 y) const;
	
	s8 obst_status[14];
	
	~Level();
	
	static int dialog_controlled_jump_p(void* sender);
	static int dialog_controlled_jump_c(void* sender);
	static int dialog_obtained_goddess_star(void* sender);
	static int game_over_dialog_finished(void* sender);
	static int level_completed_dialog_finished(void* sender);	
	static int pause_dialog_finished(void* sender);
};
