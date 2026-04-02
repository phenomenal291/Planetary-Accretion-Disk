#include "particle.cpp"
#include "raylib.h"
#include <vector>
#include "quadtree.cpp"
#include "octree.cpp"
#include <execution>
using namespace std;

vector<Particle> ChaosParticles(int numDust, float x_origin, float y_origin, float mass_origin) {
    vector<Particle> particles;
    
    for (int i = 0; i < numDust; i++) {
        float angle = GetRandomValue(0, 360) * (PI / 180.0f);
        float radius = GetRandomValue(100, 800); 
        
        float px = x_origin + std::cos(angle) * radius;
        float py = y_origin + std::sin(angle) * radius;
        
        float chaosVx = ((GetRandomValue(0, 100) / 100.0f) - 0.5f) * 6.0f;
        float chaosVy = ((GetRandomValue(0, 100) / 100.0f) - 0.5f) * 6.0f;

        float perfectSpeed = std::sqrt((G * mass_origin) / radius); 
        float spinVx = std::cos(angle + PI / 2.0f) * perfectSpeed * 0.3f; 
        float spinVy = std::sin(angle + PI / 2.0f) * perfectSpeed * 0.3f;
        
        float vx = chaosVx + spinVx;
        float vy = chaosVy + spinVy;
        
        Particle dust(Vector<float>(px, py, 0.0f), 
                      Vector<float>(vx, vy, 0.0f), 
                      Vector<float>(0.0f, 0.0f, 0.0f), 
                      1.0f); 
        
        particles.push_back(dust);
    }
    return particles;
}

vector<Particle> RingParticles(int numDust, float x_origin, float y_origin, float mass_origin) {
    vector<Particle> particles;
    
    for (int i = 0; i < numDust; i++) {
        float angle = GetRandomValue(0, 360) * (PI / 180.0f);
        float radius = GetRandomValue(200, 400); 
        
        float px = x_origin + std::cos(angle) * radius;
        float py = y_origin + std::sin(angle) * radius;
        
        float perfectSpeed = 0.95f * std::sqrt((G * mass_origin) / radius); 
        float vx = std::cos(angle + PI / 2.0f) * perfectSpeed; 
        float vy = std::sin(angle + PI / 2.0f) * perfectSpeed;
        
        Particle dust(Vector<float>(px, py, 0.0f), 
                      Vector<float>(vx, vy, 0.0f), 
                      Vector<float>(0.0f, 0.0f), 
                      1.0f); 
        
        particles.push_back(dust);
    }
    return particles;
}

vector<Particle> Ring3DParticles(int numDust, float x_origin, float y_origin, float z_origin, float mass_origin) {
    vector<Particle> particles;
    
    for (int i = 0; i < numDust; i++) {
        float angle = GetRandomValue(0, 360) * (PI / 180.0f);
        float radius = GetRandomValue(200, 500); 
        
        float px = x_origin + std::cos(angle) * radius;
        float py = y_origin + std::sin(angle) * radius;
        float pz = z_origin + ((GetRandomValue(0, 100) / 100.0f) - 0.5f) * 4.0f; 
        
        float perfectSpeed = 0.95f * std::sqrt((G * mass_origin) / radius); 
        float vx = std::cos(angle + PI / 2.0f) * perfectSpeed; 
        float vy = std::sin(angle + PI / 2.0f) * perfectSpeed;
        float vz = ((GetRandomValue(0, 100) / 100.0f) - 0.5f) * 2.0f; 
        
        Particle dust(Vector<float>(px, py, pz), 
                      Vector<float>(vx, vy, vz), 
                      Vector<float>(0.0f, 0.0f, 0.0f), 
                      1.0f); 
        
        particles.push_back(dust);
    }
    return particles;
}

void simulation2D() {
    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int numDust = 2000; 
    
    InitWindow(screenWidth, screenHeight, "Accretion Disk Simulation");
    SetTargetFPS(60);
    
    Vector<float> sunPos(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f);
    Particle Sun(sunPos, Vector<float>(0.0f, 0.0f, 0.0f), Vector<float>(0.0f, 0.0f, 0.0f), 50000.0f);

    vector<Particle> particles = RingParticles(numDust, Sun.pos.GetX(), Sun.pos.GetY(), Sun.mass);          

    float dt = 0.1f; 



    while (!WindowShouldClose()) {

        QuadTree quadTree(Vector<float>(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f), 
                          Vector<float>(4000.0f, 4000.0f, 0.0f));

        quadTree.insert(&Sun);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) { 
                quadTree.insert(&p);
            }
        }

        quadTree.update_mass(); 

        Sun.update(dt);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) p.update(dt);
        }

        quadTree.interact(&Sun);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) quadTree.interact(&p);
        }

        Sun.updateVelocity(dt);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) p.updateVelocity(dt);
        }

        // --- DRAWING ---
        BeginDrawing();
        
        DrawRectangle(0, 0, screenWidth, screenHeight, ColorAlpha(BLACK, 0.4f)); 
        
        Sun.draw();
        for (auto& p : particles) {
            if (p.mass > 0.0f) p.draw();
        }
        
        int activeCount = 0;
        for (auto& p : particles) if (p.mass > 0.0f && p.isActive) activeCount++;
        DrawText(TextFormat("Objects Remaining: %d", activeCount + 1), 10, 10, 20, GREEN);

        EndDrawing();
    }
    
    CloseWindow();
}

