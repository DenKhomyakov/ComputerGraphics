#ifndef SURFACE_H
#define SURFACE_H

#include <QVector>
#include <QVector3D>

/*
Класс Surface представляет поверхность, которая будет отображаться на экране
*/

class Surface {
private:
    qreal minX {-1};                                        // Размеры области, в которой будет строиться поверхность
    qreal maxX {1};
    qreal minZ {-1};
    qreal maxZ {1};

    qreal stepX {0.01};                                     // Шаг вдоль оси X
    qreal stepZ {0.05};                                     // Шаг вдоль оси Z

    qreal f(qreal x, qreal z) const;                        // Метод, который вычисляет значение поверхности в заданной точке (x, z)

public:
    Surface();                                              // Конструктор

    QVector<QVector<QVector3D>>& getSections() const;       // Метод возвращает вектор сечений поверхности
};

#endif // SURFACE_H
