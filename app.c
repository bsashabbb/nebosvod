#include "app.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "core/particle.h"
#include "raylib.h"
#include "raymath.h"

#include "core/world.h"
#include "physics/integrator.h"
#include "physics/physics_config.h"
#include "physics/physics_context.h"
#include "spawn/spawn.h"
#include "spawn/spawn_config.h"
#include "util/const.h"
#include "util/stats.h"
#include "render/render_debug.h"
#include "render/render.h"
#include "ui/beton.h"

#define SPAWN_AREA_SIZE 10000 // размеры области спавна
#define SPAWN_MAX_SPEED 10 // максимальная скорость при спавне
#define SPAWN_MIN_MASS 2 // диапазон масс при спавне
#define SPAWN_MAX_MASS 10

#define SPAWN_CENTRAL_MASS 1000000 // масса центральной частицы
// #define SPAWN_CENTRAL_MASS 1000000 // масса центральной частицы

#define P_COUNT 10000 // число частиц
// #define P_COUNT 10 // число частиц
#define P_MAX 2048 // максимальное число частиц в симуляции

void init_app(AppContext *app_ctx) {
}

void start_app(AppContext *app_ctx) {
    // srand(time(NULL));
    srand(1);

    set_default_config(&app_ctx->cfg);

    init_ctx(&app_ctx->ctx, P_COUNT);
    // init_ctx(&ctx, 2);
    // init_ctx(&ctx, 3);

    // SpawnConfig scfg = (SpawnConfig){
    //     .bounds = (Rectangle){
    //         - 500,
    //         - 500,
    //         1000,
    //         1000
    //     },
    //     .min_mass = SPAWN_MIN_MASS,
    //     .max_mass = SPAWN_MAX_MASS,
    //     .max_speed = 3
    // };

    SpawnConfig scfg = (SpawnConfig){
        .min_r = 1000,
        .max_r = SPAWN_AREA_SIZE,
        .min_mass = SPAWN_MIN_MASS,
        .max_mass = SPAWN_MAX_MASS,
        .max_speed = SPAWN_MAX_SPEED
    };


    spawn_solar_system(&app_ctx->ctx.world, &scfg, &app_ctx->cfg, SPAWN_CENTRAL_MASS);
    // spawn_box_cluster(&ctx.world, &scfg, &cfg);
    // spawn_test(&ctx.world);
    print_world(&app_ctx->ctx.world);

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Небосвод v.0.2");
    SetTargetFPS(TARGET_FPS); 

    ToggleFullscreen();

    int screen_width = GetScreenWidth();
    int screen_height = GetScreenHeight();
    
    int dragging = false;
    int running = false;
    int render = true;
    int render_lines = true;

    int following_p = -1;
    Vector2 lastMousePosition = {0};
    float accumulator = 0;

    double lastUpdateTime = GetTime();

    char text_buffer[256]; 
    char text_buffer_1[20];
    unsigned int steps_per_frame = STEPS_PER_FRAME;

    int bottom_menu_height = 60;

    int checked = 0;
    float value = 0;

    int fontsize = 26;
    // Font font = LoadFontEx("assets/CourierPrime-Regular.ttf", fontsize, NULL, 0);
    Font font = LoadFontEx("assets/LiberationMono-Regular.ttf", fontsize, NULL, 0);

    // ========== bottom menu ==========
    UIRect bottom_menu_rect = {0, screen_height - bottom_menu_height, screen_width, bottom_menu_height};

    UILayout bottom_menu = UI_NewLayout(bottom_menu_rect, HORIZONTAL, 15, 0, 5);

    UIRect pausebutton0_rect = UI_NextColumnFixedHeight(&bottom_menu, 50, 50);
    UIRect label1_rect = UI_NextColumn(&bottom_menu, 60);
    UIRect checkbox0_rect = UI_NextColumnFixedHeight(&bottom_menu, 20, 20);
    UIRect slider0_rect = UI_NextColumnFixedHeight(&bottom_menu, 150, 20);
    UIRect label2_rect = UI_NextColumn(&bottom_menu, 60);

    // ========== side menu ==========
    int side_menu_width = 350;

    UIRect side_menu_rect = {screen_width - side_menu_width, 0, side_menu_width, screen_height - bottom_menu_height + 1};

    UILayout side_menu = UI_NewLayout(side_menu_rect, VERTICAL, 15, 15, 5);

    UIRect step_rect = UI_NextRow(&side_menu, fontsize * 1);
    UIRect fps_rect = UI_NextRow(&side_menu, fontsize * 2);
    UIRect stats_rect = UI_NextRow(&side_menu, fontsize * 8);

    // ========== simulation viewport ==========
    UIRect sim_viewport = {0, 0, screen_width - side_menu_width, screen_height - bottom_menu_height};

    app_ctx->camera.target = (Vector2){ 0, 0 };     // What point in world space the camera looks at
    app_ctx->camera.offset = (Vector2){ sim_viewport.width * 0.5f, sim_viewport.height * 0.5f }; // Center of the screen
    app_ctx->camera.rotation = 0.0f;                      // No rotation
    app_ctx->camera.zoom = 0.05f / RENDER_SCALE;                          // Normal zoom

    
    while (!WindowShouldClose()) {
        float frametime = GetFrameTime();
        int steps = 0;

        if (running) {
            // if (GetFPS() < TARGET_FPS * 0.5f && steps_per_frame > 0) {
            //     steps_per_frame *= (GetFPS() / TARGET_FPS);
            //     steps_per_frame = fmaxf(steps_per_frame, 1);
            // } else if (GetFPS() == TARGET_FPS && steps_per_frame < STEPS_PER_FRAME && world.step % 100 == 0) {
            //     steps_per_frame++;
            // }

            // int steps = steps_per_frame;
            // // steps_per_frame = app_ctx->ctx.world.step > 1700? 1 : 100;
            // for (int i = 0; i < steps_per_frame; i++) {
            //     step_world(&app_ctx->ctx, &app_ctx->cfg);
            //     // if (ctx.world.step > 2000) 
            //         // print_world(&app_ctx->ctx.world);
            //     // if (ctx.world.step > 2200) 
            //     //     return 0;
            // }

            // update_stats(&ctx, &cfg);  

            // accumulator += frametime * STEPS_PER_FRAME;
            // steps_per_frame = 0;
            // while (accumulator >= DT) {
            //     step_world(&world);
            //     accumulator -= DT;
            //     steps_per_frame++;
            // }        
            
            
            double sim_budget = 1.0f / TARGET_FPS; // ms per frame for simulation
            double start = GetTime();
            while ((GetTime() - start) < sim_budget) {
                step_world(&app_ctx->ctx, &app_ctx->cfg);
                steps++;
                if (app_ctx->ctx.world.step % 1000 == 0) {
                    printf("step: %d\n", app_ctx->ctx.world.step);
                    // print_world(&app_ctx->ctx.world);
                }
            }

            // double currentTime = GetTime();
            // if (currentTime - lastUpdateTime >= 1.0f) {
            //     update_stats(&world);  
            //     lastUpdateTime = currentTime;
            // }
            
            // print_world(&world);
        }


        // get input
        UI_Begin(&app_ctx->ui);

            // query UI
            ButtonState checkbox0 = CheckBoxLogic(&app_ctx->ui, checkbox0_rect, &checked, 3);
            ButtonState pausebutton0 = CheckBoxLogic(&app_ctx->ui, pausebutton0_rect, &running, 4);

            ButtonState slider0 = SliderLogic(&app_ctx->ui, slider0_rect, 5, 0, 10, &value);

            if (IsKeyReleased(KEY_SPACE)) running = !running;
            if (IsKeyReleased(KEY_R)) render = !render;
            if (IsKeyReleased(KEY_V)) render_lines = !render_lines;
            if (IsKeyReleased(KEY_F)) following_p = rand() % app_ctx->ctx.world.p_count;

            if (IsKeyDown(KEY_RIGHT)) app_ctx->camera.target.x += 600.0f / app_ctx->camera.zoom * frametime;
            if (IsKeyDown(KEY_LEFT)) app_ctx->camera.target.x -= 600.0f / app_ctx->camera.zoom * frametime;
            if (IsKeyDown(KEY_DOWN)) app_ctx->camera.target.y += 600.0f / app_ctx->camera.zoom * frametime;
            if (IsKeyDown(KEY_UP)) app_ctx->camera.target.y -= 600.0f / app_ctx->camera.zoom * frametime;
            if (IsKeyDown(KEY_C)) {
                app_ctx->camera.target.x = app_ctx->ctx.com_x;
                app_ctx->camera.target.y = app_ctx->ctx.com_y;
            }

            if (CheckPointInsideRect(app_ctx->ui.mouse_input.pos, sim_viewport)) {
                if (GetMouseWheelMove() == 1) app_ctx->camera.zoom *= 1.2f;
                if (GetMouseWheelMove() == -1) app_ctx->camera.zoom /= 1.2f;

                app_ctx->camera.zoom = fmaxf(app_ctx->camera.zoom, 0.01f / RENDER_SCALE);
                // Start drag
                if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
                    dragging = true;
                    lastMousePosition = app_ctx->ui.mouse_input.pos;
                }
            }

            // End drag
            if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
                dragging = false;
            }            

            // Handle dragging
            if (dragging) {
                following_p = -1;
                Vector2 currentMouse = app_ctx->ui.mouse_input.pos;
                Vector2 delta = Vector2Subtract(lastMousePosition, currentMouse);
                delta = Vector2Scale(delta, 1.0f / app_ctx->camera.zoom);
                app_ctx->camera.target = Vector2Add(app_ctx->camera.target, delta);
                lastMousePosition = currentMouse;
            }

            if (following_p >= 0) {
                Particle* p = &app_ctx->ctx.world.particles[following_p];
                app_ctx->camera.target = (Vector2){p->x * RENDER_SCALE, p->y * RENDER_SCALE};
            }
        
        UI_End(&app_ctx->ui);
        
        

        BeginDrawing();
                ClearBackground(BLACK);
                if (render) {                      
                    BeginMode2D(app_ctx->camera);  
                    // draw coordinate axes
                    // DrawLine(-BH_ROOT_SIZE * 0.5, 0, BH_ROOT_SIZE * 0.5, 0, GRAY);
                    // DrawLine(0, -BH_ROOT_SIZE * 0.5, 0, BH_ROOT_SIZE * 0.5, GRAY);

                    // visualise_bh_tree(&ctx.world);
                    // visualise_grid(&ctx);

                    render_world(&app_ctx->ctx.world, &app_ctx->camera, sim_viewport, render_lines, V_LINE_LENGTH);
                    EndMode2D();
                }

            // ===== render bottom menu =====
            Panel(bottom_menu_rect, BETON_PANEL_STYLE);

            Label(label1_rect, 20, BLACK, "Test");
            DrawCheckBox(checkbox0_rect, BETON_CHECKBOX_STYLE, checkbox0, checked, 1);
            DrawPauseButton(pausebutton0_rect, BETON_BUTTON_STYLE, pausebutton0, running, 1);
            DrawSlider(slider0_rect, slider0, 2, BETON_SLIDER_PANEL_STYLE, BETON_BUTTON_STYLE, value, 10.0f);
            
            sprintf(text_buffer_1, "%.2f", value);
            Label(label2_rect, 20, BLACK, text_buffer_1);

            // ===== render side menu =====
            Panel(side_menu_rect, BETON_PANEL_STYLE);

            snprintf(text_buffer, sizeof(text_buffer),
                "Step: %d",
                app_ctx->ctx.world.step
            );
            DrawTextEx(font, text_buffer, (Vector2){step_rect.x, step_rect.y}, fontsize, 0, BLACK);

            snprintf(text_buffer, sizeof(text_buffer),
                "FPS: %d\nSPS: %d",
                GetFPS(), GetFPS() * steps
            );
            DrawTextEx(font, text_buffer, (Vector2){fps_rect.x, fps_rect.y}, fontsize, 0, BLACK);

            snprintf(text_buffer, sizeof(text_buffer),
                "Particles: %u\nTotal mass: %.2f\nMax mass: %.2f\n"
                "Temperature: %.2f"
                "\nKin. energy: %.2f"
                "\nPot. energy: %.2f"
                "\nTotal energy: %.2f"
                "\nAng. momentum: %.2f\n",
                app_ctx->ctx.world.p_count, app_ctx->ctx.total_mass, app_ctx->ctx.max_mass,
                app_ctx->ctx.temperature, app_ctx->ctx.kin_energy, app_ctx->ctx.pot_energy,
                app_ctx->ctx.kin_energy + app_ctx->ctx.pot_energy,
                app_ctx->ctx.ang_momentum
            );
            DrawTextEx(font, text_buffer, (Vector2){stats_rect.x, stats_rect.y}, fontsize, 0, BLACK);
        EndDrawing();
    }

    free_ctx(&app_ctx->ctx);
}