#pragma once

#include <gba.h>

extern const int LevelsCount;

struct __UserData
{
	u16 current_level;
	u32 time_played_as_human;
	u32 time_played_as_cat;
	u8 goddess_count;
	u8 goddess_collected;
};

extern __UserData UserData;

void load_user_data();

void save_user_data();