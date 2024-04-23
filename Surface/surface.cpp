#include "surface.h"
#include <QtMath>

Surface::Surface() {}

qreal Surface::f(qreal x, qreal z) const {
    return qExp(-qSqrt(x * x + z * z)) - 0.5;
}

QVector<QVector<QVector3D>>& Surface::getSections() const {
    qreal z {minZ};
    qreal x {};

    qint32 sizeZ {static_cast<qint32>((maxZ - minZ) / stepZ) + 1};
    qint32 sizeX {static_cast<qint32>((maxX - minX) / stepX) + 1};

    QVector<QVector<QVector3D>> *sections = new QVector<QVector<QVector3D>>(sizeZ);

    for (qint32 i {0}; i < sizeZ; i++, z += stepZ) {
        (*sections)[i] = QVector<QVector3D>(sizeX);
        x = minX;

        for (qint32 j {0}; j < sizeX; j++, x += stepX) {
            (*sections)[i][j] = QVector3D(x, f(x, z), z);
        }
    }

    return *sections;
}
