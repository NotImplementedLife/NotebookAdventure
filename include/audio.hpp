#pragma once

#include <gba.h>
#include <maxmod.h>
#include "soundbank.h"

#define decl_sfx(sfx_name) extern mm_sound_effect sfx_name

decl_sfx(sfx_enter_lvl);

void init_sound();

void play_sfx(mm_sound_effect* sfx);

void cancel_sfx(int sfx_id);

void play_bgm(int mod_id);

void stop_bgm();