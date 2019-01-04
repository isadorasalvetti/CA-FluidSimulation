#ifndef OCTREE_H
#define OCTREE_H

#include <QVector>
#include <QVector3D>

class Octree
{
public:
    float voxelSize = .1f;
    float particleAmount;
    std::pair<QVector3D, QVector3D> boundingBox;

    //store octree particles.
    QVector<QVector<int>> octreeToParticles; //for each cell, store the current particles
    QVector<int> particleToLocation; //for each particle, store the current cell

    Octree(QVector3D sBB, QVector3D bBB, int a):
        particleAmount(a),
        boundingBox(std::pair<QVector3D, QVector3D>(sBB, bBB)){}
    Octree(){}

    void buildOctree();
    void resetOctree();
    void addParticleToOctree(QVector3D &pos, int i);
    bool validateLocation(QVector3D &pos, int &i);
    bool validateLoc(QVector3D &pos);

    QVector<int> getNeighboorhoodCandidates (int i);

private:
    int locToIndex(int locx, int locy, int locz);
    QVector3D intToLoc(int index);

    int rezX, rezY, rezZ;

};

#endif // OCTREE_H
