#ifndef PLARTICLE_H
#define PLARTICLE_H

#include <QVector3D>
#include <iostream>
#include <fstream>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>
#include <QTimer>

#include "collider.h"
#include "octree.h"

class Particle{
public:
    Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity,
             QOpenGLShaderProgram *prog, float mass);
    Particle(){}
    ~Particle();

    enum ROLE {
        STRETCH,
        SHEER,
        BEND
    };

    bool lp = false; //last position initialized
    QVector3D m_LastPosition;
    QVector3D m_Position; // Center point of particle
    QVector3D m_Velocity; // Current particle velocity
    QVector3D m_Color;    // Particle color
    QVector3D m_Force;
    float m_Dnst;
    float m_Mass;
    float m_Radius; //size of the particle

    //Neighborhood
    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    void forceUpdate(QVector<Particle*> &particles, int &i, Octree &myOctree);
    void densityUpdate(QVector<Particle*> &particles, int &i);
    void collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres);
    void positionUpdate();

    QVector<int> neighborhoodForFabric(Particle::ROLE role, int &i, std::pair<int, int> &size);
    void fixClothSpacing(QVector<Particle*> &particles, int &i, std::pair<int, int> size, ROLE role);

private:
    bool BuildPlane(QOpenGLShaderProgram *program);
    void fixPrticleSpacing(QVector<Particle*> &particles, int &i);

    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
};

#endif // PLARTICLE_H
