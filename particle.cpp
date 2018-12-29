#include "particle.h"
#include <QOpenGLFunctions>
#include <math.h>

// Force and Position update
//****************************************************
const float PI = 3.1415;
const QVector3D G(0, -9.8f, 0);
const float myH = 0.5;

//Pressure constants
const float refDnst = 1000; //Reference density

//Viscosity constants
float u = 0.001f;


// Neighboorhood
//****************************************************
QVector<float> getParticleNeighboorhood(QVector<Particle*> &particles, int &i){
    float radius = 1;
    QVector<float> returnMe;
    for (int j = 0; j<particles.size(); j++){
        float len = (particles[i]->m_Position - particles[j]->m_Position).length();
        if (i != j && len < radius)
            returnMe.push_back(len);
    }
    return returnMe;
}

float kernelFunction(float r, const float &h){
    return (315/(64*PI*pow(h,9))) * pow((h*h-r*r), 3);
}

float kernelFunction1(float &r, const float &h){
    return (-945/(32*PI*pow(h,9))) * pow((h*h-r*r), 2) * r;
}

float kernelFunction2(float &r, const float &h){
    return (945/(8*PI*pow(h,9))) * pow((h*h-r*r), 2) * (r*r - (3/4)*(h*h-r*r)) * r;
}

void Particle::updateNighborhoodIndices(Octree &oct, int &i){
    particleNeighboorsIndex = oct.getNeighboorhoodCandidates(i);
}

void Particle::densityUpdate(QVector<Particle*> &particles, int &i){
    const float Cs = 1-(10*(m_Mass/100)); //Speed of sound
    QVector<float> neighboorRadius;
    //Compute density
    m_Dnst = 0;
    for (int i = 0; i < particleNeighboorsIndex.size(); i++){
        int p = particleNeighboorsIndex[i];
        QVector3D r = (particles[p]->m_Position - m_Position);
        float rLen = r.length();
        m_Dnst += kernelFunction(rLen, myH); //change density
    }
    //Also update pressure
    m_Prs = Cs*Cs*(m_Dnst - refDnst);
}

void Particle::forceUpdate(QVector<Particle*> &particles, int &i, Octree &myOctree){
    QVector3D aPressure (0, 0, 0);
    QVector3D aViscosity (0, 0, 0);
    for (int i = 0; i < particleNeighboorsIndex.size(); i++){
        int p = particleNeighboorsIndex[i];
        Particle *pj = particles[p];
        QVector3D r = (pj->m_Position - m_Position);
        float rLen = r.length();
        if (rLen < myH){
            aPressure += -m_Mass * ((m_Dnst/(m_Prs*m_Prs)) + (pj->m_Dnst/(pj->m_Prs*pj->m_Prs)))
                                 * kernelFunction(rLen, myH) * r;
            aViscosity += u * m_Mass * ((pj->m_Velocity-m_Velocity)/(pj->m_Dnst * m_Dnst))
                                     * kernelFunction(rLen, myH);
        }
    }
    m_Acceleration = aPressure + aViscosity + G*m_Mass;
}

void Particle::positionUpdate(){
    float  elapsedTime = .03f; //fixed timestep
    QVector3D lastPosition = m_Position;

    /* SOLVERS START HERE */
    if (!lp){
        m_Position += elapsedTime * m_Velocity + 0.5*m_Acceleration*elapsedTime*elapsedTime;
        lp = true;
    } else {
        m_Velocity = (m_Position - m_LastPosition) /elapsedTime;
        m_Position += (m_Position - m_LastPosition)+m_Acceleration*elapsedTime*elapsedTime;
    }
    m_LastPosition = lastPosition;
}

void Particle::collsionCheck(QVector<planeCollider> &planes, QVector<triangleCollider> &triangles, QVector<sphereCollider> &spheres){
    /*COLLISION CHECKS START HERE */
    //planes
    for (int i = 0; i<planes.size(); i++){
        bool check = Collider::pointPlaneCollision(m_LastPosition, m_Position, planes[i]);
        if (check) {
            lp = false;
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_Position, m_Velocity, planes[i]);
            m_Position = nD.first; m_Velocity = nD.second;
        }
    }

    //triangles
    for (int i = 0; i<triangles.size(); i++){
        bool check = Collider::pointTriCollision(m_LastPosition, m_Position, triangles[i]);
        if (check) {
            lp = false;
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_Position, m_Velocity, triangles[i]);
            m_Position = nD.first; m_Velocity = nD.second;
        }
    }

    //sphere
    for (int i = 0; i<spheres.size(); i++){
        bool check = Collider::pointSphereCollision(m_Position, spheres[i]);
        if (check) {
            lp = false;
            std::pair<QVector3D, QVector3D> nD = Collider::updateParticle(m_LastPosition, m_Velocity, spheres[i]);
            m_Position = nD.first; m_Velocity = nD.second;
        }
    }

}


/* CREATING THE PARTICLE/ BUFFERS */
// Plane
//****************************************************

bool Particle::BuildPlane(QOpenGLShaderProgram *program){
    GLfloat vertices[] = {-0.5f, -0.5f, 0, 0.5f, -0.5f, 0, 0.5f, 0.5f, 0, -0.5f, 0.5f, 0};
    GLuint faces[] = {0, 1, 3, 1, 2, 3};

    //adjust vertices by size/ position
    for (unsigned int i = 0; i < sizeof(vertices)/sizeof(GLfloat); i++){
        vertices[i] *= m_Radius;
    }

    program->bind();

    //My Buffers
    VAO = new QOpenGLVertexArrayObject;
    VAO->destroy();
    VAO->create();
    if (!VAO->isCreated()) return false;
    VAO->bind();

    coordBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    coordBuffer->destroy();
    coordBuffer->create();
    if (!coordBuffer->isCreated()) return false;
    if (!coordBuffer->bind()) return false;
    coordBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    coordBuffer->allocate(&vertices[0], sizeof(vertices));

    program->enableAttributeArray(0);
    program->setAttributeBuffer(0, GL_FLOAT, 0, 3, 0);

    indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indexBuffer->destroy();
    indexBuffer->create();
    if (!indexBuffer->isCreated()) return false;
    if (!indexBuffer->bind()) return false;
    indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
    indexBuffer->allocate(&faces[0], sizeof(faces));

    VAO->release();

    program->release();
    return true;
}

void Particle::Render(QOpenGLFunctions &gl, QOpenGLShaderProgram *program){
    QMatrix4x4 modelMatrix;
    modelMatrix.translate(m_Position);

    VAO->bind();
    program->setUniformValue("color", m_Color);
    program->setUniformValue("factor", m_Radius);
    program->setUniformValue("model", modelMatrix);
    gl.glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    VAO->release();
}

//****************************************************

Particle::Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog, float mass){
    m_Position = position;
    m_Velocity = velocity;
    m_Position = position;
    m_Radius = radius;
    m_Color = color;
    m_Mass = mass;

    if(!BuildPlane(prog)){
        std::cout << "Could not create particle" << std::endl;
    };
}


/* DESTROY PARTICLE*/
Particle::~Particle(){
    VAO->destroy();
    coordBuffer->destroy();
    indexBuffer->destroy();
}
