#include "../include/quadtree.h"        
              
Quad::Quad() {
    this->center = Vector<float>(0.0f, 0.0f, 0.0f);
    this->size = Vector<float>(0.0f, 0.0f, 0.0f);
    this->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    this->depth = 0;
    this->total_mass = 0.0f;
}

Quad::Quad(Vector<float> center, Vector<float> size, int depth) {
    this->center = center;
    this->size = size;
    this->depth = depth;
    this->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    this->total_mass = 0.0f;
}

Quad::~Quad() {
    delete NW;
    delete NE;
    delete SW;
    delete SE;
}

bool Quad::contains(const Particle* p) const {
    return (p->pos.GetX() >= this->center.GetX() - size.GetX()) && p->pos.GetX() <= center.GetX() + size.GetX() &&
            p->pos.GetY() >= center.GetY() - size.GetY() && p->pos.GetY() <= center.GetY() + size.GetY();
}

bool Quad::hasParticle() const {
    return this->particle != nullptr;
}

void Quad::subdivide(){
    
    Vector<float> new_size = this->size / 2.0f;
    float x = this->center.GetX();
    float y = this->center.GetY();
    
    NW = new Quad(Vector<float>(x - new_size.GetX(), y - new_size.GetY(), 0.0f), new_size, this->depth + 1);
    NE = new Quad(Vector<float>(x + new_size.GetX(), y - new_size.GetY(), 0.0f), new_size, this->depth + 1);
    SW = new Quad(Vector<float>(x - new_size.GetX(), y + new_size.GetY(), 0.0f), new_size, this->depth + 1);
    SE = new Quad(Vector<float>(x + new_size.GetX(), y + new_size.GetY(), 0.0f), new_size, this->depth + 1);

}

bool Quad::divided() const {
    return NW != nullptr;
}

void Quad::selectQuad(Particle* p){
    if ( NW->contains(p)) {
        NW->insertToQuad(p);
    }
    else if ( NE->contains(p)) {
        NE->insertToQuad(p);
    }
    else if ( SW->contains(p)) {
        SW->insertToQuad(p);
    }
    else if ( SE->contains(p)) {
        SE->insertToQuad(p);
    }
}
void Quad::insertToQuad(Particle* p){

    if ( this->divided()){
        this->selectQuad(p);
    }

    // not divided and has particle, need to subdivide
    else if ( this->hasParticle()){
        
        // if exceed max depth, slightly move the particle
        if ( this->depth >= MAX_DEPTH) {
            p->pos += Vector<float>(0.01f, 0.01f, 0.0f); 
        }
        this->subdivide();

        selectQuad(this->particle);
        selectQuad(p);

        this->particle = nullptr;
    }
    else {
        this->particle = p;
    }
}




QuadTree::QuadTree(Vector<float> center, Vector<float> size) {
    this->root = new Quad();
    this->root->center = center;
    this->root->size = size;
    this->root->depth = 0;
}

QuadTree::~QuadTree() {
    delete root;
}

void QuadTree::buildTree(const std::vector<Particle>& particles) {
    for (auto p : particles) {
        root->insertToQuad(&p);
    }
}

void QuadTree::insert(Particle* p) {
    this->root->insertToQuad(p);
}

void QuadTree::update_mass_helper(Quad* quad){

    if (quad == nullptr) return;
        
    // leave node
    if ( !quad->divided()){
        if (quad->hasParticle()){
            quad->total_mass = quad->particle->mass;
            quad->center_mass = quad->particle->pos;
        }
        else{
            quad->total_mass = 0.0f;
            quad->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
        }
        return;
    }
    
    update_mass_helper(quad->NW);
    update_mass_helper(quad->NE);
    update_mass_helper(quad->SW);
    update_mass_helper(quad->SE);

    quad->total_mass = quad->NW->total_mass + quad->NE->total_mass + quad->SW->total_mass + quad->SE->total_mass;
   
    if (quad->total_mass > 0.0f) {
        quad->center_mass = (quad->NW->center_mass * quad->NW->total_mass + 
                             quad->NE->center_mass * quad->NE->total_mass + 
                             quad->SW->center_mass * quad->SW->total_mass + 
                             quad->SE->center_mass * quad->SE->total_mass) / quad->total_mass;
    } else {
        quad->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    }
}

void QuadTree::update_mass() {
    update_mass_helper(this->root);   
}

void QuadTree::drawHelper(Quad* quad){
    if (quad == nullptr) return;
    
    DrawRectangleLines(quad->center.GetX() - quad->size.GetX(), quad->center.GetY() - quad->size.GetY(), quad->size.GetX() * 2.0f, quad->size.GetY() * 2.0f, GRAY);
    
    // DrawCircle(quad->center.GetX(), quad->center.GetY(), 10.0f, WHITE);
    drawHelper(quad->NW);
    drawHelper(quad->NE);
    drawHelper(quad->SW);
    drawHelper(quad->SE);
}
void QuadTree::draw(){
    drawHelper(this->root);
}

void QuadTree::interactHelper(Quad* quad, Particle* p){
    
    if ( quad == nullptr || quad->total_mass == 0.0f)
        return;
    
    // leaf node
    if ( !quad->divided()){
        if ( quad->hasParticle() && p != quad->particle){
            p->interact(quad->particle);
        }
        return;
    }

    // s/d < THETA  =>  s^2 < THETA^2 * d^2
    float s = quad->size.GetX() * 2.0f;
    Vector<float> dd = p->pos - quad->center_mass;
    float d2 = dd.GetX() * dd.GetX() + dd.GetY() * dd.GetY() + dd.GetZ() * dd.GetZ();

    static float THETA = 0.5f;
    static float THETA2 = THETA * THETA;

    if ( s * s < THETA2 * d2){
        if ( d2 < 1.0f) d2 = 1.0f;
        float r = std::sqrt(d2);
        float force_magnitude = (G * p->mass * quad->total_mass) / (d2 * r);

        Vector<float> force_vector = dd * force_magnitude;
        p->applyForce(force_vector * -1.0f);                                   
    }    
    else{
        interactHelper(quad->NE, p);
        interactHelper(quad->NW, p);
        interactHelper(quad->SE, p);
        interactHelper(quad->SW, p);
    }
}

void QuadTree::interact(Particle* partcile){
    
    interactHelper(this->root, partcile);
}

void QuadTree::clearHelper(Quad* quad){
    if (quad == nullptr) return;

    clearHelper(quad->NW);
    clearHelper(quad->NE);
    clearHelper(quad->SW);
    clearHelper(quad->SE);

    delete quad->NW;
    delete quad->NE;
    delete quad->SW;
    delete quad->SE;

    quad->NW = nullptr;
    quad->NE = nullptr;
    quad->SW = nullptr;
    quad->SE = nullptr;

}

void QuadTree::clear(){
    clearHelper(this->root);
}
