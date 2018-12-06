#ifndef PARTICLESPAWNER_H
#define PARTICLESPAWNER_H

#include "particle.h"
#include "octree.h"
#include "collider.h"

class particleSpawner
{
public:
    ~particleSpawner();
    QVector<Particle*> particles;
    void init(QOpenGLShaderProgram *prog);
    void genParticle();
    void genBoundaryCollider(QVector<planeCollider> &p, float border);
    void updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss);
    void renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog);
    void updateParticles();

    QVector3D size = QVector3D(10, 3, 5);
    float spacing = 0.10f;

    Octree myOctree;

private:
    QVector<planeCollider> planes;
    QVector<triangleCollider> tris;
    QVector<sphereCollider> spheres;
    QOpenGLShaderProgram *program;

};

#endif // PARTICLESPAWNER_H
