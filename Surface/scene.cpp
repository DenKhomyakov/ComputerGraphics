#include "scene.h"
#include <QtMath>

Scene::Scene() {}

qreal Scene::product(const QVector4D& row, const QVector4D& col) const {
    qreal res {0};

    for (int i {0}; i < 4; i++) {
        res += row[i] * col[i];
    }

    return res;
}

QMatrix4x4 Scene::getTransitionMatrixFromCSWToCSO(const QVector3D &coordObserver) const {
    QMatrix4x4 shift(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -coordObserver.x(), -coordObserver.y(), -coordObserver.z(), 1
        );

    QMatrix4x4 rightToLeft(
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    QMatrix4x4 rotationYUp(
        1, 0, 0, 0,
        0, 0, -1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
        );

    QMatrix4x4 rotationAroundY(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    qreal sin {};
    qreal cos {};

    qreal distanceZ {qSqrt(coordObserver.x() * coordObserver.x() + coordObserver.y() * coordObserver.y())};

    if (distanceZ != 0) {
        sin = coordObserver.x() / distanceZ;
        cos = coordObserver.y() / distanceZ;

        rotationAroundY(0, 0) = cos;
        rotationAroundY(0, 2) = sin;
        rotationAroundY(2, 0) = -sin;
        rotationAroundY(2, 2) = cos;
    }

    QMatrix4x4 rotationAroundX(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    qreal distanceCentre {qSqrt(distanceZ * distanceZ + coordObserver.z() * coordObserver.z())};

    if (distanceCentre != 0) {
        sin = coordObserver.z() / distanceCentre;
        cos = distanceZ / distanceCentre;

        rotationAroundX(1, 1) = cos;
        rotationAroundX(1, 2) = -sin;
        rotationAroundX(2, 1) = sin;
        rotationAroundX(2, 2) = cos;
    }

    return shift * rightToLeft * rotationYUp * rotationAroundY * rotationAroundX;
}

QVector<QVector<QVector3D>>& Scene::transitionFromCSWToCSO(const QVector3D &coordObserver) const {
    QVector<QVector<QVector3D>> sectionsInCSW(surface.getSections());

    QMatrix4x4 transitionMatrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    qreal dis {qSqrt(coordObserver.y() * coordObserver.y() + coordObserver.z() * coordObserver.z())};

    if (dis != 0) {
        qreal sin {coordObserver.y() / dis};
        qreal cos {coordObserver.z() / dis};

        transitionMatrix(1, 1) = cos;
        transitionMatrix(1, 2) = -sin;
        transitionMatrix(2, 1) = sin;
        transitionMatrix(2, 2) = cos;
    }

    QVector<QVector<QVector3D>> *sectionsInCSO = new QVector<QVector<QVector3D>>(sectionsInCSW.length());

    for (qint32 i {0}; i < sectionsInCSW.length(); i++) {
        (*sectionsInCSO)[i] = QVector<QVector3D>(sectionsInCSW[i].length());

        for (qint32 j {0}; j < sectionsInCSW[i].length(); j++) {
            (*sectionsInCSO)[i][j] = QVector3D(
                product(QVector4D(sectionsInCSW[i][j], 1), transitionMatrix.column(0)),
                product(QVector4D(sectionsInCSW[i][j], 1), transitionMatrix.column(1)),
                product(QVector4D(sectionsInCSW[i][j], 1), transitionMatrix.column(2))
                );
        }
    }

    return *sectionsInCSO;
}

QVector<QVector<QVector2D>>& Scene::transitionFromCSOToCSP(const QVector<QVector<QVector3D>> &sectionsInCSO) const {
    QVector<QVector<QVector2D>> *sectionsInCSP = new QVector<QVector<QVector2D>>(sectionsInCSO.length());

    for (qint32 i {0}; i < sectionsInCSO.length(); i++) {
        (*sectionsInCSP)[i] = QVector<QVector2D>(sectionsInCSO[0].length());

        for (qint32 j {0}; j < sectionsInCSO[0].length(); j++) {
            (*sectionsInCSP)[i][j] = QVector2D(sectionsInCSO[i][j].x(), sectionsInCSO[i][j].y());
        }
    }

    delete &sectionsInCSO;
    return *sectionsInCSP;
}

QVector2D& Scene::getHalfSizeInCSP(const QVector<QVector<QVector2D>> &sectionsInCSP) const {
    QVector2D *halfSizeInCSP = new QVector2D(abs(sectionsInCSP[0][0].x()), abs(sectionsInCSP[0][0].y()));

    for (const QVector<QVector2D> &section : sectionsInCSP)
        for (const QVector2D &point : section) {
            if (abs(point.x()) > halfSizeInCSP->x()) {
                halfSizeInCSP->setX(abs(point.x()));
            }
            if (abs(point.y()) > halfSizeInCSP->y()) {
                halfSizeInCSP->setY(abs(point.y()));
            }
        }

    return *halfSizeInCSP;
}

void Scene::transitionFromCSPToCSS(QVector<QVector<QVector2D>> &sectionsInCSP, QVector2D &halfSizeInCSP, const QVector2D &center, const QVector2D &halfSizeInCSS) const {
    qreal max = halfSizeInCSP.x() > halfSizeInCSP.y() ? halfSizeInCSP.x() : halfSizeInCSP.y();

    for (QVector<QVector2D> &section : sectionsInCSP) {
        for (QVector2D &point : section) {
            point.setX(point.x() * halfSizeInCSS.x() / max + center.x());
            point.setY(point.y() * halfSizeInCSS.y() / max + center.y());
            point.setY(2 * center.y() - point.y());
        }
    }

    delete &halfSizeInCSP;
}

void Scene::drawFor(const QVector3D& coordObserver, QPainter &painter, qint32 width,  qint32 height) const {
    QVector<QVector<QVector2D>> sectionsInCSP(transitionFromCSOToCSP(transitionFromCSWToCSO(coordObserver)));
    transitionFromCSPToCSS(sectionsInCSP, getHalfSizeInCSP(sectionsInCSP), QVector2D(width / 2, height / 2), QVector2D(width / 2, height / 2));

    QVector<qreal> horizonUp(sectionsInCSP[0].length());
    for (qreal &pointY : horizonUp) {
        pointY = -1;
    }

    QVector<qreal> horizonDown(sectionsInCSP[0].length());
    for (qreal &pointY : horizonDown) {
        pointY = height + 1;
    }

    QPen red(Qt::red, 1);
    QPen green(Qt::green, 1);

    for (qint32 i {0}; i < sectionsInCSP.length(); i++) {
        for (qint32 j {0}; j < sectionsInCSP[i].length(); j++) {
            if (sectionsInCSP[i][j].y() < horizonDown[j]) {
                horizonDown[j] = sectionsInCSP[i][j].y();

                if (i != 0) {
                    painter.setPen(red);
                }

                painter.drawPoint(sectionsInCSP[i][j].toPointF());
            }
            if (sectionsInCSP[i][j].y() > horizonUp[j]) {
                horizonUp[j] = sectionsInCSP[i][j].y();

                if (i != 0) {
                    painter.setPen(green);
                }

                painter.drawPoint(sectionsInCSP[i][j].toPointF());
            }
        }
    }
}
