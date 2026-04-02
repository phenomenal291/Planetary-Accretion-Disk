#include "../include/test.h"
#include <raylib.h>
Mover::Mover(float pos[], float v[], float a[], float mass){
    this->position[0] = pos[0];
    this->position[1] = pos[1];
    this->velocity[0] = v[0];
    this->velocity[1] = v[1];
    this->acceleration[0] = a[0];
    this->acceleration[1] = a[1];
    this->mass = mass;
}

void Mover::applyForce(float force[]){
    this->acceleration[0] += force[0] / this->mass;
    this->acceleration[1] += force[1] / this->mass;
}

void Mover::update(){
    this->velocity[0] += this->acceleration[0];
    this->velocity[1] += this->acceleration[1];
    this->position[0] += this->velocity[0];
    this->position[1] += this->velocity[1];
    // Reset acceleration after each update
    this->acceleration[0] = 0.0f;
    this->acceleration[1] = 0.0f;

    // check for boundaries
    if (this->position[0] > 800.0f) {
        this->position[0] = 800.0f;
        this->velocity[0] *= -0.8; // Reverse velocity on x-axis
    } else if (this->position[0] < 0.0f) {
        this->position[0] = 0.0f;
        this->velocity[0] *= -0.8f; // Reverse velocity on x-axis
    }

    if ( this->position[1] > 600.0f) {
        this->position[1] = 600.0f;
        this->velocity[1] *= -0.8f; // Reverse velocity on y-axis
    } 
    else if (this->position[1] < 0.0f) {
        this->position[1] = 0.0f;
        this->velocity[1] *= -0.8f; // Reverse velocity on y-axis
    }
}
void Mover::draw(){
    DrawCircle(this->position[0], this->position[1], 3 * this->mass, WHITE);
}

int main(){
    float v[2] = {0.0f, 0.0f};
    float a[2] = {0.0f, 0.0f};
    float pos1[2] = {600.0f, 100.0f};
    float pos2[2] = {200.0f, 100.0f};
    Mover mover1(pos1, v, a);
    Mover mover2(pos2, v, a, 10.0f);
    float wind[2] = {0.1f, 0.0f};
    float gravity[2] = {0.0f, 0.1f};

    InitWindow(800, 600, "Mover Simulation");
    SetTargetFPS(60);
    while (!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("Press ESC to exit, Click to apply wind force", 10, 10, 20, GRAY);

        mover1.applyForce(gravity);
        mover2.applyForce(gravity);
        if ( IsKeyDown(KEY_ESCAPE) ) break;
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
            mover1.applyForce(wind);
            mover2.applyForce(wind);
        }

        mover1.update();
        mover1.draw();

        mover2.update();
        mover2.draw();

        EndDrawing();
    }
}


