#include "objects.h"
#include <math.h>

void buildNormals(Obj &m) {
  const int kFaces = m.triangles.size();
  std::vector<float> face_normals(kFaces, 0);

  for (int i = 0; i < kFaces; i += 3) {
    QVector3D v1(m.vertices[m.triangles[i] * 3], m.vertices[m.triangles[i] * 3 + 1],
                       m.vertices[m.triangles[i] * 3 + 2]);
    QVector3D v2(m.vertices[m.triangles[i + 1] * 3],
                       m.vertices[m.triangles[i + 1] * 3 + 1],
                       m.vertices[m.triangles[i + 1] * 3 + 2]);
    QVector3D v3(m.vertices[m.triangles[i + 2] * 3],
                       m.vertices[m.triangles[i + 2] * 3 + 1],
                       m.vertices[m.triangles[i + 2] * 3 + 2]);
    QVector3D v1v2 = v2 - v1;
    QVector3D v1v3 = v3 - v1;
    QVector3D normal = QVector3D::crossProduct(v1v2, v1v3);

    normal.normalize();
    for (int j = 0; j < 3; ++j) face_normals[i + j] = normal[j];
}
  const int kVertices = m.vertices.size();
  m.normals.resize(kVertices, 0);
  for (int i = 0; i < kFaces; i += 3) {
    for (int j = 0; j < 3; ++j) {
      int idx = m.triangles[i + j];
      QVector3D v1(m.vertices[m.triangles[i + j] * 3],
                         m.vertices[m.triangles[i + j] * 3 + 1],
                         m.vertices[m.triangles[i + j] * 3 + 2]);
      QVector3D v2(m.vertices[m.triangles[i + (j + 1) % 3] * 3],
                         m.vertices[m.triangles[i + (j + 1) % 3] * 3 + 1],
                         m.vertices[m.triangles[i + (j + 1) % 3] * 3 + 2]);
      QVector3D v3(m.vertices[m.triangles[i + (j + 2) % 3] * 3],
                         m.vertices[m.triangles[i + (j + 2) % 3] * 3 + 1],
                         m.vertices[m.triangles[i + (j + 2) % 3] * 3 + 2]);

      QVector3D v1v2 = v2 - v1;
      QVector3D v1v3 = v3 - v1;
      double angle = acos(QVector3D::dotProduct(v1v2, v1v3) / (v1v2.length() * v1v3.length()));

      for (int k = 0; k < 3; ++k) {
      m.normals[idx * 3 + k] += face_normals[i + k] * angle;
      }
    }
  }
}

bool Objects::Init(QOpenGLShaderProgram *program){

    //*************************************
    //Tri Mesh
    //*************************************

    float triVertices[] {
        .5, 0, 0,
        0, .5, 0,
        0, 0, .5,
        -.5, 0, 0,
        0, -.5, 0,
        0, 0, -.5
    };

    int faces[]{
        0, 1, 2,
        2, 1, 3,
        3, 1, 5,
        5, 1, 0,
//        2, 4, 0,
//        3, 4, 2,
//        5, 4, 3,
//        0, 4, 5,
    };


    float normal[]{
        1, 0, 0,
        0, 1, 0,
        0, 0, 1,
        -1, 0, 0,
        0, -1, 0,
        0, 0, -1
    };

    //Mesh translation
    triangleMesh.modelMatrix.translate(0, -1.4f, 0);
    triangleMesh.color = QVector3D (0.8f, 0.0f, 0.8f);

    for (int i=0; i<sizeof(triVertices)/sizeof(float); i+=3){

        //change vertex position
        QVector4D vert (triVertices[i], triVertices[i+1], triVertices[i+2], 1);
        vert = triangleMesh.modelMatrix * vert;

        //push back vertices and normals
        triangleMesh.vertices.push_back(vert[0]); triangleMesh.vertices.push_back(vert[1]); triangleMesh.vertices.push_back(vert[2]);
        triangleMesh.normals.push_back(normal[i]); triangleMesh.normals.push_back(normal[i+1]); triangleMesh.normals.push_back(normal[i+2]);
    }

    for (int i=0; i<sizeof(faces)/sizeof(int); i++){
        triangleMesh.triangles.push_back(faces[i]);
    }

    buildBuffers(program, &triangleMesh);

    //*************************************
    //Sphere mesh
    //*************************************
    sphereMesh.radius = .5f;

    const float X=.525731112119133606f;
    const float Z=.850650808352039932f;
    const float N=0.f;

    float sVerts[] {-X,N,Z,  X,N,Z,  -X,N,-Z,
            X,N,-Z,  N,Z,X,  N,Z,-X,
            N,-Z,X,  N,-Z,-X,  Z,X,N,
            -Z,X, N,  Z,-X,N,  -Z,-X, N};


     int triangles[] {1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
        1,10,8,  10,3,8,  8,3,5,  3,2,5,  3,7,2,
        3,10,7,  10,6,7,  6,11,7,  6,0,11,  6,1,0,
        10,1,6,  11,0,9,  2,11,9,  5,2,9,  11,2,7};

     //Mesh translation (-1.4f, -0.5f, -1.4f)
     sphereMesh.modelMatrix.translate(0,-0.2f,0);
     sphereMesh.modelMatrix.scale(sphereMesh.radius);

     sphereMesh.color = QVector3D (0.0f, 0.8f, 0.8f);
     sphereMesh.center = QVector3D (0,-0.2f,0);

     for (int i=0; i<sizeof(sVerts)/sizeof(float); i+=3){

         //change vertex position
         QVector4D vert (sVerts[i], sVerts[i+1], sVerts[i+2], 1);
         vert = sphereMesh.modelMatrix * vert;

         //push back vertices and normals
         sphereMesh.vertices.push_back(vert[0]); sphereMesh.vertices.push_back(vert[1]); sphereMesh.vertices.push_back(vert[2]);
     }

     for (int i=0; i<sizeof(triangles)/sizeof(int); i++){
         sphereMesh.triangles.push_back(triangles[i]);
     }
     buildNormals(sphereMesh);
     buildBuffers(program, &sphereMesh);

     addColision(triangleColliders, sphereColliders);

    return true;
    //Sphere
}

