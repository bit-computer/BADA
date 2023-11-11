//***************************************************************************
// 
// 파일: app.h
// 
// 설명: APP
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#ifndef APP_H
#define APP_H

#include "BADA/canvas.h"
#include "safe99_common/defines.h"
#include "safe99_soft_renderer_2d/i_soft_renderer_2d.h"
#include "safe99_file_system/i_file_system.h"

typedef struct app
{
    // DLL 핸들
    HINSTANCE h_renderer_dll;
    HINSTANCE h_file_system_dll;

    // DLL 인스턴스
    i_soft_renderer_2d_t* p_renderer;
    i_file_system_t* p_file_system;

    // 앱
    bool b_running;
    
    timer_t frame_timer;
    size_t fps;

    // 캔버스
    size_t pixel_size;
    size_t rows;
    size_t cols;
    uint32_t* pa_canvas;

    uint32_t cur_color;
    command_t command;
} app_t;

app_t* gp_app;

START_EXTERN_C

bool init_app(const size_t pixel_size);
void shutdown_app(void);
void tick_app(void);

FORCEINLINE bool is_running_app(void)
{
    return gp_app->b_running;
}

FORCEINLINE void update_window_pos_app(void)
{
    gp_app->p_renderer->vtbl->update_window_pos(gp_app->p_renderer);
}

FORCEINLINE void update_window_size_app(void)
{
    gp_app->p_renderer->vtbl->update_window_size(gp_app->p_renderer);
}

END_EXTERN_C

#endif // APP_H