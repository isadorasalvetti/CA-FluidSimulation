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

//****************************

QVector<int> Octree::getNeighboorhoodCandidates(int i){
    int vi = particleToLocation[i];
    QVector<int> surroundingVoxels;

    //Z voxels
    if (vi >= 1) surroundingVoxels.append(vi - 1);
    surroundingVoxels.append(vi + 1);

    //Y voxels
    surroundingVoxels.append(vi + rezZ);
    surroundingVoxels.append(vi - rezZ);

    //X voxels
    surroundingVoxels.append(vi + rezZ*rezY);
    surroundingVoxels.append(vi - rezZ*rezY);

}


