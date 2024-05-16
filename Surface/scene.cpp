#include "scene.h"
#include <QtMath>

// Конструктор
Scene::Scene() {}

// Скалярное произведение двух векторов
qreal Scene::product(const QVector4D& row, const QVector4D& col) const {
    // Переменная, хранящая результат
    qreal res {0};

    // Перебираем элементы векторов, и на каждой итерации происходит умножение соответствующих элементов
    for (int i {0}; i < 4; i++) {
        res += row[i] * col[i];
    }

    // Возвращаем результат
    return res;
}

// Метод возвращает матрицу преобразования из системы координат мира (CSW) в систему координат наблюдателя (CSO)
QMatrix4x4 Scene::getTransitionMatrixFromCSWToCSO(const QVector3D &coordObserver) const {
    // Матрица смещения, которая перемещает начало координат из центра мира в точку, соответствующую координатам наблюдателя
    QMatrix4x4 shift(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -coordObserver.x(), -coordObserver.y(), -coordObserver.z(), 1
        );

    // Матрица, которая отражает ось X, так как в CSW ось X направлена вправо, а в CSO - влево
    QMatrix4x4 rightToLeft(
        -1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    // Матрица, которая поворачивает ось Y вверх, так как в CSW ось Y направлена вверх, а в CSO - вниз
    QMatrix4x4 rotationYUp(
        1, 0, 0, 0,
        0, 0, -1, 0,
        0, 1, 0, 0,
        0, 0, 0, 1
        );

    // Матрица, которая поворачивает систему координат вокруг оси X на угол, соответствующий положению наблюдателя
    QMatrix4x4 rotationAroundX(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    // Матрица, которая поворачивает систему координат вокруг оси Y на угол, соответствующий положению наблюдателя
    QMatrix4x4 rotationAroundY(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    // Синус и косинус
    qreal sin {};
    qreal cos {};

    // Расстояние от наблюдателя до плоскости XY
    qreal distanceZ {qSqrt(coordObserver.x() * coordObserver.x() + coordObserver.y() * coordObserver.y())};

    // Если расстояния не равны нулю, то вычисляются значения синуса и косинуса углов поворота и соответствующие элементы матриц rotationAroundY заполняются этими значениями
    if (distanceZ != 0) {
        sin = coordObserver.x() / distanceZ;
        cos = coordObserver.y() / distanceZ;

        rotationAroundY(0, 0) = cos;
        rotationAroundY(0, 2) = sin;
        rotationAroundY(2, 0) = -sin;
        rotationAroundY(2, 2) = cos;
    }

    // Расстояние от наблюдателя до центра мира
    qreal distanceCentre {qSqrt(distanceZ * distanceZ + coordObserver.z() * coordObserver.z())};

    // Если расстояния не равны нулю, то вычисляются значения синуса и косинуса углов поворота и соответствующие элементы матриц rotationAroundX заполняются этими значениями
    if (distanceCentre != 0) {
        sin = coordObserver.z() / distanceCentre;
        cos = distanceZ / distanceCentre;

        rotationAroundX(1, 1) = cos;
        rotationAroundX(1, 2) = -sin;
        rotationAroundX(2, 1) = sin;
        rotationAroundX(2, 2) = cos;
    }

    // Возвращается вычисленная матрица преобразования
    return shift * rightToLeft * rotationYUp * rotationAroundY * rotationAroundX;
}

// Метод выполняет преобразование координат сечений из системы координат мира (CSW) в систему координат наблюдателя (CSO)
QVector<QVector<QVector3D>>& Scene::transitionFromCSWToCSO(const QVector3D &coordObserver) const {
    // Вектор, который содержит сечения, полученные из поверхности
    QVector<QVector<QVector3D>> sectionsInCSW(surface.getSections());

    // Матрица преобразования, которая будет использоваться для преобразования координат
    QMatrix4x4 transitionMatrix(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
        );

    // Расстояние от наблюдателя до плоскости, проходящей через начало координат и перпендикулярной оси Y
    qreal dis {qSqrt(coordObserver.y() * coordObserver.y() + coordObserver.z() * coordObserver.z())};

    // Если это расстояние не равно нулю, то вычисляются синус и косинус угла между осью Z и этой плоскостью. Эти значения записываются в соответствующие элементы матрицы
    if (dis != 0) {
        qreal sin {coordObserver.y() / dis};
        qreal cos {coordObserver.z() / dis};

        transitionMatrix(1, 1) = cos;
        transitionMatrix(1, 2) = -sin;
        transitionMatrix(2, 1) = sin;
        transitionMatrix(2, 2) = cos;
    }

    // Вектор, который будет содержать сечения в системе координат наблюдателя
    QVector<QVector<QVector3D>> *sectionsInCSO = new QVector<QVector<QVector3D>>(sectionsInCSW.length());

    // Для каждого сечения выполняется цикл, в котором для каждой точки сечения вычисляются координаты в системе координат наблюдателя с помощью матрицы
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

    // Ссылка на вектор
    return *sectionsInCSO;
}

// Метод выполняет преобразование координат сечений из системы координат наблюдателя (CSO) в систему координат проекции (CSP)
QVector<QVector<QVector2D>>& Scene::transitionFromCSOToCSP(const QVector<QVector<QVector3D>> &sectionsInCSO) const {
    // Указатель на новый вектор, который будет содержать сечения в системе координат проекции
    QVector<QVector<QVector2D>> *sectionsInCSP = new QVector<QVector<QVector2D>>(sectionsInCSO.length());

    // Цикл по всем сечениям в векторе, создаётся ещё один вектор, который будет содержать точки сечения в системе координат проекции
    for (qint32 i {0}; i < sectionsInCSO.length(); i++) {
        (*sectionsInCSP)[i] = QVector<QVector2D>(sectionsInCSO[0].length());

        // Цикл по всем точкам сечения в векторе, создаётся новый вектор с двумя компонентами, который будет содержать координаты точки сечения в системе координат проекции
        for (qint32 j {0}; j < sectionsInCSO[0].length(); j++) {
            (*sectionsInCSP)[i][j] = QVector2D(sectionsInCSO[i][j].x(), sectionsInCSO[i][j].y());
        }
    }

    // Удаляем вектор
    delete &sectionsInCSO;

    // Возвращаем ссылку
    return *sectionsInCSP;
}

// Метод вычисляет половину размера прямоугольника, ограничивающего все сечения в системе координат проекции (CSP)
QVector2D& Scene::getHalfSizeInCSP(const QVector<QVector<QVector2D>> &sectionsInCSP) const {
    // Указатель на вектор с двумя компонентами, инициализированными значениями модулей координат x и y первой точки первого сечения
    QVector2D *halfSizeInCSP = new QVector2D(abs(sectionsInCSP[0][0].x()), abs(sectionsInCSP[0][0].y()));

    // Перебор всех сечений и точек в них
    for (const QVector<QVector2D> &section : sectionsInCSP)
        // Для каждой точки проверяется, что её модуль координаты x или y больше соответствующей компоненты вектора
        for (const QVector2D &point : section) {
            if (abs(point.x()) > halfSizeInCSP->x()) {
                halfSizeInCSP->setX(abs(point.x()));
            }
            if (abs(point.y()) > halfSizeInCSP->y()) {
                halfSizeInCSP->setY(abs(point.y()));
            }
        }

    // Возвращает ссылку на вектор, содержащий половину размера прямоугольника, ограничивающего все сечения в CSP
    return *halfSizeInCSP;
}

// Преобразование координат сечений из системы координат проекции (CSP) в систему координат экрана (CSS)
void Scene::transitionFromCSPToCSS(QVector<QVector<QVector2D>> &sectionsInCSP, QVector2D &halfSizeInCSP, const QVector2D &center, const QVector2D &halfSizeInCSS) const {
    // Вычисляется максимальное значение, которое будет использовано для масштабирования координат сечений
    qreal max = halfSizeInCSP.x() > halfSizeInCSP.y() ? halfSizeInCSP.x() : halfSizeInCSP.y();

    // Цикл по всем сечениям и точкам в них, для каждой точки вычисляются новые координаты в системе координат экрана (CSS)
    for (QVector<QVector2D> &section : sectionsInCSP) {
        for (QVector2D &point : section) {
            point.setX(point.x() * halfSizeInCSS.x() / max + center.x());
            point.setY(point.y() * halfSizeInCSS.y() / max + center.y());
            point.setY(2 * center.y() - point.y());
        }
    }

    // Освобождаем память
    delete &halfSizeInCSP;
}

// Метод рисует сцену на основе координат наблюдателя
void Scene::drawFor(const QVector3D& coordObserver, QPainter &painter, qint32 width,  qint32 height) const {
    // Преобразование координат сечений из системы координат мира (CSW) в систему координат наблюдателя (CSO)
    QVector<QVector<QVector2D>> sectionsInCSP(transitionFromCSOToCSP(transitionFromCSWToCSO(coordObserver)));
    // Сечения преобразуются из CSO в систему координат проекции (CSP)
    transitionFromCSPToCSS(sectionsInCSP, getHalfSizeInCSP(sectionsInCSP), QVector2D(width / 2, height / 2), QVector2D(width / 2, height / 2));

    // Создаём два вектора, которые будут использоваться для отслеживания горизонтальной линии
    QVector<qreal> horizonUp(sectionsInCSP[0].length());
    for (qreal &pointY : horizonUp) {
        pointY = -1;
    }

    QVector<qreal> horizonDown(sectionsInCSP[0].length());
    for (qreal &pointY : horizonDown) {
        pointY = height + 1;
    }

    // Создаём два объекта с цветами синий и белый, которые будут использоваться для рисования точек сечений
    QPen red(Qt::blue, 1);
    QPen green(Qt::white, 1);

    // Перебираем все сечения в цикле и рисуем точки каждого сечения
    for (qint32 i {0}; i < sectionsInCSP.length(); i++) {
        for (qint32 j {0}; j < sectionsInCSP[i].length(); j++) {
            // Если точка сечения находится ниже, чем текущая точка горизонтальной линии в векторе horizonDown,
            // то эта точка горизонтальной линии обновляется и точка сечения рисуется красным цветом
            if (sectionsInCSP[i][j].y() < horizonDown[j]) {
                horizonDown[j] = sectionsInCSP[i][j].y();

                if (i != 0) {
                    painter.setPen(Qt::blue);
                }

                painter.drawPoint(sectionsInCSP[i][j].toPointF());
            }
            // Если точка сечения находится выше, чем текущая точка горизонтальной линии в векторе horizonUp,
            // то эта точка горизонтальной линии обновляется и точка сечения рисуется зелёным цветом
            if (sectionsInCSP[i][j].y() > horizonUp[j]) {
                horizonUp[j] = sectionsInCSP[i][j].y();

                if (i != 0) {
                    painter.setPen(Qt::white);
                }

                painter.drawPoint(sectionsInCSP[i][j].toPointF());
            }
        }
    }
}
