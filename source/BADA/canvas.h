//***************************************************************************
// 
// 파일: canvas.h
// 
// 설명: 캔버스
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#ifndef CANVAS_H
#define CANVAS_H

#include "safe99_common/defines.h"

typedef enum command {
    COMMAND_DRAW_PIXEL,
    COMMAND_REMOVE_PIXEL,
    COMMAND_FILL_CANVAS,
    COMMAND_DRAW_HORIZONTAL_LINE,
    COMMAND_DRAW_VERTICAL_LINE,
    COMMAND_DRAW_RECTANGLE,
    COMMAND_UNDO,
    COMMAND_REDO,
} command_t;

START_EXTERN_C

void set_canvas(uint32_t* canvas, size_t rows, size_t cols);

void release_canvas(void);

void draw_pixel(size_t x, size_t y, uint32_t rgb_color);

void remove_pixel(size_t x, size_t y);

void fill_canvas(uint32_t rgb_color);

void draw_horizontal_line(size_t y, uint32_t rgb_color);

void draw_vertical_line(size_t x, uint32_t rgb_color);

void draw_rectangle(size_t start_x, size_t start_y, size_t end_x, size_t end_y, uint32_t rgb_color);

bool undo(void);

bool redo(void);

END_EXTERN_C

#endif // CANVAS_H