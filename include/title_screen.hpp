#pragma once

#include "engine.hpp"
#include "screen_msg.hpp"

class TitleScreen : public TextScrollMap
{
private:
	static const int options_cnt = 2;
	Sprite* options[options_cnt];
	u8 crt_option = 0;
	
	Sprite* crayon;
public:	
	TitleScreen();
	void init() override;

	void on_key_down(int keys) override;
};