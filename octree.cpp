#include "octree.h"
#include <cmath>

void Octree::buildOctree(){

    /* Define octree size and expand vectors
    to fit octree and amount of particles */

    float sizeX = boundingBox.second.x() - boundingBox.first.x();
    float sizeY = boundingBox.second.y() - boundingBox.first.y();
    float sizeZ = boundingBox.second.z() - boundingBox.first.z();

    rezX = static_cast<int>(roundf(sizeX/voxelSize));
    rezY = static_cast<int>(roundf(sizeY/voxelSize));
    rezZ = static_cast<int>(roundf(sizeZ/voxelSize));

    octreeToParticles.resize(rezX*rezY*rezZ);
    particleToLocation.resize(particleAmount);

}

// ************************

int Octree::locToIndex(int locx, int locy, int locz){
    return locx*rezZ*rezY + locy*rezZ + locz;
}

QVector3D Octree::intToLoc(int index){
    return QVector3D(0,0,0);
}

//**************************

void Octree::addParticleToOctree(QVector3D &pos, int i){
    /* Places particle in octree voxels based on its position */
    int pX = static_cast<int>((pos.x() - boundingBox.first.x())/voxelSize);
    int pY = static_cast<int>((pos.y() - boundingBox.first.y())/voxelSize);
    int pZ = static_cast<int>((pos.z() - boundingBox.first.z())/voxelSize);

    int octreeIndex = locToIndex(pX, pY, pZ);
    octreeToParticles[octreeIndex].append(i);
    particleToLocation[i] = octreeIndex;
}

bool Octree::validateLocation(QVector3D &pos, int &i){
    /* Verify if particle is the correct voxel, if not, reassign it. */
    int pX = static_cast<int>(pos.x()/voxelSize);
    int pY = static_cast<int>(pos.y()/voxelSize);
    int pZ = static_cast<int>(pos.z()/voxelSize);

    int newLocation = locToIndex(pX, pY, pZ);
    int currentLoc = particleToLocation[i];

    if (newLocation == currentLoc) return true;

    // Remove particle from old voxel and reassign to correct one.
    octreeToParticles[currentLoc].remove(i);
    octreeToParticles[newLocation].append(i);
    particleToLocation[i] = newLocation;

    return false;
}

bool Octree::validateLoc(QVector3D &pos){
    if (pos.x() < rezX and pos.y() < rezY and pos.z() < rezZ and
        pos.x() >= 0 and pos.y() >= 0 and pos.z() >= 0)
    return true;
    else return false;
}

//****************************

QVector<int> Octree::getNeighboorhoodCandidates(int i){
    int vi = particleToLocation[i];
    QVector3D pvi = intToLoc(vi);
    QVector<int> surroundingVoxels;

    QVector3D p;

    //Z voxels
    QVector3D pvj = QVector3D (pvi[0], pvi[1], pvi[2]+1);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));
    pvj = QVector3D (pvi[0], pvi[1], pvi[2]-1);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));

    //Y voxels
    pvj = QVector3D (pvi[0], pvi[1]+1, pvi[2]);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));
    pvj = QVector3D (pvi[0], pvi[1]-1, pvi[2]);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));

    //X voxels
    pvj = QVector3D (pvi[0]+1, pvi[1], pvi[2]);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));
    pvj = QVector3D (pvi[0]-1, pvi[1], pvi[2]);
    if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));

    QVector<int> particlesInSVoxels; particlesInSVoxels.reserve(particleAmount);
    for (int i =0; i < surroundingVoxels.size(); i++){
        int v = surroundingVoxels[i];
        for (int p = 0; p < octreeToParticles[v].size(); p++){
            particlesInSVoxels.push_back(octreeToParticles[v][p]);
        }
    }
    return  particlesInSVoxels;
}


