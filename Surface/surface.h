#ifndef SURFACE_H
#define SURFACE_H

#include <QVector>
#include <QVector3D>

class Surface {
private:
    qreal minX {-1}, maxX {1};
    qreal minZ {-1}, maxZ {1};
    qreal stepX {0.01}, stepZ {0.05};
    qreal f(qreal x, qreal z) const;

public:
    Surface();

    QVector<QVector<QVector3D>>& getSections() const;
};

#endif // SURFACE_H
