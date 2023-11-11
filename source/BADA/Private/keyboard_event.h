//***************************************************************************
// 
// 파일: keyboard_event.h
// 
// 설명: 키보드 이벤트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#ifndef KEYBOARD_EVENT_H
#define KEYBOARD_EVENT_H

#include "safe99_common/defines.h"

void key_down(const uint8_t key);
void key_up(const uint8_t key);
void release_keyboard(void);
bool is_key_pressed(const uint8_t key);

#endif // KEYBOARD_EVENT_H