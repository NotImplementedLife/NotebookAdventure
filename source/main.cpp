#include <gba.h>
#include <stdio.h>
#include <stdlib.h>
#include "engine.hpp"

#include "level.hpp"
#include "title_screen.hpp"

#include "test_level_bin.h"

int main(void) {
	irqInit();
	irqEnable(IRQ_VBLANK);
	SetMode(MODE_0 | OBJ_ENABLE | OBJ_1D_MAP);
		
	OamPool::reset();
	
	int screen_result=0;
	TextScrollMap* screen = new TitleScreen();
		
	while(1)
	{
		screen->init();
		screen_result = screen->run();
		delete screen;		
		
		switch(screen_result&3)
		{
			case LVL_MENU:
			{
				screen = new TitleScreen();
				break;
			}
			case LVL_ENTER:
			{
				screen = new Level(test_level_bin);
				break;
			}
		}				
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


