#include "surface.h"
#include <QtMath>

// Конструктор
Surface::Surface() {}

// Метод возвращает значение функции для заданных значений x и z
qreal Surface::f(qreal x, qreal z) const {
    //return qExp(-qSqrt(x * x + z * z)) - 0.5;
    return z * z * z + x * z;
}

// Метод возвращает вектор сечений поверхности
QVector<QVector<QVector3D>>& Surface::getSections() const {
    // Вычисляются размеры сетки по осям X и Z в зависимости от шага сетки и диапазона значений
    qreal z {minZ};
    qreal x {};

    qint32 sizeZ {static_cast<qint32>((maxZ - minZ) / stepZ) + 1};
    qint32 sizeX {static_cast<qint32>((maxX - minX) / stepX) + 1};

    // Вектор сечений
    QVector<QVector<QVector3D>> *sections = new QVector<QVector<QVector3D>>(sizeZ);

    // Цикл по всем сечениям
    for (qint32 i {0}; i < sizeZ; i++, z += stepZ) {
        // Для каждого сечения вычисляется набор точек на поверхности, расположенных на одной и той же высоте
        (*sections)[i] = QVector<QVector3D>(sizeX);
        x = minX;

        // Результаты записываются в вектор точек, который добавляется в соответствующее сечение
        for (qint32 j {0}; j < sizeX; j++, x += stepX) {
            (*sections)[i][j] = QVector3D(x, f(x, z), z);
        }
    }

    // Возвращаем вектор сечений
    return *sections;
}
