#include "colors.hpp"
#include "debug.hpp"

const u16 SYS_COLORS[] =
{ 
	Black,
	DarkBlue,
	DarkGreen,
	DarkCyan,
	DarkRed,
	DarkMagenta,
	DarkYellow,
	DarkGray,
	Blue,
	Green,
	Cyan,
	Red,
	Magenta,
	Yellow,
	Gray,
	White
};

u16 cl_brighten(u16 color, u8 level)
{
	u16 r = color & 0x1F;
	u16 g = (color>>5) & 0x1F;
	u16 b = (color>>10) & 0x1F;
	
	r = ((16-level)*r + level*31)/16;
	g = ((16-level)*g + level*31)/16;
	b = ((16-level)*b + level*31)/16;
	
	return RGB5(r,g,b);
}