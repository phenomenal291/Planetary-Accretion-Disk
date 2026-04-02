#pragma once
#include "Vector.h"
#include <vector>
using namespace std;
const float G = 0.1f; // Gravitational constant
class Particle{
public:
    Vector<float> pos;
    Vector<float> velocity;
    Vector<float> acceleration;
    vector<Vector<float>> trail;
    int maxTrailLength = 5;

    float mass;
    float radius;
    bool isActive = true;
    bool drawLine = false;
    Particle(Vector<float> pos, Vector<float> v, Vector<float> a, float m = 2.0f);
    void updateRadius();
    void applyForce(const Vector<float>& force);
    void interact( Particle* other);
    void update(float dt);
    void updateVelocity(float dt);
    void draw();
};
