#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "collider.h"

class particleSpawner
{
public:
    ~particleSpawner();
    QVector<Particle*> particles;
    void init(QOpenGLShaderProgram *prog, int dimension, float kd, float ke, Particle::SOLVER s);
    void genParticle();
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void updateParticles();

    Particle::SOLVER solver; //true = euler, false= verlet

    //Springs
    float kE; //elasticity
    float kD; //damping

    //Rope Simulation
    int lenght = 35;

    //Cloth simulation
    std::pair<int, int > size = std::pair<int, int> (60, 40);
    float spacing = 0.05f;
    
    static QVector<int> neightboors();

private:
    int dim = 0;

    QVector<planeCollider> planes;
    QVector<triangleCollider> tris;
    QVector<sphereCollider> spheres;
    QOpenGLShaderProgram *program;

};

#endif // PARTICLESPAWNER_H
