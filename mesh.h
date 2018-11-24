#ifndef MESH_H
#define MESH_H

#include <vector>
#include <QOpenGLFunctions>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "collider.h"

class Mesh {
public:
    bool init(QOpenGLShaderProgram *program);

    //GL VAO n VBO
    QOpenGLVertexArrayObject VAO;
    QOpenGLBuffer* coordBuffer;
    QOpenGLBuffer* indexBuffer;
    QOpenGLBuffer* normBuffer;

    QMatrix4x4 modelMatrix;
    QVector3D color;

    void render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program);
    void addColision(QVector<planeCollider> &vec);
    QVector3D correctPosition();

private:
    void addVertex(float v0, float v1, float v2);
    void addTriangle(int v0, int v1, int v2);
    void buildCube();
    void buildNormals();

    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<int> triangles;
};

#endif // MESH_H
