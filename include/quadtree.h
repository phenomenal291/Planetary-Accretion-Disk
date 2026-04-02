#pragma once
#include "Vector.h"
#include "particle.h"
#include "raylib.h"
#include <vector>
class Particle;
class Quad{
private:
    const int MAX_DEPTH = 8;
public:

    Vector<float> center;
    Vector<float> size;
    Vector<float> center_mass;
    int depth;
    float total_mass;
    Particle* particle = nullptr;
    Quad* NW = nullptr;
    Quad* NE = nullptr;
    Quad* SW = nullptr;
    Quad* SE = nullptr;

    Quad();
    ~Quad();
    Quad(Vector<float> center, Vector<float> size, int depth);
    void selectQuad( Particle* p);
    void insertToQuad( Particle* p);
    // -----------
    // | NW | NE |
    // ----------
    // | SW | SE |
    // |---------

    bool contains(const Particle* p) const;
    bool hasParticle() const;
    bool divided() const;
    void subdivide();
};

class QuadTree{
public:
    
    Quad* root;
    QuadTree(Vector<float> center, Vector<float> size);
    ~QuadTree();
    
    void buildTree(const std::vector<Particle>& particles);
    void insert(Particle* p);
    void insert(Quad quad);
    void update_mass_helper(Quad* quad);
    void update_mass();
    void draw();
    void drawHelper(Quad* quad);
    void interactHelper(Quad* quad, Particle* p);
    void interact(Particle* particle);
    void clearHelper(Quad* quad);
    void clear();
};  
