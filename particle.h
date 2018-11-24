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

class Particle{
public:
    Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog, float s);
    Particle(){}
    ~Particle();

    enum ROLE {
        STRETCH,
        SHEER,
        BEND
    };

    enum SOLVER {
        VERLET,
        EULER
    };

    bool lp = false; //last position initialized
    QVector3D m_LastPosition;
    QVector3D m_Position; // Center point of particle
    QVector3D m_Velocity; // Current particle velocity
    QVector3D m_Color;    // Particle color
    QVector3D m_Force;
    float m_Radius; //size of the particle

    //Spring Vars
    float sSpring;

    QVector3D p1Force;
    QVector3D p2Force; //neighboor to p2Forces


    //Neighborhood

    void Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    void forceUpdate(QVector<Particle*> &particles, int &i, int &dim, std::pair<int, int> &size, float &kE, float &kD);
    void collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres);
    void positionUpdate(SOLVER &solver, QVector<Particle*> &particles, int &i, int &dim, std::pair<int, int> &size, float kD);

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