void simulation3D() {

    const int screenWidth = 1920;
    const int screenHeight = 1080;
    const int numDust = 1500; 
    Camera3D camera = { 0 };
    camera.position = { 0.0f, 1500.0f, 1500.0f }; // Placed high up and pulled back
    camera.target = { screenWidth / 2.0f, screenHeight / 2.0f, 0.0f };         // Looking at the Sun
    camera.up = { 0.0f, 1.0f, 0.0f };             // Which way is "up"
    camera.fovy = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    
    InitWindow(screenWidth, screenHeight, "Accretion Disk Simulation");
    SetTargetFPS(60);
    
    Vector<float> sunPos(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f);
    Particle Sun(sunPos, Vector<float>(0.0f, 0.0f, 0.0f), Vector<float>(0.0f, 0.0f, 0.0f), 50000.0f);

    vector<Particle> particles = Ring3DParticles(numDust, Sun.pos.GetX(), Sun.pos.GetY(), Sun.pos.GetZ(), Sun.mass);          

    float dt = 0.1f; 

    float cameraRadius = 2500.0f;
    float cameraAngleX = PI / 4.0f; 
    float cameraAngleY = PI / 4.0f; 

    while (!WindowShouldClose()) {


        // ------ CAMERA ------ 
        UpdateCamera(&camera, CAMERA_ORBITAL);
        cameraRadius -= GetMouseWheelMove() * 150.0f; // Adjust 150.0f for zoom speed
        if (cameraRadius > 8000.0f) cameraRadius = 8000.0f;
        if (cameraRadius < 300.0f) cameraRadius = 300.0f;   // Prevent zooming INSIDE the Sun
        OctTree octTree(Vector<float>(screenWidth / 2.0f, screenHeight / 2.0f, 0.0f), 
                         Vector<float>(4000.0f, 4000.0f, 4000.0f));

        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 mouseDelta = GetMouseDelta();
            
            cameraAngleX -= mouseDelta.x * 0.005f; // Left/Right spin
            cameraAngleY -= mouseDelta.y * 0.005f; // Up/Down spin
            
            // Clamp the vertical angle so the camera doesn't flip upside down
            if (cameraAngleY < 0.01f) cameraAngleY = 0.01f;
            if (cameraAngleY > PI - 0.01f) cameraAngleY = PI - 0.01f;
        }
        camera.position.x = Sun.pos.GetX() + cameraRadius * std::sin(cameraAngleY) * std::cos(cameraAngleX);
        camera.position.y = Sun.pos.GetY() + cameraRadius * std::cos(cameraAngleY);
        camera.position.z = Sun.pos.GetZ() + cameraRadius * std::sin(cameraAngleY) * std::sin(cameraAngleX);
        // Keep the camera locked dead-center on the Sun
        camera.target = { Sun.pos.GetX(), Sun.pos.GetY(), Sun.pos.GetZ() };


        // ------ SIMULATION ------
        octTree.insert(&Sun);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) { 
                octTree.insert(&p);
            }
        }

        octTree.update_mass(); 

        Sun.update(dt);
        std::for_each(std::execution::par, particles.begin(), particles.end(), [&](Particle& p){
            if ( p.isActive && p.mass > 0.0f) p.update(dt);
        });

        octTree.interact(&Sun);
        for (auto& p : particles) {
            if (p.isActive && p.mass > 0.0f) octTree.interact(&p);
        }

        Sun.updateVelocity(dt);
        std::for_each(std::execution::par, particles.begin(), particles.end(), [&](Particle& p){
            if ( p.isActive && p.mass > 0.0f) p.updateVelocity(dt);
        });



        // --- DRAWING ---
        BeginDrawing();
        ClearBackground(BLACK);       
        BeginMode3D(camera);
        
        DrawSphere({Sun.pos.GetX(), Sun.pos.GetY(), Sun.pos.GetZ()}, 100.0f, GRAY);

        Vector3 sunDrawPos = { Sun.pos.GetX(), Sun.pos.GetY(), Sun.pos.GetZ() };

        // 1. Draw the bright, solid core
        DrawSphere(sunDrawPos, Sun.radius, RAYWHITE);
        for ( int i = 1; i <= 8; i++){
            
            float auraRadius = Sun.radius + ( i * 6.0f);
            float alpha = 0.2f / i;

            DrawSphere(sunDrawPos, auraRadius, ColorAlpha(ORANGE, alpha));
        }
        DrawSphereWires(sunDrawPos, Sun.radius + 1.0f, 16, 16, ColorAlpha(RED, 0.4f));

        for (auto& p : particles) {
            if (p.mass > 0.0f) {
                Vector3 pDrawPos = { p.pos.GetX(), p.pos.GetY(), p.pos.GetZ() };
                
                // Draw the solid planet
                DrawSphere(pDrawPos, p.radius, GRAY);
                
                if (p.radius > 5.0f) {
                    DrawSphereWires(pDrawPos, p.radius + 0.2f, 8, 8, DARKGRAY);
                }
            }
        }


        EndMode3D();
        // FPS counter
        DrawText(TextFormat("FPS: %d", GetFPS()), 10, 10, 20, GREEN);
        EndDrawing();
    }
    
    CloseWindow();
}

int main() {

    //simulation2D();
    simulation3D();
    return 0;
}

