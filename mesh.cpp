#include "mesh.h"
#include "collider.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <QOpenGLFunctions>
#include <QDebug>

//renders ONLY the surrounding cube

void Mesh::addVertex(float v0, float v1, float v2)
{
    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);
}

void Mesh::addTriangle(int v0, int v1, int v2)
{
    triangles.push_back(v0);
    triangles.push_back(v1);
    triangles.push_back(v2);
}

void Mesh::buildCube()
{
    GLfloat vertices[] = {-1, -.5, -1,
                          1, -.5, -1,
                          1,  .5, -1,
                         -1,  .5, -1,
                         -1, -.5,  1,
                          1, -.5,  1,
                          1,  .5,  1,
                         -1,  .5,  1
                        };

    GLuint faces[] = {0, 1, 3, //z pos
                      1, 2, 3,
                    7, 6, 5, //z neg
                      7, 5, 4,
                    0, 3, 7, //x pos
                      7, 4, 0,
                    6, 2, 1, //x neg
                      1, 5, 6,
                    4, 1, 0, //y pos
                      1, 4, 5,
                    7, 3, 2, //y neg
                      2, 6, 7
                  };

    int i;
    float cubeSize = 2.8f;

    for(i=0; i<8; i+=1)
        addVertex(cubeSize * vertices[3*i], cubeSize * vertices[3*i+1], cubeSize * vertices[3*i+2]);
    for(i=0; i<12; i++)
        addTriangle(faces[3*i], faces[3*i+1], faces[3*i+2]);
}

void Mesh::buildNormals() {
  const int kFaces = triangles.size();
  std::vector<float> face_normals(kFaces, 0);

  for (int i = 0; i < kFaces; i += 3) {
    QVector3D v1(vertices[triangles[i] * 3], vertices[triangles[i] * 3 + 1],
                       vertices[triangles[i] * 3 + 2]);
    QVector3D v2(vertices[triangles[i + 1] * 3],
                       vertices[triangles[i + 1] * 3 + 1],
                       vertices[triangles[i + 1] * 3 + 2]);
    QVector3D v3(vertices[triangles[i + 2] * 3],
                       vertices[triangles[i + 2] * 3 + 1],
                       vertices[triangles[i + 2] * 3 + 2]);
    QVector3D v1v2 = v2 - v1;
    QVector3D v1v3 = v3 - v1;
    QVector3D normal = QVector3D::crossProduct(v1v2, v1v3);

    normal.normalize();
    for (int j = 0; j < 3; ++j) face_normals[i + j] = normal[j];
}
  const int kVertices = vertices.size();
  normals.resize(kVertices, 0);
  for (int i = 0; i < kFaces; i += 3) {
    for (int j = 0; j < 3; ++j) {
      int idx = triangles[i + j];
      QVector3D v1(vertices[triangles[i + j] * 3],
                         vertices[triangles[i + j] * 3 + 1],
                         vertices[triangles[i + j] * 3 + 2]);
      QVector3D v2(vertices[triangles[i + (j + 1) % 3] * 3],
                         vertices[triangles[i + (j + 1) % 3] * 3 + 1],
                         vertices[triangles[i + (j + 1) % 3] * 3 + 2]);
      QVector3D v3(vertices[triangles[i + (j + 2) % 3] * 3],
                         vertices[triangles[i + (j + 2) % 3] * 3 + 1],
                         vertices[triangles[i + (j + 2) % 3] * 3 + 2]);

      QVector3D v1v2 = v2 - v1;
      QVector3D v1v3 = v3 - v1;
      double angle = acos(QVector3D::dotProduct(v1v2, v1v3) / (v1v2.length() * v1v3.length()));

      for (int k = 0; k < 3; ++k) {
      normals[idx * 3 + k] += face_normals[i + k] * angle;
      }
    }
  }
}

bool Mesh::init(QOpenGLShaderProgram *program)
{
    //Create the mesh and normals
    buildCube();
    buildNormals();

    program->bind();

    //My Uniforms
    color = QVector3D (0.8f, 0.8f, 0.8f);

    //My Buffers
    VAO.destroy();
    VAO.create();
    if (!VAO.isCreated()) return false;
    VAO.bind();

    coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    coordBuffer->destroy();
    coordBuffer->create();
    if (!coordBuffer->isCreated()) return false;
    if (!coordBuffer->bind()) return false;
    coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    coordBuffer->allocate(&vertices[0], sizeof(float) * vertices.size());

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    normBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    normBuffer->destroy();
    normBuffer->create();
    if (!normBuffer->isCreated()) return false;
    if (!normBuffer->bind()) return false;
    normBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    normBuffer->allocate(&normals[0], sizeof(float) * normals.size());

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    indexBuffer->destroy();
    indexBuffer->create();
    if (!indexBuffer->isCreated()) return false;
    if (!indexBuffer->bind()) return false;
    indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer->allocate(&triangles[0], sizeof(int) * triangles.size());

    VAO.release();
    program->release();
    return true;
}

void Mesh::render(QOpenGLFunctions &gl,QOpenGLShaderProgram *program)
{
    program->setUniformValue("color", color);
    program->setUniformValue("amICube", true);
    VAO.bind();
    gl.glDrawElements(GL_TRIANGLES, triangles.size(), GL_UNSIGNED_INT, nullptr);
    VAO.release();
}

// Collision
//**************************************

void Mesh::addColision(QVector<planeCollider> &vec){
    for (unsigned int i=0; i+2<triangles.size(); i+= 6){
        QVector3D v1(vertices[triangles[i] * 3], vertices[triangles[i] * 3 + 1],
                           vertices[triangles[i] * 3 + 2]);
        QVector3D v2(vertices[triangles[i + 1] * 3],
                           vertices[triangles[i + 1] * 3 + 1],
                           vertices[triangles[i + 1] * 3 + 2]);
        QVector3D v3(vertices[triangles[i + 2] * 3],
                           vertices[triangles[i + 2] * 3 + 1],
                           vertices[triangles[i + 2] * 3 + 2]);
        QVector3D v1v2 = v2 - v1;
        QVector3D v1v3 = v3 - v1;
        QVector3D normal = QVector3D::crossProduct(v1v2, v1v3).normalized();
        float d = -QVector3D::dotProduct(normal, v1);
        planeCollider pl(normal, d, 0.15f);
        vec.push_back(pl);
    }
}
