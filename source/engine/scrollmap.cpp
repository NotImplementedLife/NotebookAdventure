#include "engine/scrollmap.hpp"

TextScrollMap::TextScrollMap()
{
	for(int i=0;i<4;i++) 
	{
		bg[i] = NULL;
		scroll_speed[i] = 0x10;
	}
	OamPool::reset();
	camera = new Camera();
}

void TextScrollMap::set_background(u8 i, Background* bg, u8 scroll_speed)
{
	this->bg[i]=bg;
	this->scroll_speed[i]=scroll_speed;
}

void TextScrollMap::run()
{
	while(1)
	{
		VBlankIntrWait();
		scanKeys();
		int keys_down = keysDown();
		int keys_held = keysHeld();
		int keys_up = keysUp();
		
		for(int i=0;i<3;i++)		
			if(bg[i])
			{
				bg[i]->build_map();	
				bg[i]->key_down(keys_down);
				bg[i]->key_held(keys_held);
				bg[i]->key_up(keys_up);
				bg[i]->render();
			}				
		
		OamPool::deploy();
	}
}

TextScrollMap::~TextScrollMap()
{
	for(int i=0;i<3;i++)
		delete bg[i];
	delete camera;
	OamPool::reset();
}