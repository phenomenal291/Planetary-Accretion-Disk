#include "../include/particle.h"
#include "raylib.h"
#include <cmath>
#include <string>

void Particle::updateRadius() {
    this->radius = 1.5f + std::sqrt(this->mass) * 0.5f;
}

Particle::Particle(Vector<float> pos, Vector<float> v, Vector<float> a, float m) {
    this->pos = pos;
    this->velocity = v;
    this->acceleration = a;
    this->mass = m;
    updateRadius();
}

void Particle::applyForce(const Vector<float>& force) {
    if ( !this->isActive) return;
    this->acceleration = this->acceleration + (force / this->mass);
}

void Particle::update(float dt){
    if ( !this->isActive) return;
    this->pos = this->pos + this->velocity * dt + this->acceleration * ( 0.5f * dt * dt); 
    this->velocity = this->velocity + this->acceleration * (0.5f * dt);
    this->acceleration = Vector<float>(0.0f, 0.0f, 0.0f); 

//     trail.push_back(this->pos);
//     if (trail.size() > maxTrailLength) {
//         trail.erase(trail.begin()); // Remove the oldest point
//     }
// 
}



void Particle::interact(Particle* other){
    if ( !this->isActive || !other->isActive) return;

    Vector<float> dd = this->pos - other->pos;
    float r_squared = dd.GetX() * dd.GetX() + dd.GetY() * dd.GetY() + dd.GetZ() * dd.GetZ();
     if ( r_squared < 1.0f) r_squared = 1.0f; 
    float r = std::sqrt(r_squared);

    if ( r < (this->radius + other->radius)) {
        float total_mass = this->mass + other->mass;
        Vector<float> new_velocity = ( this->velocity * this-> mass + other->velocity * other->mass) / total_mass;
        Vector<float> new_pos = ( this->pos * this-> mass + other->pos * other->mass) / total_mass;
        
        if ( this->mass >= other->mass) {
            this->velocity = new_velocity;
            this->pos = new_pos;
            this->mass = total_mass;
            updateRadius();
            other->isActive = false;
            other->mass = 0.0f;
        } else {
            other->velocity = new_velocity;
            other->pos = new_pos;
            other->mass = total_mass;
            updateRadius();
            this->isActive = false;
            this->mass = 0.0f;
        }
        return;
    }


    float force_magnitude = (G * this->mass * other->mass) / (r_squared * r);
    Vector<float> force_direction = dd * force_magnitude; 


    this->applyForce(force_direction  * -1.0f);
}


void Particle::updateVelocity(float dt){
    if ( !this->isActive) return;
    this->velocity = this->velocity + this->acceleration * (0.5f * dt);
}


void Particle::draw() {
    if ( !this->isActive) return;
    // for (size_t i = 1; i < trail.size(); i++) {
    //     // Calculate how faded this line segment should be (0.0 to 1.0)
    //     float alpha = (float)i / trail.size(); 
    //     
    //     Color trailColor = Fade(BROWN, alpha); 
    //
    //     DrawLine(
    //         (int)trail[i - 1].GetX(), (int)trail[i - 1].GetY(),
    //         (int)trail[i].GetX(), (int)trail[i].GetY(),
    //         trailColor
    //     );
    // }

    DrawCircle((int)this->pos.GetX(), (int)this->pos.GetY(), this->radius, BROWN);
}
