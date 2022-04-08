#pragma once

#include "engine.hpp"

#include "actor_cat.hpp"
#include "actor_player.hpp"

extern const int INTRO_SCRIPT[];

class Intro : public TextScrollMap
{
private:	
	Cat* cat;
	Player* player;
	Sprite* emoji_paw;
	Sprite* emoji_warn;

	Sprite* mva=NULL;
	u8 speed=1;
	s16 mvx=0, mvy=0;
	u32 mvdelay=0;
	const int* script = INTRO_SCRIPT;
public:	
	Intro();
	void init() override;

	void on_key_down(int keys) override;
	
	void on_frame() override;	
};