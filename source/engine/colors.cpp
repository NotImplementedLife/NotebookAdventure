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

u16 cl_blend(u16 color_1, u16 color_2, u8 alpha)
{
	u16 r1 = color_1 & 0x1F;
	u16 g1 = (color_1>>5) & 0x1F;
	u16 b1 = (color_1>>10) & 0x1F;
	
	u16 r2 = color_2 & 0x1F;
	u16 g2 = (color_2>>5) & 0x1F;
	u16 b2 = (color_2>>10) & 0x1F;
	
	
	r1 = ((16-alpha)*r1 + alpha*r2)/16;
	g1 = ((16-alpha)*g1 + alpha*g2)/16;
	b1 = ((16-alpha)*b1 + alpha*b2)/16;
	
	return RGB5(r1,g1,b1);
}








