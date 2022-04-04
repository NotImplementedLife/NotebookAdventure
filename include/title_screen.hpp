#pragma once

#include "engine.hpp"
#include "screen_msg.hpp"

class Digit;

class TitleScreen : public TextScrollMap
{
private:
	static const int options_cnt = 3;
	Sprite* options[options_cnt];
	Digit* digit1;
	Digit* digit2;
	u8 crt_option = 0;
	u8 sel_level = 1;
	
	Sprite* crayon;
public:	
	TitleScreen();
	void init() override;

	void on_key_down(int keys) override;
	
	void on_frame() override;
	
	void update_sel_level(s8 d);
	
	void swap_digits_rb();
	
	~TitleScreen();
};