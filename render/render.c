#include <raylib.h>

#include "../core/world.h"

void render_world(const World *world, float v_line_length) {
    for (int i = 0; i < world->p_count; i++) {
        const Particle *prt = &world->particles[i];
        // DrawCircle(prt->x, prt->y, fmaxf(prt->radius, 1.0f / camera.zoom), WHITE);
        DrawCircle(prt->x, prt->y, prt->radius, WHITE);
        DrawLine(prt->x, prt->y, 
        prt->x + prt->vx * v_line_length, prt->y + prt->vy * v_line_length, GREEN);
    }
}