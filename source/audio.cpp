#include "audio.hpp"

#include "debug.hpp"

#include "soundbank_bin.h"

#define assign_sfx(sfx_name, sfx_id,rate) mm_sound_effect sfx_name = { \
		{ sfx_id } ,		 \
		(int)(1.0f * (1<<rate)),	 \
		0,		 \
		255,	 \
		0,	     \
	}
	
assign_sfx(sfx_enter_lvl, SFX_SFX_ENTER_LVL,11);

void init_sound()
{
	// from: examples
	
	// Maxmod requires the vblank interrupt to reset sound DMA.
	// Link the VBlank interrupt to mmVBlank, and enable it. 
	irqSet( IRQ_VBLANK, mmVBlank );
	irqEnable(IRQ_VBLANK);
	
	// initialise maxmod with soundbank and 8 channels
    mmInitDefault( (mm_addr)soundbank_bin, 8 );
}

void play_sfx(mm_sound_effect* sfx)
{
	mmEffectEx((mm_sound_effect*)sfx);
}

void cancel_sfx(int sfx_id)
{
	mmEffectCancel(sfx_id);
}

void play_bgm(int mod_id)
{
	mmPause();
	mmStop();
	mmStart(mod_id, MM_PLAY_LOOP);
}

void stop_bgm()
{	
	mmStop();	
}