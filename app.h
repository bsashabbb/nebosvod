#pragma once

#include <raylib.h>
#include "physics/physics_config.h"
#include "physics/physics_context.h"
#include "ui/beton.h"

typedef struct AppContext {
    PhysicsConfig cfg;
    PhysicsContext ctx;
    Camera2D camera;
    UIContext ui;
} AppContext;

void init_app(AppContext *app_ctx);

void start_app(AppContext *app_ctx);