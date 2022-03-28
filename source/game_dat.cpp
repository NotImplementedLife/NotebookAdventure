#include "game_dat.hpp"

#include "engine/sram.hpp"

const int LevelsCount = 1;

__UserData UserData;

void load_user_data()
{
	if(SaveTool::check_integrity())
	{
		SaveTool::read(&UserData, sizeof(UserData), 0x10);
	}
	else
	{
		//u32 _0 = 0;
		//CpuFastSet(&_0, &UserData, (sizeof(UserData)/sizeof(u16)) | FILL | COPY16); // breaks level???
		for(u32 i=0;i<sizeof(UserData);i++)
			((u8*)&UserData)[i]=0;
	}
}

void save_user_data()
{	
	SaveTool::write(&UserData, sizeof(UserData), 0x10);
}