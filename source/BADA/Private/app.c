//***************************************************************************
// 
// 파일: app.c
// 
// 설명: APP
// 
// 작성자: bumpsgoodman
// 
// 작성일: 2023/09/12
// 
//***************************************************************************

#include "precompiled.h"
#include "app.h"

extern HWND g_hwnd;

static void update(void);
static void draw(void);

static void update_command(void);

static void print_info(void);

bool init_app(const size_t pixel_size)
{
    ASSERT(pixel_size > 0, "pixel_size == 0");

    // 콘솔창 커서 숨기기
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(console_handle, &info);

    memset(gp_app, 0, sizeof(app_t));

    // DLL 로드
    {
    #if defined(_WIN64)
        gp_app->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x64.dll");
        gp_app->h_file_system_dll = LoadLibrary(L"safe99_file_system_x64.dll");
    #else
        gp_app->h_renderer_dll = LoadLibrary(L"safe99_soft_renderer_2d_x86.dll");
        gp_app->h_file_system_dll = LoadLibrary(L"safe99_file_system_x86.dll");
    #endif // _WIN64

        if (gp_app->h_renderer_dll == NULL
            || gp_app->h_file_system_dll == NULL)
        {
            ASSERT(false, "Failed load DLL");
            goto failed_init;
        }

        // 인스턴스 생성 함수
        create_instance_func pf_create_renderer = (create_instance_func)GetProcAddress(gp_app->h_renderer_dll, "create_instance");
        create_instance_func pf_create_file_system = (create_instance_func)GetProcAddress(gp_app->h_file_system_dll, "create_instance");

        // 렌더러 초기화
        pf_create_renderer(&gp_app->p_renderer);
        if (!gp_app->p_renderer->vtbl->initialize(gp_app->p_renderer, g_hwnd))
        {
            ASSERT(false, "Failed init renderer");
            goto failed_init;
        }

        // 파일 시스템 초기화
        pf_create_file_system(&gp_app->p_file_system);
        if (!gp_app->p_file_system->vtbl->initialize(gp_app->p_file_system))
        {
            ASSERT(false, "Failed init file system");
            goto failed_init;
        }
}

    // 앱 초기화
    {
        gp_app->b_running = true;

        timer_init(&gp_app->frame_timer);
    }

    // 캔버스 초기화
    {
        gp_app->pixel_size = pixel_size;
        gp_app->rows = gp_app->p_renderer->vtbl->get_height(gp_app->p_renderer) / pixel_size;
        gp_app->cols = gp_app->p_renderer->vtbl->get_width(gp_app->p_renderer) / pixel_size;

        gp_app->pa_canvas = (uint32_t*)malloc(sizeof(uint32_t) * gp_app->rows * gp_app->cols);
        if (gp_app->pa_canvas == NULL)
        {
            ASSERT(false, "Failed to malloc canvas");
            goto failed_init;
        }
        memset(gp_app->pa_canvas, 0xffffffff, sizeof(uint32_t) * gp_app->rows * gp_app->cols);

        gp_app->cur_color = 0xffff0000;
        gp_app->command = COMMAND_DRAW_PIXEL;

        set_canvas(gp_app->pa_canvas, gp_app->rows, gp_app->cols);
    }

    // 도움말 띄워주기
    key_down(VK_F1);

    return true;

failed_init:
    shutdown_app();
    return false;
}

void shutdown_app(void)
{
    release_canvas();

    SAFE_FREE(gp_app->pa_canvas);

    SAFE_RELEASE(gp_app->p_renderer);
    SAFE_RELEASE(gp_app->p_file_system);
}

void tick_app(void)
{
    static size_t frame_count;
    static float start;

    const float end = timer_get_time(&gp_app->frame_timer);
    const float elapsed = (end - start) * 1000.0f;
    if (elapsed >= 1000.0f)
    {
        gp_app->fps = frame_count;
        frame_count = 0;
        start = end;
    }

    ++frame_count;

    update();
    draw();

    print_info();
}

