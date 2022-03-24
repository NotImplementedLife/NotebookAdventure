#include "engine/scrollmap.hpp"

TextScrollMap::TextScrollMap()
{
	OamPool::reset();
	for(int i=0;i<4;i++) 
	{
		bg[i] = NULL;
		scroll_speed[i] = 0x10;
	}
	camera = new Camera();
}

void TextScrollMap::init() { }

void TextScrollMap::set_background(u8 i, Background* bgi, u8 scroll_speedi)
{
	bg[i]=bgi;
	scroll_speed[i]=scroll_speedi;
}

void TextScrollMap::on_frame() { }

#include "error.hpp"

void TextScrollMap::run()
{	
	for(int i=0;i<4;i++)
		if(bg[i])
		{			
			bg[i]->init();	
		}	
	while(1)
	{
		VBlankIntrWait();
		scanKeys();
		int keys_down = keysDown();
		int keys_held = keysHeld();
		int keys_up = keysUp();
		
		on_frame();		
						
		for(int i=0;i<4;i++)		
			if(bg[i])
			{		
				bg[i]->set_scroll(camera->get_x(), camera->get_y());
				bg[i]->build_map();
				bg[i]->key_down(keys_down);
				bg[i]->key_held(keys_held);
				bg[i]->key_up(keys_up);
				bg[i]->render();
			}		
			
		OamPool::deploy();
	}
}

Camera* TextScrollMap::get_camera() const
{ 
	return camera;
}

TextScrollMap::~TextScrollMap()
{
	for(int i=0;i<4;i++)
		if(bg[i])
			delete bg[i];
	delete camera;
	OamPool::reset();
}