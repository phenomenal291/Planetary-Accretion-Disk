class Mover{
public:
    float position[2];
    float velocity[2];
    float acceleration[2];
    float mass = 1.0f;
    Mover(float pos[], float v[], float a[], float mass = 1.0f);
    void applyForce(float force[]);
    void update();
    void draw();
};


