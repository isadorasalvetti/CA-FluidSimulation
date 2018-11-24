#include "collider.h"

bool Collider::pointPlaneCollision(QVector3D p1, QVector3D p2, planeCollider plane){
    //test if a collision happened
    float p1Direction = QVector3D::dotProduct(plane.n,p1) + plane.d;
    float p2Direction = QVector3D::dotProduct(plane.n,p2) + plane.d;
    return p1Direction * p2Direction < 0;
}

bool Collider::pointTriCollision(QVector3D p1, QVector3D p2, triangleCollider tri){
    float p1Direction = QVector3D::dotProduct(tri.n,p1) + tri.d;
    float p2Direction = QVector3D::dotProduct(tri.n,p2) + tri.d;
    QVector3D pProj = p2 -  QVector3D::dotProduct(p2 - tri.p1, tri.n)*tri.n;
    if (p1Direction * p2Direction < 0){
        float a1 = triangleArea(tri.p1, tri.p2, pProj);
        float a2 = triangleArea(pProj, tri.p2, tri.p3);
        float a3 = triangleArea(tri.p1, pProj, tri.p3);
        float a = triangleArea(tri.p1, tri.p2, tri.p3);
        float tArea = a1 + a2 + a3 - a;
        return tArea < 0.01f;
    }
    return false;
}

bool Collider::pointSphereCollision(QVector3D p1, sphereCollider sphere){
    float dist = p1.distanceToPoint(sphere.c);
    return dist*dist <= sphere.r*sphere.r;
}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, sphereCollider sphere){
    //Compute contact point
    QVector3D normal = sphere.c - p2;
    normal.normalize();
    float d = -QVector3D::dotProduct(p2, normal);

    QVector3D pC = p2 - (1+sphere.b)*(QVector3D::dotProduct(normal,p2)+d)*normal;
    QVector3D vC = v2 - (1+sphere.b)*(QVector3D::dotProduct(normal,v2))*normal;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, planeCollider plane){
    QVector3D pC = p2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,p2)+plane.d)*plane.n;
    QVector3D vC = v2 - (1+plane.b)*(QVector3D::dotProduct(plane.n,v2))*plane.n;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}

std::pair<QVector3D, QVector3D> Collider::updateParticle(QVector3D p2, QVector3D v2, triangleCollider tri){
    QVector3D pC = p2 - (1+tri.b)*(QVector3D::dotProduct(tri.n,p2)+tri.d)*tri.n;
    QVector3D vC = v2 - (1+tri.b)*(QVector3D::dotProduct(tri.n,v2))*tri.n;

    std::pair<QVector3D, QVector3D> update(pC, vC);
    return update;
}

float Collider::triangleArea(QVector3D a, QVector3D b, QVector3D c){
    QVector3D ab = a - b;
    QVector3D ac = a - c;
    return QVector3D::crossProduct(ab, ac).length()/2.0f;
}
