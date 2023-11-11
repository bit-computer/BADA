//***************************************************************************
// 
// 파일: keyboard_event.c
// 
// 설명: 키보드 이벤트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#include "precompiled.h"

static bool sb_key_map[256];

void key_down(const uint8_t key)
{
    sb_key_map[key] = true;
}

void key_up(const uint8_t key)
{
    sb_key_map[key] = false;
}

void release_keyboard(void)
{
    memset(sb_key_map, false, sizeof(sb_key_map));
}

bool is_key_pressed(const uint8_t key)
{
    return sb_key_map[key];
}