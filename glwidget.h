#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include "mesh.h"
#include "particlespawner.h"
#include "collider.h"
#include "objects.h"
#include "timer.h"

QT_BEGIN_NAMESPACE

class QOpenGLBuffer;
class QOpenGLVertexArrayObject;

QT_END_NAMESPACE

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    GLWidget(QWidget *parent = nullptr);
    ~GLWidget();

    //Scene management
    //colliders
    QVector<planeCollider> planeColliders;
    QVector<triangleCollider> triColliders;
    QVector<sphereCollider> sphereColliders;

    void Reset(int dim, float kd, float ke, Particle::SOLVER s);

    //Default Spawner Values
    Particle::SOLVER solver = Particle::VERLET;
    int dim = 1;
    float kE = 8.0f;
    float kD = 0.0f;

protected:
    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    void setProjection(float aspect);
    void setModelview();
    //QMatrix4x4 projectionMatrix;
    //QMatrix4x4 modelviewMatrix;
    float angleX, angleY, distance;
    QPoint lastMousePos;

    QOpenGLShaderProgram *program;
    QOpenGLShaderProgram *program_particle;
    Mesh mesh;
    Objects objectColliders;
    particleSpawner spawner;

    Timer *timer;
};

#endif
