#include "../include/octree.h"        

std::vector<OctTree::Oct> OctTree::nodePool;
int OctTree::nextNodeIndex = 0;
              
OctTree::Oct::Oct() {
    this->center = Vector<float>(0.0f, 0.0f, 0.0f);
    this->size = Vector<float>(0.0f, 0.0f, 0.0f);
    this->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    this->depth = 0;
    this->total_mass = 0.0f;
}

OctTree::Oct::Oct(Vector<float> center, Vector<float> size, int depth) {
    this->center = center;
    this->size = size;
    this->depth = depth;
    this->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    this->total_mass = 0.0f;
}


OctTree::Oct::~Oct() {
    // nodePool handles deletion
}

bool OctTree::Oct::contains(const Particle* p) const {
    return (p->pos.GetX() >= this->center.GetX() - size.GetX()) && 
            p->pos.GetX() <= center.GetX() + size.GetX() &&
            p->pos.GetY() >= center.GetY() - size.GetY() && 
            p->pos.GetY() <= center.GetY() + size.GetY() &&
            p->pos.GetZ() >= center.GetZ() - size.GetZ() &&
            p->pos.GetZ() <= center.GetZ() + size.GetZ();
}

bool OctTree::Oct::hasParticle() const {
    return this->particle != nullptr;
}

void OctTree::Oct::subdivide(){
    
    Vector<float> new_size = this->size / 2.0f;

    float x = this->center.GetX();
    float y = this->center.GetY();
    float z = this->center.GetZ();
 
    octs.resize(8); 
    for ( int i = 0; i < 8; i++){
        octs[i] = getNextNode();
        octs[i]->size = new_size;
        octs[i]->depth = this->depth + 1;
    }
    octs[0]->center = Vector<float>(x - new_size.GetX(), y - new_size.GetY(), z + new_size.GetZ());
    octs[1]->center = Vector<float>(x + new_size.GetX(), y - new_size.GetY(), z + new_size.GetZ());
    octs[2]->center = Vector<float>(x - new_size.GetX(), y + new_size.GetY(), z + new_size.GetZ());
    octs[3]->center = Vector<float>(x + new_size.GetX(), y + new_size.GetY(), z + new_size.GetZ());
    octs[4]->center = Vector<float>(x - new_size.GetX(), y - new_size.GetY(), z - new_size.GetZ());
    octs[5]->center = Vector<float>(x + new_size.GetX(), y - new_size.GetY(), z - new_size.GetZ());
    octs[6]->center = Vector<float>(x - new_size.GetX(), y + new_size.GetY(), z - new_size.GetZ());
    octs[7]->center = Vector<float>(x + new_size.GetX(), y + new_size.GetY(), z - new_size.GetZ());

}

bool OctTree::Oct::divided() const {
    return octs.size() ;
}

void OctTree::Oct::selectOct(Particle* p){

    for ( Oct* oct : octs){
        if ( oct->contains(p)){
            oct->insertToOct(p);
            return;
        }
    }
}
void OctTree::Oct::insertToOct(Particle* p){

    if ( this->divided()){
        this->selectOct(p);
    }

    // not divided and has particle, need to subdivide
    else if ( this->hasParticle()){
        
        if ( this->depth >= MAX_DEPTH) {
            p->pos += Vector<float>(0.01f, 0.01f, 0.0f); 
        }
        this->subdivide();

        selectOct(this->particle);
        selectOct(p);

        this->particle = nullptr;
    }
    else {
        this->particle = p;
    }
}

void OctTree::Oct::clear(){
    this->particle = nullptr;
    this->total_mass = 0.0f;
    this->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    octs.clear();
}


OctTree::OctTree(Vector<float> center, Vector<float> size) {
    this->root = new Oct();
    this->root->center = center;
    this->root->size = size;
    this->root->depth = 0;
    nodePool.resize(50000);
    nextNodeIndex = 0;
}

OctTree::~OctTree() {
    delete root;
}

void OctTree::buildTree(const std::vector<Particle>& particles) {
    for (auto p : particles) {
        root->insertToOct(&p);
    }
}

void OctTree::insert(Particle* p) {
    this->root->insertToOct(p);
}

void OctTree::update_mass_helper(Oct* oct){

    if (oct == nullptr) return;
        
    // leave node
    if ( !oct->divided()){
        if (oct->hasParticle()){
            oct->total_mass = oct->particle->mass;
            oct->center_mass = oct->particle->pos;
        }
        else{
            oct->total_mass = 0.0f;
            oct->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
        }
        return;
    }
    
    for ( Oct* child: oct->octs){
        update_mass_helper(child);
        oct->total_mass += child->total_mass;
        oct->center_mass += child->center_mass * child->total_mass;
    }
    if (oct->total_mass > 0.0f) {
        oct->center_mass = oct->center_mass / oct->total_mass;
    } else {
        oct->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    }
}

void OctTree::update_mass() {
    update_mass_helper(this->root);   
}

void OctTree::drawHelper(Oct* oct){
    if (oct == nullptr) return;
    
    DrawRectangleLines(oct->center.GetX() - oct->size.GetX(), oct->center.GetY() - oct->size.GetY(), oct->size.GetX() * 2.0f, oct->size.GetY() * 2.0f, GRAY);
    
    // DrawCircle(oct->center.GetX(), oct->center.GetY(), 10.0f, WHITE);
    
    for ( Oct* child : oct->octs){
        drawHelper(child);
    }
}
void OctTree::draw(){
    drawHelper(this->root);
}

void OctTree::interactHelper(Oct* oct, Particle* p){

    if ( oct == nullptr || oct->total_mass == 0.0f)
        return;

    // leaf node
    if ( !oct->divided()){
        if ( oct->hasParticle() && p != oct->particle){
            p->interact(oct->particle);
        }
        return;
    }

    // s/d < THETA  =>  s^2 < THETA^2 * d^2
    float s = oct->size.GetX() * 2.0f;
    Vector<float> dd = p->pos - oct->center_mass;
    float d2 = dd.GetX() * dd.GetX() + dd.GetY() * dd.GetY() + dd.GetZ() * dd.GetZ();

    static float THETA = 0.5f;
    static float THETA2 = THETA * THETA;

    if ( s * s < THETA2 * d2){
        if ( d2 < 1.0f) d2 = 1.0f;
        float r = std::sqrt(d2);
        float force_magnitude = (G * p->mass * oct->total_mass) / (d2 * r);

        Vector<float> force_vector = dd * force_magnitude;
        p->applyForce(force_vector * -1.0f);                                   
    }    
    else{
        for ( Oct* child : oct->octs){
            interactHelper(child, p);
        }
    }
}

void OctTree::interact(Particle* partcile){
    
    interactHelper(this->root, partcile);
}

void OctTree::clearHelper(Oct* oct){
    if (oct == nullptr) return;

    for ( Oct* child : oct->octs){
        clearHelper(child);
    }
    oct->particle = nullptr;
    oct->total_mass = 0.0f;
    oct->center_mass = Vector<float>(0.0f, 0.0f, 0.0f);
    
}

void OctTree::clear(){
    clearHelper(this->root);
}

OctTree::Oct* OctTree::Oct::getNextNode(){
    if ( (size_t)nextNodeIndex >= nodePool.size()){
        return nullptr;
    }
    Oct* oct = &nodePool[nextNodeIndex++];
    oct->clear();
    return oct;
}
