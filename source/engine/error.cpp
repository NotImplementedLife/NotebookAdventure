#include "error.hpp"

#include "engine/scrollmap.hpp"

const char* __str_err_intro = "Execution stopped due to a fatal error.\nError message:\n\n";
const char* __str_err_code = "Error code : ";
const char* __str_hint = "\nHint : ";
const char* __str_hex = "0123456789ABCDEF";

class FatalErrScreen : public DialogBackground
{	
private:
	const char* msg;
	const char* hint;
	u32 err_code;
public:
	FatalErrScreen(const char* msg,u32 err_code, const char* hint=NULL) : DialogBackground(0, 0, 28)
	{
		this->msg = msg;
		this->err_code = err_code;
		this->hint = hint;		
	}
	
	void init() override
	{
		u32 _0 = 0;
		CpuFastSet(&_0, BG_PALETTE, 128 | FILL);
		CpuFastSet(&_0, (void*)VRAM, (0x10000 / 4) | FILL);
		clear_map();
		Vwf vwf=Vwf(defaultFont816);
		
		create_dialog_box(0, 0, 30, 14, &vwf);
		show_dialog_box(0);
		
		vwf.set_text_color(fgColor);
		vwf.draw_text(__str_err_intro);
		
		vwf.set_text_color(hlColor);
		vwf.draw_text(msg);				

		if(err_code != 0)
		{
			create_dialog_box(0, 14, 30, 6, &vwf);
			show_dialog_box(1);
			vwf.set_text_color(fgColor);
			vwf.draw_text(__str_err_code);
			vwf.set_text_color(hlColor);
			char* str_code = new char[9];		
			for(int i=0;i<8;i++)
			{
				str_code[7-i] = __str_hex[err_code & 0xF];
				err_code>>=4;
			}						
			str_code[8]='\0';
			vwf.draw_text("0x");
			vwf.draw_text(str_code);
			delete[] str_code;
			
			if(hint!=NULL)
			{
				vwf.set_text_color(fgColor);
				vwf.draw_text(__str_hint);
				vwf.set_text_color(hlColor);
				vwf.draw_text(hint);
			}
		}		
		
		BG_PALETTE[bgColor] = RGB5(24,0,0);		
		BG_PALETTE[hlColor] = RGB5(31,31,0);
		BG_PALETTE[fgColor] = RGB5(31,31,31);		
	}
	
	const int bgColor = 0;
	const int fgColor = 2;
	const int hlColor = 1;
};

void fatal(const char* msg, u32 err_code, const char* hint)
{
	FatalErrScreen scr(msg, err_code, hint);
	scr.init();
	scr.set_scroll(0,0);	
	scr.build_map();
	while (1) {		
		VBlankIntrWait();
	}
}

const char* __default_err_messages[] =
{
	/* ERR_UNKNOWN          */ "Unknown error",
	/* ERR_OAM_FULL         */ "OAM limit exceeded",
	/* ERR_OAM_INVALID_ID   */ "Invalid OAM id",
	/* ERR_OAM_ID_OVERFLOW  */ "Too many sprites have been created",
	/* ERR_DLG_EXCEEDED     */ "Up to 4 dialog boxes allowed per dialog background",
	/* ERR_DLG_INVALID_ID   */ "Invalid dialog box id",
	/* ERR_ARG_OUT_OF_RANGE */ "Argument or index out of range"
};

void fatal(ErrorCode code, const char* hint)
{
	fatal(__default_err_messages[code], code, hint);
}
