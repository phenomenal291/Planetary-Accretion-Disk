#pragma once
#include "Vector.h"
#include "particle.h"
#include "raylib.h"
#include <vector>
class Particle;


class OctTree{
public:
    class Oct{
    private:
        const int MAX_DEPTH = 8;
    public:

        Vector<float> center;
        Vector<float> size;
        Vector<float> center_mass;
        int depth;
        float total_mass;
        Particle* particle = nullptr;
        vector<Oct*> octs; // 8 children: T_NW, T_NE, T_SW, T_SE, B_NW, B_NE, B_SW, B_SE


        Oct();
        ~Oct();
        Oct(Vector<float> center, Vector<float> size, int depth);
        void selectOct( Particle* p);
        void insertToOct( Particle* p);

        //   ___________
        //  /         / |
        // /_________/  |
        // |         |  |
        // |         |  |
        // |         | /
        // |_________|/
        //   ___________
        //  /____/___ / |
        // /____/____/| |
        // |    |    ||/|
        // |____|____|/ |
        // |         | /
        // |_________|/
        //   ___________
        //  /         / |
        // /_________/  |
        // |         | /|
        // |_________|/||
        // |____|____|_|
        // |____|____|/



        bool contains(const Particle* p) const;
        bool hasParticle() const;
        bool divided() const;
        void subdivide();
        void clear();
        Oct* getNextNode();
    };
public:
    
    static vector<Oct> nodePool;
    static int nextNodeIndex;
    Oct* root;

    OctTree(Vector<float> center, Vector<float> size);
    ~OctTree();
    
    void buildTree(const std::vector<Particle>& particles);
    void insert(Particle* p);
    void insert(Oct* quad);
    void update_mass_helper(Oct* quad);
    void update_mass();
    void draw();
    void drawHelper(Oct* quad);
    void interactHelper(Oct* quad, Particle* p);
    void interact(Particle* particle);
    void clearHelper(Oct* quad);
    void clear();
};  