static void update(void)
{
    static uint32_t click_count = 0;

    update_command();

    if (is_key_pressed(VK_F1))
    {
        const wchar_t* text = L"\
            F1: 도움말\n\
            F2: 색상 선택\n\
            F3: 모두 지우기\n\
            F5: 되돌리기\n\
            F6: 되돌리기 취소\n\n\
            1: 픽셀 그리기\n\
            2: 지우개\n\
            3: 채우기\n\
            4: 수평선 그리기\n\
            5: 수직선 그리기\n\
            6: 사각형 그리기";
        MessageBox(NULL, text, L"도움말", MB_OK | MB_ICONINFORMATION);
    }

    if (is_key_pressed(VK_F2))
    {
        static COLORREF custom_colors[16];
        uint32_t r = (gp_app->cur_color & 0xFF0000) >> 16;
        uint32_t g = gp_app->cur_color & 0xFF00;
        uint32_t b = (gp_app->cur_color & 0xFF) << 16;

        uint32_t default_color = r | g | b;

        CHOOSECOLOR cc;
        ZeroMemory(&cc, sizeof(cc));
        cc.lStructSize = sizeof(cc);
        cc.lpCustColors = (LPDWORD)custom_colors;
        cc.Flags = CC_FULLOPEN | CC_RGBINIT;
        cc.hwndOwner = g_hwnd;
        cc.rgbResult = default_color;

        if (ChooseColor(&cc))
        {
            uint32_t r = (cc.rgbResult & 0xFF) << 16;
            uint32_t g = cc.rgbResult & 0xFF00;
            uint32_t b = (cc.rgbResult & 0xFF0000) >> 16;
            gp_app->cur_color = (0xff << 24) | r | g | b;
        }
    }

    if (is_key_pressed(VK_F3))
    {
        fill_canvas(0xffffffff);
    }

    if (is_key_pressed(VK_F5))
    {
        undo();
    }

    if (is_key_pressed(VK_F6))
    {
        redo();
    }

    if (get_left_mouse_state() == MOUSE_STATE_DOWN)
    {
        int32_t x = get_mouse_x() / (int32_t)gp_app->pixel_size;
        int32_t y = get_mouse_y() / (int32_t)gp_app->pixel_size;

        switch (gp_app->command)
        {
        case COMMAND_DRAW_PIXEL:
            draw_pixel(x, y, gp_app->cur_color);
            break;
        case COMMAND_REMOVE_PIXEL:
            remove_pixel(x, y);
            break;
        case COMMAND_FILL_CANVAS:
            fill_canvas(gp_app->cur_color);
            break;
        case COMMAND_DRAW_HORIZONTAL_LINE:
            draw_horizontal_line(y, gp_app->cur_color);
            break;
        case COMMAND_DRAW_VERTICAL_LINE:
            draw_vertical_line(x, gp_app->cur_color);
            break;
        case COMMAND_DRAW_RECTANGLE:
        {
            static int32_t prev_clicked_x;
            static int32_t prev_clicked_y;

            if (click_count == 0)
            {
                prev_clicked_x = x;
                prev_clicked_y = y;

                ++click_count;
            }
            else
            {
                int32_t start_x = MIN(prev_clicked_x, x);
                int32_t start_y = MIN(prev_clicked_y, y);
                int32_t end_x = MAX(prev_clicked_x, x);
                int32_t end_y = MAX(prev_clicked_y, y);

                draw_rectangle(start_x, start_y, end_x, end_y, gp_app->cur_color);

                prev_clicked_x = 0;
                prev_clicked_y = 0;

                click_count = 0;
            }
            break;
        }
        default:
            ASSERT(false, "Invalid drawing mode");
            break;
        }

        release_mouse();
    }

    release_keyboard();
}

static void draw(void)
{
    gp_app->p_renderer->vtbl->begin_draw(gp_app->p_renderer);
    {
        for (size_t r = 0; r < gp_app->rows; ++r)
        {
            const size_t y = gp_app->pixel_size * r + 1;
            for (size_t c = 0; c < gp_app->cols; ++c)
            {
                const size_t x = gp_app->pixel_size * c + 1;
                const uint32_t color = gp_app->pa_canvas[r * gp_app->cols + c];

                gp_app->p_renderer->vtbl->draw_rectangle(gp_app->p_renderer,
                                                         (int32_t)x, (int32_t)y,
                                                         gp_app->pixel_size - 1, gp_app->pixel_size - 1,
                                                         color);
            }
        }
    }
    gp_app->p_renderer->vtbl->end_draw(gp_app->p_renderer);

    gp_app->p_renderer->vtbl->on_draw(gp_app->p_renderer);
}

static void update_command(void)
{
    if (is_key_pressed('1'))
    {
        gp_app->command = COMMAND_DRAW_PIXEL;
    }

    if (is_key_pressed('2'))
    {
        gp_app->command = COMMAND_REMOVE_PIXEL;
    }

    if (is_key_pressed('3'))
    {
        gp_app->command = COMMAND_FILL_CANVAS;
    }

    if (is_key_pressed('4'))
    {
        gp_app->command = COMMAND_DRAW_HORIZONTAL_LINE;
    }

    if (is_key_pressed('5'))
    {
        gp_app->command = COMMAND_DRAW_VERTICAL_LINE;
    }

    if (is_key_pressed('6'))
    {
        gp_app->command = COMMAND_DRAW_RECTANGLE;
    }
}

static void print_info(void)
{
    COORD pos = { 0,0 };

    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);

    printf("window info\n");
    printf("- window width = %zd\n", gp_app->p_renderer->vtbl->get_width(gp_app->p_renderer));
    printf("- window height = %zd\n\n", gp_app->p_renderer->vtbl->get_height(gp_app->p_renderer));

    printf("canvas info\n");
    printf("- pixel size: %zd\n", gp_app->pixel_size);
    printf("- rows: %zd\n", gp_app->rows);
    printf("- cols: %zd\n\n", gp_app->cols);

    const uint32_t color = gp_app->cur_color;
    printf("color info\n");
    printf("- argb:  0x%08x\n", color);
    printf("- alpha: 0x%02x\n", (color & 0xff000000) >> 24);
    printf("- red:   0x%02x\n", (color & 0x00ff0000) >> 16);
    printf("- green: 0x%02x\n", (color & 0x0000ff00) >> 8);
    printf("- blue:  0x%02x\n\n", (color & 0x000000ff));

    printf("command info\n");
    switch (gp_app->command)
    {
    case COMMAND_DRAW_PIXEL:
        printf("- draw pixel              \n");
        break;
    case COMMAND_REMOVE_PIXEL:
        printf("- remove pixel            \n");
        break;
    case COMMAND_FILL_CANVAS:
        printf("- fill canvas             \n");
        break;
    case COMMAND_DRAW_HORIZONTAL_LINE:
        printf("- draw horizontal line    \n");
        break;
    case COMMAND_DRAW_VERTICAL_LINE:
        printf("- draw vertical line      \n");
        break;
    case COMMAND_DRAW_RECTANGLE:
        printf("- draw rectangle          \n");
        break;
    default:
        ASSERT(false, "Invalid drawing mode");
        break;
    }
    printf("\n");

    printf("FPS: %zd                      \n", gp_app->fps);
}