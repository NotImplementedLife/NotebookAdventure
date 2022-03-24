#include "dialog_background.hpp"
#include "error.hpp"
#include <string.h>

DialogBackground::DialogBackground(u16 id, u16 char_base, u16 map_base) : Background(id, char_base, map_base, 32, 32)
{
	strcpy(magic,"DialogBkg");
	dialogs_count=0;	
	for(int i=0;i<4; i++)
	{
		dialogs[i]=NULL;
		allocated[i]=0;
		awaiting_text[i]=0;
		_cooldown[i]=0;
	}
	obj_caret = NULL;
	caret_waiting = 0;
}

void DialogBackground::load_tiles(const void* source, u8 palette_displacement)
{
	Background::load_tiles(source, 5*64, false, palette_displacement);
}

void DialogBackground::set_caret(int obj_tile_id)
{
	obj_caret_id = OamPool::add_obj(ObjAttribute(SIZE_8x8, 16, 16, obj_tile_id));
	obj_caret = OamPool::get_object_by_id(obj_caret_id);
	obj_caret->hide();
}

void DialogBackground::assign_caret_to(int dialog_id)
{	
	DialogBox* dialog = get_dialog(dialog_id);
	
	u8 left = dialog->left;
	u8 top = dialog->top;
	u8 width = dialog->width;
	u8 height = dialog->height;	
	obj_caret->set_x(((left+width-2)<<3)+2);
	obj_caret->set_y(((top+height-2)<<3)+2);
}

void DialogBackground::clear_map()
{
	u32 _0 = 0x00000000;
	CpuFastSet(&_0, map_base_address, (16*32) | FILL);
}

void DialogBackground::key_down(u16 keys)
{
	if(active_dialog_id!=4)
	{
		if(!_cooldown[active_dialog_id] && caret_waiting && (keys & KEY_A))
		{		
			caret_waiting = 0;		
			obj_caret->hide();		
				
			if(caret_release_action & DIALOG_CLEAR)
			{
				dialogs[active_dialog_id]->clear();								
			}
			if(caret_release_action & DIALOG_HIDE)
			{		
				hide_dialog_box(active_dialog_id);
				active_dialog_id=4;				
			}						
			caret_release_action=0;			
		}	
		
		if(_cooldown[active_dialog_id] && caret_waiting)
		{
			if(_cooldown[active_dialog_id]==1)								
			{
				caret_waiting = 0;
				obj_caret->hide();		
					
				if(caret_release_action & DIALOG_CLEAR)
				{
					dialogs[active_dialog_id]->clear();								
				}
				if(caret_release_action & DIALOG_HIDE)
				{		
					hide_dialog_box(active_dialog_id);
					active_dialog_id=4;				
				}						
				caret_release_action=0;		
			}			
			_cooldown[active_dialog_id]--;
		}
	}
}

void DialogBackground::key_held(u16 keys)
{
}

void DialogBackground::key_up(u16 keys)
{
}

void DialogBackground::render()
{
	if(caret_waiting)
	{
		caret_waiting++;
		if(caret_waiting==10)
		{	
			caret_waiting=1;
			if(!_cooldown[active_dialog_id])
				obj_caret->show();
		}
		else if(caret_waiting==5)
		{
			obj_caret->hide();
		}		
		return;
	}
	if(active_dialog_id==4)
	{
		for(int i=0;i<dialogs_count;i++)
		{
			if(awaiting_text[i])
			{			
				active_dialog_id=i;
				assign_caret_to(active_dialog_id);
				show_dialog_box(active_dialog_id);				
				break;
			}	
		}			
	}
	if(active_dialog_id==4) return;	
	int k=dialogs[active_dialog_id]->vwf->draw_word(awaiting_text[active_dialog_id]);
	if(k==0)
	{
		caret_release_action = DIALOG_CLEAR;
		if(awaiting_text[active_dialog_id][0]=='\0')
		{
			awaiting_text[active_dialog_id]=NULL;
			obj_caret->hide();			
			caret_release_action |= DIALOG_HIDE;
		}
		caret_waiting = 1;		
	}
	else			
		awaiting_text[active_dialog_id] += k;
}

void DialogBackground::build_map()
{
	
}

DialogBox* DialogBackground::get_dialog(u32 dialog_id)
{
	if(dialog_id >= dialogs_count)	
		fatal(ERR_DLG_INVALID_ID);
	return dialogs[dialog_id];
}

void DialogBackground::show_dialog_box(u32 id)
{	
	DialogBox* dialog = get_dialog(id);
	u8 left = dialog->left;
	u8 top = dialog->top;
	u8 width = dialog->width;
	u8 height = dialog->height;
	
	u16* map_gfx = ((u16*)map_base_address) + 32*top + left;
	for(int r=1;r<height-1;r++)
	{		
		map_gfx[32*r] = 0x0003;
		map_gfx[32*r+width-1] = 0x0403;
	}
	
	for(int c=1;c<width-1;c++)
	{		
		map_gfx[c] = 0x0002;
		map_gfx[32*(height-1)+c] = 0x0C02;
	}
	
	map_gfx[0] = 0x0001;
	map_gfx[width-1] = 0x0401;
	map_gfx[32*(height-1)] = 0x0801;
	map_gfx[32*(height-1)+width-1] = 0x0C01;
	
	u32 total_alloc = id==0 ? 0 : allocated[id-1];	
	u16 k=8+total_alloc;
	for(int r=1;r<height-1;r++)
		for(int c=1;c<width-1;c++)
		{
			map_gfx[32*r+c] = k++;
		}	
}

void DialogBackground::hide_dialog_box(u32 id)
{				
	DialogBox* dialog = get_dialog(id);
	u8 left = dialog->left;
	u8 top = dialog->top;
	u8 width = dialog->width;
	u8 height = dialog->height;
	u16* map_gfx = ((u16*)map_base_address) + 32*top + left;	
	for(int r=0;r<height;r++)
		for(int c=0;c<width;c++)
		{
			map_gfx[32*r+c] = 0;
		}	
}

int DialogBackground::create_dialog_box(u8 left, u8 top, u8 width, u8 height, Vwf* vwf)
{
	if(dialogs_count==4)
	{
		fatal(ERR_DLG_EXCEEDED);
	}
		
	u32 total_alloc = 0;
	if(dialogs_count>0)
		total_alloc = allocated[dialogs_count-1];
	
	u32 crt_alloc = DialogBox::get_vram_tiles_count(width,height);	
	
	total_alloc+=crt_alloc;
	if(total_alloc>=max_alloc)
		return -1;	
	allocated[dialogs_count] = total_alloc;
	total_alloc-=crt_alloc;
	
	dialogs[dialogs_count]=new DialogBox(left,top,width,height,vwf);
	
	u32* base_tile_gfx = ((u32*)char_base_address) + 16*4;	
	u32* fill_addr = ((u32*)char_base_address)+16*(8+total_alloc);	
	dialogs[dialogs_count]->set_vram(fill_addr, base_tile_gfx);	
	
	return dialogs_count++;
}

bool DialogBackground::launch_dialog(int dialog_id, const char* msg, u16 cooldown)
{
	get_dialog(dialog_id); // just to check if the dialog exists
	if(awaiting_text[dialog_id]!=NULL)
		// fatal("Dialog box can't run two texts simultaneously",123);
		return false;
	awaiting_text[dialog_id]=(char*)msg;
	_cooldown[dialog_id]=cooldown;
	return true;
}

DialogBackground::~DialogBackground()
{	
	for(int i=0;i<4;i++)
		delete dialogs[i];
	OamPool::remove_obj(obj_caret_id);
}
