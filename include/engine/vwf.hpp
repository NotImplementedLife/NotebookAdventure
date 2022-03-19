/*! \file vwf.hpp
	\brief Variable width font resources
 */

#pragma once

#include <gba.h>

#include "font.hpp"

/*! \class Vwf
	\brief variable width font processor
 */
class Vwf
{
	friend class DialogBox;
private:
	const Font816& font;
	u32* render_space;
	u16 width, height;
	u8 text_color;
	u16 px_offset,crt_row, crt_col;
public:
	/*! \brief creates a Vwf processor
		\param font the rendered text font
	 */
	Vwf(const Font816& _font);
	
	/*! \brief assigns a render zone in memory to the VWF processor
		\param address pointer to the start of the memory render zone
		\param box_width width of the render rectangle (in tiles)
		\param box_height height of the render rectangle (in tiles)
	 */
	void set_render_space(u32* address, int box_width, int box_height);
	
	int draw_word(const char* word);
	
	int draw_text(const char* text);
	
	/*! \brief prints a character to the render space
		\param c character to print
		\return 1 if character was drawn, 0 if couldn't draw the character (due to insufficient space)
	 */
	int put_char(char c);
	
	/*! \brief specifies which color the rendered text would have
		\param color_id color index in palette
	 */
	void set_text_color(u8 color_id);
	
	void reset();
};

