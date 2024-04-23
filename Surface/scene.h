#ifndef SCENE_H
#define SCENE_H

#include "surface.h"
#include <QMatrix4x4>
#include <QVector2D>
#include <QPainter>

class Scene {
private:
    Surface surface;

    qreal product(const QVector4D &row, const QVector4D &col) const;

    QMatrix4x4 getTransitionMatrixFromCSWToCSO(const QVector3D &coordObserver) const;

    QVector<QVector<QVector3D>>& transitionFromCSWToCSO(const QVector3D &coordObserver) const;
    QVector<QVector<QVector2D>>& transitionFromCSOToCSP(const QVector<QVector<QVector3D>> &sectionsInCSO) const;

    QVector2D& getHalfSizeInCSP(const QVector<QVector<QVector2D>> &sectionsInCSP) const;

    void transitionFromCSPToCSS(QVector<QVector<QVector2D>> &sectionsInCSP, QVector2D &halfSizeInCSP, const QVector2D &center, const QVector2D &halfSizeInCSS) const;

public:
    Scene();

    void drawFor(const QVector3D& coordObserver, QPainter &painter, qint32 width,  qint32 height) const;
};

#endif // SCENE_H
