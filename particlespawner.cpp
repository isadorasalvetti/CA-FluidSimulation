#include "particlespawner.h"

void particleSpawner::init(QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    float border = 0.5;
    genBoundaryCollider(planes, border);

    myOctree = Octree(
            QVector3D(-size[0]*spacing/2 - border, -size[1]*spacing/2 - border, -size[2]*spacing/2 - border),
            QVector3D(size[0]*spacing/2 + border, size[1]*spacing/2 + border, size[2]*spacing/2 + border),
            size[0]*size[1]*size[2]
            );
    myOctree.buildOctree();

    program = prog;
    genParticle();
}
void particleSpawner::updateColliders(QVector<planeCollider> &p, QVector<triangleCollider> &ts, QVector<sphereCollider> &ss){
    planes = p;
    tris = ts;
    spheres = ss;
}

void particleSpawner::genBoundaryCollider(QVector<planeCollider> &p, float border){
/* Generate colider around the spawned particles*/
   QVector3D min (-size[0]*spacing/2 - border, -size[1]*spacing/2 - border, -size[2]*spacing/2 - border);
   QVector3D max (size[0]*spacing/2 + border, size[1]*spacing/2 + border, size[2]*spacing/2 + border);

   //left, right
   QVector3D normal = QVector3D(-1, 0, 0);
   float d = min.x();
   p.append(planeCollider (normal, d, 0.15f));
   p.append(planeCollider (-normal, -d, 0.15f));

   //back, front
   normal = QVector3D(0, 0, -1);
   d = min.z();
   p.append(planeCollider (normal, d, 0.15f));
   p.append(planeCollider (-normal, -d, -0.15f));

   //bottom, top
   normal = QVector3D(0,-1,0);
   d = min.y();
   p.append(planeCollider (normal, d, 0.15f));
   p.append(planeCollider (-normal, -d, 0.15f));
}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
}

void particleSpawner::genParticle(){
    float radius = .05f;
    QVector3D velocity = QVector3D(0, 0, 0);
    float mass = 0.01f;
    for (unsigned int i = 0; i < size[0]; i++){
        float x = -size[0]*spacing/2 + i*spacing;
        for (unsigned int j = 0; j < size[1]; j++){
            float y = -size[1]*spacing/2 + j*spacing;
            for (unsigned int k = 0; k< size[2]; k++){
                float z = -size[2]*spacing/2 + k*spacing;
                QVector3D position = QVector3D(x, y, z);
                QVector3D fcolor = QVector3D((float)i/(size[0]-1), (float) j/(size[1]-1), (float)k/(size[0]-1));
                Particle *p = new Particle(position, radius, fcolor, velocity, program, mass);

                myOctree.addParticleToOctree(position, i*size[2]*size[1] + j*size[2] + k);
                particles.push_back(p);
            }
        }
    }
}

void particleSpawner::updateParticles(){    
    for(int i = 0; i<particles.size(); i++){
        particles[i]->updateNighborhoodIndices(myOctree, i);
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->densityUpdate(particles, i);
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->forceUpdate(particles, i, myOctree);
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->positionUpdate();
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->collsionCheck(planes, tris, spheres);
    }

    myOctree.resetOctree();
    for(int i = 0; i<particles.size(); i++){
        myOctree.addParticleToOctree(particles[i]->m_Position, i);
    }
}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
