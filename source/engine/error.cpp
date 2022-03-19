#include "error.hpp"

#include "engine/scrollmap.hpp"

const char* __str_err_intro = "Execution stopped due to a fatal error.\nError message:\n\n";
const char* __str_err_code = "Error code : ";
const char* __str_hex = "0123456789ABCDEF";

class FatalErrScreen : public DialogBackground
{	
public:
	FatalErrScreen(const char* msg,u32 err_code) : DialogBackground(0, 0, 28)
	{
		u32 _0 = 0;
		CpuFastSet(&_0, BG_PALETTE, 128 | FILL);
		CpuFastSet(&_0, (void*)VRAM,2*240*260 | FILL);
		clear_map();
		Vwf vwf=Vwf(defaultFont816);
		
		create_dialog_box(0, 0, 30, 14, &vwf);
		
		vwf.set_text_color(2);
		vwf.draw_text(__str_err_intro);
		
		vwf.set_text_color(1);
		vwf.draw_text(msg);				

		if(err_code != 0)
		{
			create_dialog_box(0, 15, 30, 4, &vwf);
			vwf.set_text_color(2);
			vwf.draw_text(__str_err_code);
			vwf.set_text_color(1);
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
		}		
		
		BG_PALETTE[0] = RGB5(24,0,0);		
		BG_PALETTE[1] = RGB5(31,31,0);
		BG_PALETTE[2] = RGB5(31,31,31);
	}
};

void fatal(const char* msg, u32 err_code)
{
	FatalErrScreen scr(msg, err_code);
	scr.set_scroll(0,0);	
	scr.build_map();
	while (1) {		
		VBlankIntrWait();				
	}
}

const char* __default_err_messages[] =
{
	/* ERR_UNKNOWN         */ "Unknown error",
	/* ERR_OAM_FULL        */ "OAM limit exceeded",
	/* ERR_OAM_INVALID_ID  */ "Invalid OAM id",
	/* ERR_OAM_ID_OVERFLOW */ "Too many sprites have been created",
	/* ERR_DLG_EXCEEDED    */ "Up to 4 dialog boxes allowed per dialog background",
	/* ERR_DLG_INVALID_ID  */ "Invalid dialog box id"
};

void fatal(ErrorCode code)
{
	fatal(__default_err_messages[code], code);
}
