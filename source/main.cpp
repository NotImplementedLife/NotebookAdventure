#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "game_dat.hpp"
#include "level.hpp"
#include "title_screen.hpp"

#include "test_level_bin.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	OamPool::reset();
	
	load_user_data();	
	
	int screen_result = LVL_MENU;
	TextScrollMap* screen = NULL;	
		
	while(1)
	{		
		switch(screen_result&3)
		{
			case LVL_MENU:
			{								
				save_user_data();				
				screen = new TitleScreen();
				break;
			}
			case LVL_ENTER:
			{
				screen = new Level(test_level_bin);
				break;
			}
		}	
		screen->init();
		screen_result = screen->run();
		delete screen;					
	}
	TitleScreen* ts = new TitleScreen();
	ts->init();
	fatal("result",ts->run());
	delete ts;
	
	
	Level* lvl = new Level(test_level_bin);
	lvl->init();	
	lvl->run();
	delete lvl;
	
	lvl = new Level(test_level_bin);
	lvl->init();
	lvl->run();
	delete lvl;
	
	
	return 0;
}


