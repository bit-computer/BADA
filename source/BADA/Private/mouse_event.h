//***************************************************************************
// 
// 파일: mouse_event.h
// 
// 설명: 마우스 이벤트
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#ifndef MOUSE_EVENT_H
#define MOUSE_EVENT_H

#include "safe99_common/defines.h"

typedef enum mouse_state
{
    MOUSE_STATE_UP,
    MOUSE_STATE_DOWN
} mouse_state_t;

void on_move_mouse(const int32_t x, const int32_t y);
void on_down_left_mouse(void);
void on_up_left_mouse(void);
void on_down_right_mouse(void);
void on_up_right_mouse(void);

void release_mouse(void);

int32_t get_mouse_x(void);
int32_t get_mouse_y(void);

mouse_state_t get_left_mouse_state(void);
mouse_state_t get_right_mouse_state(void);

#endif // MOUSE_EVENT_H