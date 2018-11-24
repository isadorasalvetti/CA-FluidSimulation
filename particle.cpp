#include "particle.h"
#include <QOpenGLFunctions>
#include <math.h>

// Particle simulation
//****************************************************
const QVector3D G(0, -3.8f, 0);
const QVector3D G1(0, -9.8f, 0);

void Particle::forceUpdate(QVector<Particle*> &particles, int &i, int &dim, std::pair<int, int> &size, float &kD, float &kE){
    bool springs = true;
    m_Force = QVector3D(0, 0, 0);

    /* SPRING MESHES*/

    if (dim == 0) m_Force = G1;
    else if (dim == 1){

        /* ROPE FORCE CALCULATION STARTS HERE */
        if (i+1 < particles.size()){
            QVector3D q = particles[i+1]->m_Position - m_Position;
            float velDot = QVector3D::dotProduct(particles[i+1]->m_Velocity - m_Velocity, q.normalized());

            p1Force = ((kE)*(q.length()-sSpring) + kD*velDot) * q.normalized();
            p2Force = - p1Force;
        }
        if (i != 0) {
            m_Force = G1 + p1Force + particles[i-1]->p2Force;
        }
    } else if (dim == 2){

        /* CLOTH FORCE CALCULATION STARTS HERE */
        /* SPRINGS */
        float sprSize = sSpring; //initial distance between particle - spring size
        float sprSizeD = sqrt((sSpring*sSpring)*2); //diagonal spring size
        float sprSize2 = sSpring*2; //diagonal spring size

        m_Force = G;
        if (springs){
            QVector <int> nhd = neighborhoodForFabric(Particle::STRETCH, i, size);
            for (int j = 0; j < nhd.size(); j++) {
                QVector3D q = particles[nhd[j]]->m_Position - m_Position;
                float separatingVelocity = QVector3D::dotProduct(particles[nhd[j]]->m_Velocity - m_Velocity, q.normalized());
                m_Force += (kE*10*(q.length()-sprSize) + kD*separatingVelocity) * q.normalized();
            }

            nhd = neighborhoodForFabric(Particle::SHEER, i, size);
            for (int j = 0; j < nhd.size(); j++) {
                QVector3D q = particles[nhd[j]]->m_Position - m_Position;
                float separatingVelocity = QVector3D::dotProduct(particles[nhd[j]]->m_Velocity - m_Velocity, q.normalized());
                m_Force += (kE*10*(q.length()-sprSizeD) + kD*separatingVelocity) * q.normalized();
            }

            nhd = neighborhoodForFabric(Particle::BEND, i, size);
            for (int j = 0; j < nhd.size(); j++) {
                QVector3D q = particles[nhd[j]]->m_Position - m_Position;
                float separatingVelocity = QVector3D::dotProduct(particles[nhd[j]]->m_Velocity - m_Velocity, q.normalized());
                m_Force += (kE*10*(q.length()-sprSize2) + kD*separatingVelocity) * q.normalized();
            }
       }
    }
}

void Particle::positionUpdate(Particle::SOLVER &solver, QVector<Particle*> &particles, int &i, int &dim, std::pair<int, int> &size, float kD){
    float  elapsedTime = .03f; //fixed timestep
    QVector3D lastPosition = m_Position;

        /* SOLVERS START HERE */
    if (solver == Particle::EULER){
        m_Velocity += m_Force * elapsedTime;
        m_Position += elapsedTime * m_Velocity;
    }
    else if (solver == Particle::VERLET){
        if (!lp){
            m_Position += elapsedTime * m_Velocity + 0.5*m_Force*elapsedTime*elapsedTime;
            lp = true;
        } else {
            m_Velocity = (m_Position - m_LastPosition) /elapsedTime;
            m_Position += kD*(m_Position - m_LastPosition)+m_Force*elapsedTime*elapsedTime;
        }
    }
    m_LastPosition = lastPosition;

    /* RESTRICTION FIXING */
    if (dim == 1) fixPrticleSpacing(particles, i);
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

void Particle::fixPrticleSpacing(QVector<Particle*> &particles, int &i){
    float mod = 1.8f;
    if (i > 0){
        QVector3D p2p1 = particles[i-1]->m_Position - m_Position;
        if (p2p1.length() > sSpring * mod){ //if distance between particles is bigger than initil distance
            m_Position = particles[i-1]->m_Position - p2p1.normalized() * sSpring * mod;
        }
    }
}

void Particle::fixClothSpacing(QVector<Particle*> &particles, int &i, std::pair<int, int> size, Particle::ROLE role){
    float mod = 1.3f;
    float sz;
    if(role == Particle::STRETCH) sz = sSpring;
    if(role == Particle::SHEER) sz = sqrt((sSpring*sSpring)*2);

    QVector<int> ngt = neighborhoodForFabric(role, i, size);
    for (int j = 0; j<ngt.size(); j++){
        QVector3D p1p2 = particles[ngt[j]]->m_Position - m_Position;
        if (p1p2.length() > sz * mod){ //if distance between particles is bigger than initil distance
        QVector3D midPoint = m_Position + p1p2/2;
        particles[ngt[j]]->m_Position = midPoint + p1p2.normalized() * sz * mod/2;
        m_Position = midPoint - p1p2.normalized() * sz * mod/2;
        }
    }

}


std::pair<int,int> index2grid(int i, std::pair<int,int> &size) {
    return std::make_pair(i/size.second, i%size.second);
}

int grid2index(std::pair<int,int> &p, std::pair<int,int> &size) {
    return p.second + p.first * size.second;
}

bool isInGrid(std::pair<int,int> &p, std::pair<int,int> &size) {
    return p.first >= 0 and p.second >= 0 and
           p.first < size.first and p.second < size.second;
}

typedef std::pair<int,int> ii;

QVector<int> Particle::neighborhoodForFabric(Particle::ROLE role, int &i, std::pair<int, int> &size){
    QVector<int> neighbors;

    ii p = index2grid(i, size);
    if (role == Particle::ROLE::STRETCH) {
        ii p2 = p;
        p2.first += 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.first -= 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.second -= 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.second += 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));
    }

    else if (role == Particle::ROLE::SHEER) {
        ii p2 = p;
        p2.first += 1;
        p2.second += 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.first -= 1;
        p2.second += 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.first += 1;
        p2.second -= 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.first -= 1;
        p2.second -= 1;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));
    }

    else if (role == Particle::ROLE::BEND) {
        ii p2 = p;
        p2.first += 2;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.first -= 2;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.second += 2;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));

        p2 = p;
        p2.second -= 2;
        if (isInGrid(p2, size)) neighbors.append(grid2index(p2, size));
    }

    return neighbors;
}

//****************************************************

Particle::Particle(QVector3D position, float radius, QVector3D color, QVector3D velocity, QOpenGLShaderProgram *prog, float s){
    m_Position = position;
    m_Velocity = velocity;
    m_Position = position;
    m_Radius = radius;
    m_Color = color;

    sSpring = s;

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