bool Objects::buildBuffers(QOpenGLShaderProgram *program, Obj *m){
    //My Buffers
    m->VAO = new QOpenGLVertexArrayObject;

    m->VAO->destroy();
    m->VAO->create();
    if (!m->VAO->isCreated()) return false;
    m->VAO->bind();

    m->coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    m->coordBuffer->destroy();
    m->coordBuffer->create();
    if (!m->coordBuffer->isCreated()) return false;
    if (!m->coordBuffer->bind()) return false;
    m->coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->coordBuffer->allocate(&m->vertices[0], sizeof(float) * m->vertices.size());

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    m->normBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);

    m->normBuffer->destroy();
    m->normBuffer->create();
    if (!m->normBuffer->isCreated()) return false;
    if (!m->normBuffer->bind()) return false;
    m->normBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->normBuffer->allocate(&m->normals[0], sizeof(float) * m->normals.size());

    program->enableAttributeArray(1);
    program->setAttributeBuffer(1, GL_FLOAT, 0, 3, 0);

    m->indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);

    m->indexBuffer->destroy();
    m->indexBuffer->create();
    if (!m->indexBuffer->isCreated()) return false;
    if (!m->indexBuffer->bind()) return false;
    m->indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    m->indexBuffer->allocate(&m->triangles[0], sizeof(int) * m->triangles.size());

    m->VAO->release();
    program->release();
    return true;
}

void Objects::render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program){   
    //Triangles
    program->setUniformValue("color", triangleMesh.color);
    program->setUniformValue("amICube", false);
    triangleMesh.VAO->bind();
    gl.glDrawElements(GL_TRIANGLES, triangleMesh.triangles.size(), GL_UNSIGNED_INT, nullptr);
    triangleMesh.VAO->release();

    //Sphere
    program->setUniformValue("color", sphereMesh.color);
    sphereMesh.VAO->bind();
    gl.glDrawElements(GL_TRIANGLES, sphereMesh.triangles.size(), GL_UNSIGNED_INT, nullptr);
    sphereMesh.VAO->release();
}

void Objects::addColision(QVector<triangleCollider> &vec, QVector<sphereCollider> &spc){
    Obj m = triangleMesh;
    Obj s = sphereMesh;

    for (unsigned int i=0; i+2<m.triangles.size(); i+= 3){
        QVector3D v1(m.vertices[m.triangles[i] * 3], m.vertices[m.triangles[i] * 3 + 1],
                           m.vertices[m.triangles[i] * 3 + 2]);
        QVector3D v2(m.vertices[m.triangles[i + 1] * 3],
                           m.vertices[m.triangles[i + 1] * 3 + 1],
                           m.vertices[m.triangles[i + 1] * 3 + 2]);
        QVector3D v3(m.vertices[m.triangles[i + 2] * 3],
                           m.vertices[m.triangles[i + 2] * 3 + 1],
                           m.vertices[m.triangles[i + 2] * 3 + 2]);
        QVector3D v1v2 = v2 - v1;
        QVector3D v1v3 = v3 - v1;

        //construct plane
        QVector3D normal = QVector3D::crossProduct(v1v2, v1v3).normalized();
        float d = - QVector3D::dotProduct(normal, v1);

        //pass triangle coordinates
        QVector3D tri0 (m.vertices[m.triangles[i]*3], m.vertices[m.triangles[i]*3+1], m.vertices[m.triangles[i]*3+2]);
        QVector3D tri1 (m.vertices[m.triangles[i+1]*3], m.vertices[m.triangles[i+1]*3+1], m.vertices[m.triangles[i+1]*3+2]);
        QVector3D tri2 (m.vertices[m.triangles[i+2]*3], m.vertices[m.triangles[i+2]*3+1], m.vertices[m.triangles[i+2]*3+2]);
        triangleCollider pl(normal, tri0, tri1, tri2, d, 1);
        vec.push_back(pl);
    }

    sphereCollider sColl(sphereMesh.center, sphereMesh.radius, 1);
    spc.push_back(sColl);

}
