#pragma once

const int LVL_NEXT = 1;
const int LVL_MENU = 2;
const int LVL_ENTER = 3;

const int LVL_COMPLETE = 0x80;
const int NEW_GAME = 0x40;

#define LVL_ENTER_CODE(i) LVL_ENTER | (i<<2)