#ifndef OBJECTS_H
#define OBJECTS_H

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "collider.h"

class Obj {
public:
    //GL VAO n VBO
    QOpenGLVertexArrayObject* VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* normBuffer;
    float radius = -1; // -1 = triangle mesh/ > 1 = sphere
    QVector3D center;
    QVector3D color;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
    QMatrix4x4 modelMatrix;
};

class Objects {
public:
    std::vector<Obj> myObjs;
    QVector<triangleCollider> triangleColliders;
    QVector<sphereCollider> sphereColliders;
    Obj sphereMesh;
    Obj triangleMesh;

    bool Init(QOpenGLShaderProgram *program);
    void render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    bool LoadObj(const char *path);
    bool buildBuffers(QOpenGLShaderProgram *program, Obj *m);
    void addColision(QVector<triangleCollider> &vec, QVector<sphereCollider> &spc);

};

#endif // OBJECTS_H
