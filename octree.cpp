#include "octree.h"
#include <cmath>

void Octree::buildOctree(){

    /* Define octree size and expand vectors
    to fit octree and amount of particles */

    float sizeX = boundingBox.second.x() - boundingBox.first.x();
    float sizeY = boundingBox.second.y() - boundingBox.first.y();
    float sizeZ = boundingBox.second.z() - boundingBox.first.z();

    rezX = round(sizeX/voxelSize)+4;
    rezY = round(sizeY/voxelSize)+4;
    rezZ = round(sizeZ/voxelSize)+4;

    octreeToParticles.resize(rezX*rezY*rezZ);
    particleToLocation.resize(particleAmount);

}

// ************************

int Octree::locToIndex(int locx, int locy, int locz){
    return (locz * rezX * rezY) + (locy * rezX) + locx;
}

QVector3D Octree::intToLoc(int idx){
    int z = idx / (rezX * rezY);
    idx -= (z * rezX * rezY);
    int y = idx / rezX;
    int x = idx % rezX;
    return QVector3D(x,y,z);
}

//**************************

void Octree::addParticleToOctree(QVector3D &pos, int i){
    /* Places particle in octree voxels based on its position */
    int pX = round((pos.x() - boundingBox.first.x())/voxelSize);
    int pY = round((pos.y() - boundingBox.first.y())/voxelSize);
    int pZ = round((pos.z() - boundingBox.first.z())/voxelSize);

    int octreeIndex = locToIndex(pX, pY, pZ);
    octreeToParticles[octreeIndex].append(i);
    particleToLocation[i] = octreeIndex;
}

void Octree::resetOctree(){
    for (int i =0; i < octreeToParticles.size(); i++)
        octreeToParticles[i].clear();
}

bool Octree::validateLocation(QVector3D &pos, int &i){
    /* Verify if particle is the correct voxel, if not, reassign it. */
    int pX = static_cast<int>(pos.x()-boundingBox.first.x()/voxelSize);
    int pY = static_cast<int>(pos.y()-boundingBox.first.y()/voxelSize);
    int pZ = static_cast<int>(pos.z()-boundingBox.first.z()/voxelSize);

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

    //Z voxels
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++)
            for (int k = -1; k <= 1; k++){
                QVector3D pvj = QVector3D (pvi[0]+i, pvi[1]+j, pvi[2]+k);
                if (validateLoc(pvj)) surroundingVoxels.append(locToIndex(pvj[0], pvj[1], pvj[2]));
            }
    QVector<int> particlesInSVoxels; particlesInSVoxels.reserve(particleAmount);
    for (int i =0; i < surroundingVoxels.size(); i++){
        int v = surroundingVoxels[i];
        for (int p = 0; p < octreeToParticles[v].size(); p++){
            particlesInSVoxels.push_back(octreeToParticles[v][p]);
        }
    }
    return  particlesInSVoxels;
}


