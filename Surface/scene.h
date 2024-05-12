#ifndef SCENE_H
#define SCENE_H

#include "surface.h"
#include <QMatrix4x4>
#include <QVector2D>
#include <QPainter>

/*
Класс Scene представляет собой сцену, подконтрольную наблюдателю
*/

class Scene {
private:
    Surface surface;

    qreal product(const QVector4D &row, const QVector4D &col) const;                                                    // Метод, который вычисляет скалярное произведение двух 4-мерных векторов

    QMatrix4x4 getTransitionMatrixFromCSWToCSO(const QVector3D &coordObserver) const;                                   // Метод, который вычисляет матрицу преобразования из мировой системы координат (CSW) в систему координат наблюдателя (CSO)

    QVector<QVector<QVector3D>>& transitionFromCSWToCSO(const QVector3D &coordObserver) const;                          // Метод, который выполняет преобразование всех сечений из CSW в CSO
    QVector<QVector<QVector2D>>& transitionFromCSOToCSP(const QVector<QVector<QVector3D>> &sectionsInCSO) const;        // Метод, который выполняет преобразование всех сечений из CSO в проекцию на плоскость (CSP)

    QVector2D& getHalfSizeInCSP(const QVector<QVector<QVector2D>> &sectionsInCSP) const;                                // Метод, который вычисляет половину размера проекции на плоскость (CSP)

    void transitionFromCSPToCSS(QVector<QVector<QVector2D>> &sectionsInCSP, QVector2D &halfSizeInCSP, const QVector2D &center, const QVector2D &halfSizeInCSS) const;   // Метод, который выполняет преобразование всех сечений из CSP в систему координат экрана (CSS)

public:
    Scene();                                                                                                            // Конструктор

    void drawFor(const QVector3D& coordObserver, QPainter &painter, qint32 width,  qint32 height) const;                // Рисование сцены на основе координат наблюдателя на указанном виджете
};

#endif // SCENE_H
