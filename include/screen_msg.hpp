#pragma once

const int LVL_NEXT = 1;
const int LVL_MENU = 2;
const int LVL_ENTER = 3;

#define LVL_ENTER_CODE(i) LVL_ENTER | (i<<2)