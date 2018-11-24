#include "particlespawner.h"

const float max = 0.2;

void particleSpawner::init(QOpenGLShaderProgram *prog, int dimension, float kd, float ke, Particle::SOLVER s){
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    program = prog;
    kD = kd;
    kE = ke;
    solver = s;
    dim = dimension; //dimention of the simulation.
    /* 0 - Spawn individual particles
     * 1 - Create Rope
     * 2 - Create fabric
    */
    genParticle();
}

void particleSpawner::updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss){
    planes = p;
    tris = ts;
    spheres = ss;
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
}

void particleSpawner::genParticle(){
    float radius = .05f;
    QVector3D color = QVector3D(.8f, .8f, 0);
    QVector3D velocity = QVector3D(0, 0, 0);

    if (dim == 0){
        for (unsigned int i = 0; i< 30; i++){
            float x = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
            float y = .7f;
            float z = max - 2 * max * static_cast<float>(rand())/static_cast<float>(RAND_MAX);
            QVector3D position = QVector3D(x, y, z);
            Particle *p = new Particle(position, radius, color, velocity, program, spacing);
            particles.push_back(p);
        }
    }

    //Make a 1 dim spring system (rope)
    else if (dim == 1){
        float x = 0.1; float y = 0.7;
        for (unsigned int i = 0; i < lenght; i++){
            QVector3D fcolor = ((float)i/(float)lenght)*color;
            float z = i*spacing;
            QVector3D position = QVector3D(x, y, z);
            Particle *p = new Particle(position, radius, fcolor, velocity, program, spacing);
            particles.push_back(p);
        }
    }

    //Make a 2 dim spring system (cloth)
    else if (dim == 2){
        //Spawn regular cloth mesh
        float y = .7f;
        for (unsigned int i = 0; i < size.first; i++){
            float x = -size.first*spacing/2 + i*spacing;
            for (unsigned int j = 0; j < size.second; j++){
                float z = -size.second*spacing/2 + j*spacing;
                QVector3D position = QVector3D(x, y, z);
                QVector3D fcolor = QVector3D( (float)i/(size.first-1), (float) j/(size.second-1), 1.0);
                Particle *p = new Particle(position, radius, fcolor, velocity, program, spacing);
                particles.push_back(p);
            }
        }
    }
}

void particleSpawner::updateParticles(){
    //Update particles positions
    for(int i = 0; i<particles.size(); i++){
        particles[i]->forceUpdate(particles, i, dim, size, kD, kE);
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->positionUpdate(solver, particles, i, dim, size, kD);
    }

    if (dim == 2) {for(int i = 0; i<particles.size()/2; i++){
            int i1 = particles.size()/2 + i;
            int i2 = particles.size()/2-i-1;
            particles[i1]->fixClothSpacing(particles, i1, size, Particle::STRETCH);
            particles[i2]->fixClothSpacing(particles, i2, size, Particle::STRETCH);
        }
        for(int i = 0; i<particles.size()/2; i++){
            int i1 = particles.size()/2 + i;
            int i2 = particles.size()/2-i-1;
            particles[i1]->fixClothSpacing(particles, i1, size, Particle::SHEER);
            particles[i2]->fixClothSpacing(particles, i2, size, Particle::SHEER);
        }}

    for(int i = 0; i<particles.size(); i++){
        particles[i]->collsionCheck(planes, tris, spheres);
    }

}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
