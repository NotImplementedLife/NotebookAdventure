#include "vwf.hpp"

Vwf::Vwf(const Font816& _font) : font(_font)
{
	text_color = 1;
}

void Vwf::set_render_space(u32* address, int box_width, int box_height)
{
	render_space = address;
	width = box_width<<3; 
	height = box_height;  // keep height in tiles
	crt_row = crt_col= 0;
	px_offset = 0;
	
	opt_report_dialog=0;
	opt_report_dest=NULL;;
}

void Vwf::reset()
{	
	crt_row = crt_col= 0;
	px_offset = 0;
}

int Vwf::draw_word(const char* word)
{	
	if(word[0]=='\0') 
		return 0;
	if(word[0]==' ' || word[0]=='\n')
	{
		return put_char(word[0]);
	}
	int slen=0, wlen=0;
	while(word[slen]!='\0' && word[slen]!=' ' && word[slen]!='\n')
	{
		slen++;
		wlen+=font.get_char_width(word[slen]);
	}
	
	if(wlen>=width)  // word too large, write as it is
	{
		int result=0;
		for(int i=0;i<slen;i++)
		{
			int ok = put_char(word[i]);
			if(ok==0) 
				break;
			result+=ok;
		}
		return result;
		return 1;
	}		
	
	if(8*crt_col+px_offset+wlen+3>=width)
		if(!put_char('\n'))
			return 0;
		
	for(int i=0;i<slen;i++)
		put_char(word[i]);
	return slen;	
	
	
	for(;*word!='\0';word++)
	{
		put_char(*word);
	}		
	return 1;
}

int Vwf::draw_text(const char* text)
{
	int p=0;	
	for(int k=0; (k=draw_word(text+p)); p+=k);
	return p;
}

int Vwf::put_char(char c)
{		
	if(c=='\n')
	{		
		if(crt_row+2>=height)		
			return 0;
		crt_row+=2;
		crt_col=0;
		px_offset=0;
		return 1;
	}
	u8* gfx = font.get_char_gfx(c);
	u8 sz = font.get_char_width(c);
	
	if(8*crt_col+px_offset+sz>=width)
	{
		if(crt_row+2>=height)		
			return 0;
		crt_row+=2;
		crt_col=px_offset=0;
	}	
	
	if(c==1)
	{
		if(opt_report_dialog)
		{						
			opt_report_dialog(opt_report_dest, (crt_col<<3) | px_offset, crt_row<<4);
		}
	}
	
	u8* tmp=new u8[16];	
	for(int i=0;i<16;i++) tmp[i]=0;		
	
	for(int y=0;y<8;y++)
	{					
		dmaCopy(render_space+2*width*crt_row+16*crt_col+2*y, tmp, 8);
		dmaCopy(render_space+2*width*crt_row+16*(crt_col+1)+2*y, tmp+8, 8);
		
		for(int x=0;x<8;x++){
			tmp[px_offset+x] |= gfx[8*y+x] & text_color;
		}
		
		dmaCopy(tmp,((u8*)render_space)+4*(2*width*crt_row+16*crt_col+2*y), 8);
		dmaCopy(tmp+8,&render_space[2*width*crt_row+16*(crt_col+1)+2*y], 8);
		
		
		dmaCopy(render_space+2*width*(crt_row+1)+16*crt_col+2*y, tmp, 8);
		dmaCopy(render_space+2*width*(crt_row+1)+16*(crt_col+1)+2*y, tmp+8, 8);
		
		for(int x=0;x<8;x++){
			tmp[px_offset+x] |= gfx[8*(y+8)+x] & text_color;
		}
		
		dmaCopy(tmp,((u8*)render_space)+4*(2*width*(crt_row+1)+16*crt_col+2*y), 8);
		dmaCopy(tmp+8,&render_space[2*width*(crt_row+1)+16*(crt_col+1)+2*y], 8);	
	}
	delete[] tmp;
	px_offset+=sz;
	crt_col+=px_offset>>3;
	px_offset&=7;
	
	return 1;
}

void Vwf::set_text_color(u8 color_id)
{
	text_color = color_id;
}

void Vwf::set_option_report(DialogOptionReport report,void* target)
{
	opt_report_dialog = report;
	opt_report_dest = target;
}

