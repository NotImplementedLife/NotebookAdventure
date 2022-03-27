/*! \file dialog_background.hpp	
 */
 
#pragma once

#include "background.hpp"
#include "dialogbox.hpp"
#include "oam.hpp"

typedef int (*DialogCompletedCallback)(void*);

class DialogBackground : public Background
{		
	friend class DialogBox;
public:		
	//void char_base_address;
	using Background::set_scroll;
		
	/*! \brief Creates a Background component specialized in dialog render
		\param id background id (0..3)
		\param char_base  VRAM char block base (0..3)
		\param map_base   VRAM screen block base (0..31)			
	*/
	DialogBackground(u16 id, u16 char_base, u16 map_base);	
protected:	
	/*! \brief loads tileset graphics into its specific VRAM block		
		\param source     tiles buffer data
		\param len        buffer length
		\param compressed specifies if tiles are LZ77 compressed			
		\details In a Dialog Background, tiles have predefined purposes:
		- tile 0 : transparent tile		
		- tile 1 : dialog box corner tile		
		- tile 2 : horizontal box border tile (top/bottom by reflection)		
		- tile 3 : vertical box border tile (left/right by reflection)  				
		- tile 4 : dialog box fill tile		
		- Tiles 16+ can be allocated to a VWF renderer
		
	 */
	void load_tiles(const void* source, u8 palette_displacement=0);
	
	/*! \brief sets the tile id which would be the caret sprite
		\param obj_tile_id tile id in OBJ Tiles VRAM
	 */
	void set_caret(int obj_tile_id);	
	
protected:
	void assign_caret_to(int dialog_id);
public:
	/*! \brief fills map with transparent tiles
	*/
	void clear_map();
	
	/*! \brief builds & updates dialog interface
	*/
	void build_map() override;	
	
	void render();
	
	void key_down(u16 keys);
	void key_held(u16 keys);
	void key_up(u16 keys);
	
private:
	int caret_waiting;
	u16 obj_caret_id;
	ObjAttribute* obj_caret;
	
	DialogBox* dialogs[4];
	u8 dialogs_count;
	u32 allocated[4];
	char* awaiting_text[4];
	u16 _cooldown[4];
	const u32 max_alloc = 0x1800;
	
	DialogBox* get_dialog(u32 dialog_id);
	u32 active_dialog_id=4;
	u16 caret_release_action = 0;
	
	const int DIALOG_NOTHING = 0;
	const int DIALOG_CLEAR   = 1<<0;
	const int DIALOG_HIDE    = 1<<1;	
	
protected:
	void show_dialog_box(u32 id);
	void hide_dialog_box(u32 id);
public:

	/*! \brief creates a new dialog box on the background
		\param left    horizontal position in tiles of the dialog top-left corner
		\param top     vertical position in tiles of the dialog top-left corner
		\param width   dialog width in tiles
		\param height  dialog height in tiles
		\param vwf     Vwf processor
	 */
	int create_dialog_box(u8 left, u8 top, u8 width, u8 height, Vwf* vwf);	
		
	/*! \brief schedules a new dialog text to render
		\param dialog_id id of the dialog box that should display the text
		\param msg       the text to be displayed
		\param cooldown  number of render ticks to wait until dialog disappears. If 0, dialog box closes on A button pressed
		\return true if the operation succeeded, false otherwise
		
		\details Calling launch_dialog() does not necessarily start displaying the desired dialog box. 
		If any dialog box is already displaying, the render system will wait for the operation to be finished.
		Eventually, the pending dialog box with the lowest id is being processed. 
		
		Therefore, the following example will display first "text0", "text1" and finally "text2", despite the order of launching dialogs:
		
		\code
			dialog_bg.launch_dialog(2, "text2");
			dialog_bg.launch_dialog(0, "text0", 30);
			dialog_bg.launch_dialog(1, "text1", 30);	

			while(1) { 
				VBlankIntrWait();
				scanKeys();
				int keys_down = keysDown();
				dialog_bg.key_down(keys_down);
				dialog_bg.render();
			}
		\endcode
	 */
	bool launch_dialog(int dialog_id, const char* msg, u16 cooldown=0);
	
private:
	int pending_event_id = -1;
	DialogCompletedCallback dialog_finished_callback[4];
	void* dialog_finished_args[4];
	
public:
	void run_on_dialog_finished(int id, DialogCompletedCallback callback, void* callback_arg);
	
public:		
	virtual ~DialogBackground();

public:
	u16 *opt_x, *opt_y;
	u8 opt_len;	
	u8 opt_crt_sel;
	
	u8 opt_result[8];
	u8 opt_rescnt;
	
	static void vwf_option_report(void* target,int x,int y);
public:
	u8 get_option(u32 n) const;
	
};
