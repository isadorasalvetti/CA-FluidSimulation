#include "particlespawner.h"

const float max = 0.2;

void particleSpawner::init(QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++) delete particles[i];
    particles.clear();

    //generate new ones
    float border = 0.5f;
    genBoundaryCollider(planes, border);

    myOctree = Octree(
            QVector3D(-size[0]*spacing/2 + border, -size[1]*spacing/2 + border, -size[2]*spacing/2 + border),
            QVector3D(size[0]*spacing/2 + border, size[1]*spacing/2 + border, size[2]*spacing/2 + border),
            size[0]*size[1]*size[2]
            );
    myOctree.buildOctree();

    genBoundaryCollider(planes, 0.1);

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
   QVector3D min (-size[0]*spacing/2 + border, -size[1]*spacing/2 + border, -size[2]*spacing/2 + border);
   QVector3D max (size[0]*spacing/2 + border, size[1]*spacing/2 + border, size[2]*spacing/2 + border);

   QVector3D p2 = QVector3D(min[0], max[1], max[2]); //left, top, front
   QVector3D p3 = QVector3D(min[0], min[1], max[2]); //left, bottom, front
   QVector3D p4 = QVector3D(min[0], max[1], min[2]); //left, top, back
   //QVector3D p5 = QVector3D(max[0], min[1], min[2]); //right, bottom, back
   //QVector3D p6 = QVector3D(max[0], max[1], min[2]); //right, top, back
   //QVector3D p7 = QVector3D(max[0], min[1], max[2]); //right, bottom, front

   //left
   QVector3D normal = (p2-max).normalized();
   float d = -QVector3D::dotProduct(normal, min);
   p.append(planeCollider (normal, d, 0.15f));

   //back
   normal = (p4-p2).normalized();
   d = -QVector3D::dotProduct(normal, min);
   p.append(planeCollider (normal, d, 0.15f));

   //bottom
   normal = (p3-p2).normalized();
   d = -QVector3D::dotProduct(normal, min);
   p.append(planeCollider (normal, d, 0.15f));

   //right
   normal = (p2-max).normalized();
   d = -QVector3D::dotProduct(normal, max);
   p.append(planeCollider (normal, d, 0.15f));

   //front
   normal = (p4-p2).normalized();
   d = -QVector3D::dotProduct(normal, max);
   p.append(planeCollider (normal, d, 0.15f));

   //top
   normal = (p3-p2).normalized();
   d = -QVector3D::dotProduct(normal, max);
   p.append(planeCollider (normal, d, 0.15f));



}

void particleSpawner::renderParticles(QOpenGLFunctions &gl, QOpenGLShaderProgram *prog){
    for(int i = 0; i<particles.size(); i++){
        particles[i]->Render(gl, prog);
    }
}

void particleSpawner::genParticle(){
    float radius = .05f;
    QVector3D velocity = QVector3D(0, 0, 0);
    float mass = 1/(size[0] * size[1] * size[2]);
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
    //Update particles positions
    for(int i = 0; i<particles.size(); i++){
        particles[i]->forceUpdate(particles, i, myOctree);
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->positionUpdate();
    }
    for(int i = 0; i<particles.size(); i++){
        particles[i]->collsionCheck(planes, tris, spheres);
    }

}

particleSpawner::~particleSpawner(){
    //Delete old particles
    for(int i = 0; i<particles.size(); i++) delete particles[i];
}
