#pragma once

// useful color definitions

#include <gba.h>


#define Black        RGB5(0x00,0x00,0x00)
#define DarkBlue     RGB5(0x00,0x00,0x10)
#define DarkGreen    RGB5(0x00,0x10,0x00)
#define DarkCyan     RGB5(0x00,0x10,0x10)
#define DarkRed      RGB5(0x10,0x00,0x00)
#define DarkMagenta  RGB5(0x10,0x00,0x10)
#define DarkYellow   RGB5(0x10,0x10,0x00)
#define DarkGray     RGB5(0x10,0x10,0x10)
#define Blue         RGB5(0x00,0x00,0x1F)
#define Green        RGB5(0x00,0x1F,0x00)
#define Cyan         RGB5(0x00,0x1F,0x1F)
#define Red          RGB5(0x1F,0x00,0x00)
#define Magenta      RGB5(0x1F,0x00,0x1F)
#define Yellow       RGB5(0x1F,0x1F,0x00)
#define Gray         RGB5(0x18,0x18,0x18)
#define White        RGB5(0x1F,0x1F,0x1F)

extern const u16 SYS_COLORS[];

u16 cl_brighten(u16 color, u8 level);

u16 cl_blend(u16 color_1, u16 color_2, u8 alpha);

extern u16 tmp_palette[512];

void load_tmp_palette();

#define TMP_BG_PALETTE (tmp_palette)

#define TMP_SPRITE_PALETTE (tmp_palette+256)













