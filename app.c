#include "app.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
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


#define V_LINE_LENGTH 1 // длина линий скорости

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
    // srand(time(NULL));
    srand(1);

    set_default_config(&app_ctx->cfg);

    PhysicsContext ctx;
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

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Небосвод v.0.1.2");
    SetTargetFPS(TARGET_FPS); 

    app_ctx->camera.target = (Vector2){ 0, 0 };     // What point in world space the camera looks at
    app_ctx->camera.offset = (Vector2){ WINDOW_WIDTH * 0.5f, WINDOW_HEIGHT * 0.5f }; // Center of the screen
    app_ctx->camera.rotation = 0.0f;                      // No rotation
    app_ctx->camera.zoom = 0.05f;                          // Normal zoom
}

void start_app(AppContext *app_ctx) {

    bool dragging = false;
    bool running = false;
    bool render = true;
    Vector2 lastMousePosition = {0};
    float accumulator = 0;

    double lastUpdateTime = GetTime();

    char text_buffer[256]; 
    unsigned int steps_per_frame = STEPS_PER_FRAME;
    
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
            // steps_per_frame = ctx.world.step > 1700? 1 : 100;
            // for (int i = 0; i < steps_per_frame; i++) {
            //     step_world(&ctx, &cfg);
            //     // if (ctx.world.step > 2000) 
            //         print_world(&ctx.world);
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
        
        if (IsKeyReleased(KEY_SPACE)) running = !running;
        if (IsKeyReleased(KEY_R)) render = !render;

        if (IsKeyDown(KEY_RIGHT)) app_ctx->camera.target.x += 600.0f / app_ctx->camera.zoom * frametime;
        if (IsKeyDown(KEY_LEFT)) app_ctx->camera.target.x -= 600.0f / app_ctx->camera.zoom * frametime;
        if (IsKeyDown(KEY_DOWN)) app_ctx->camera.target.y += 600.0f / app_ctx->camera.zoom * frametime;
        if (IsKeyDown(KEY_UP)) app_ctx->camera.target.y -= 600.0f / app_ctx->camera.zoom * frametime;
        if (IsKeyDown(KEY_C)) {
            app_ctx->camera.target.x = app_ctx->ctx.com_x;
            app_ctx->camera.target.y = app_ctx->ctx.com_y;
        }

        if (GetMouseWheelMove() == 1) app_ctx->camera.zoom *= 1.2f;
        if (GetMouseWheelMove() == -1) app_ctx->camera.zoom /= 1.2f;

        if (app_ctx->camera.zoom < 0.01f) app_ctx->camera.zoom = 0.01f;
        // Start drag
        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            dragging = true;
            lastMousePosition = GetMousePosition();
        }

        // End drag
        if (IsMouseButtonReleased(MOUSE_BUTTON_MIDDLE)) {
            dragging = false;
        }

        // Handle dragging
        if (dragging) {
            Vector2 currentMouse = GetMousePosition();
            Vector2 delta = Vector2Subtract(lastMousePosition, currentMouse);
            delta = Vector2Scale(delta, 1.0f / app_ctx->camera.zoom);
            app_ctx->camera.target = Vector2Add(app_ctx->camera.target, delta);
            lastMousePosition = currentMouse;
        }

        BeginDrawing();
            BeginMode2D(app_ctx->camera);
                ClearBackground(BLACK);
                if (render) {                        
                    // draw coordinate axes
                    // DrawLine(-BH_ROOT_SIZE * 0.5, 0, BH_ROOT_SIZE * 0.5, 0, GRAY);
                    // DrawLine(0, -BH_ROOT_SIZE * 0.5, 0, BH_ROOT_SIZE * 0.5, GRAY);

                    // visualise_bh_tree(&ctx.world);
                    // visualise_grid(&ctx);

                    render_world(&app_ctx->ctx.world, V_LINE_LENGTH);
                }
            EndMode2D();

            snprintf(text_buffer, sizeof(text_buffer),
                "FPS: %d\nSPS: %d\nStep: %d\n"
                "Particles: %u\nTotal mass: %.2f\nMax mass: %.2f\n"
                "Temperature: %.2f\nKinetic energy: %.2f\nPotential energy: %.2f\nTotal energy: %.2f\nAng. momentum: %.2f\n",
                GetFPS(), GetFPS() * steps, app_ctx->ctx.world.step,
                app_ctx->ctx.world.p_count, app_ctx->ctx.total_mass, app_ctx->ctx.max_mass,
                app_ctx->ctx.temperature, app_ctx->ctx.kin_energy, app_ctx->ctx.pot_energy,
                app_ctx->ctx.kin_energy + app_ctx->ctx.pot_energy,
                app_ctx->ctx.ang_momentum
            );
            DrawText(text_buffer, 10, 10, 20, GREEN);
        EndDrawing();
    }

    free_ctx(&app_ctx->ctx);
}