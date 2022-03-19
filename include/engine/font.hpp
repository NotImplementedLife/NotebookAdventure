/*! \file font.hpp
	\brief text font definitions
 */

#pragma once

#include <gba.h>

/*! \class Font816
	\brief 8x16-sized font handler
 */
class Font816
{
private:
	u8* source;
	u8* charw; // char widths
public:
	/*! \fn Font816(const void*)
		\brief creates a new Font instance
		\param font_gfx font graphics address
	*/
	Font816(const void* font_gfx);
	
	/*! \fn measure_text(const char*)
		\brief  measure input text written with current font
		\param  text a string to measure
		\return length of the text, in pixels
	 */
	int measure_text(const char* text) const;	
	
	/*! \fn get_char_gfx(char)
		\brief calculates address of tiles representing the specified character
		\param ch character id
		\return pointer to the memory zone containing character visuals (2 tiles)
	 */
	u8* get_char_gfx(char ch) const;
	
	/*! \fn get_char_width(char)
		\brief gets character width in pixels
		\param ch character id
		\return character width
	 */
	u8 get_char_width(char ch) const;
	
	~Font816();
};

extern const Font816& defaultFont816; /*!< default Font816 font */