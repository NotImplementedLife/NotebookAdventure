#include "font.hpp"

Font816::Font816(const void* font_gfx)
{	
	source = (u8*)font_gfx;
	charw = new u8[256];
	for(int i=0;i<256;i++)
	{
		u8* chaddr = (u8*)source + 128*i;
		int chsize = 0;
		for(int l=0;l<16;l++)
		{
			u8* lnaddr = chaddr+8*l;
			int c=8;
			for(;c--;)
			{										
				if(lnaddr[c]!=0)
				{										
					break;					
				}
			}
			if(c!=-1)
			{
				if(c>chsize)
					chsize=c;
			}
		}		
		charw[i]=(u8)(chsize+1);
	}				
	charw[1] = 8;   // dialog seletor
	charw[' '] = 3; // default space size
}

int Font816::measure_text(const char* text) const
{
	int len=0;
	for(;*text!='\0';text++)		
	{
		len+=charw[(u8)*text];
	}
	return len;		
}

u8* Font816::get_char_gfx(char ch) const
{
	return source + 128*((u32)ch);
}

u8 Font816::get_char_width(char ch) const
{
	return charw[(u8)ch];
}

Font816::~Font816()
{
	delete[] charw;
}

#include "font816_default.h"

const Font816& defaultFont816 = Font816(font816_defaultTiles);

