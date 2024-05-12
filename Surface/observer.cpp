#include "observer.h"
#include <QPainter>
#include <QMessageBox>
#include <QtMath>

// Конструктор
Observer::Observer(qreal x, qreal y, qreal z, QWidget *parent): QWidget(parent) {
    // Здадаём задний фон, минимальный и основной размеры виджета
    setStyleSheet("background-color: black");
    setMinimumSize(QSize(100, 100));
    resize(700, 500);

    // Инициализируем позицию наблюдателя
    coordObserver = QVector3D(x, y, z);

    // Настраиваем палитру цветов - задаём белый цвет для текста и кнопок
    QPalette palette;
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);

    // Задаём размер кнопок
    qint32 widthButton = 40;
    qint32 heightButton = 20;

    // Создаём и настраиваем поле ввода координаты по x
    inputX = new QLineEdit(QString().setNum(x), this);
    inputX->move(0, 0);
    inputX->setMaximumSize(QSize(widthButton, heightButton));
    inputX->setPalette(palette);
    inputX->setMaxLength(5);

    // Создаём и настраиваем поле ввода координаты по y
    inputY = new QLineEdit(QString().setNum(y), this);
    inputY->move(widthButton, 0);
    inputY->setMaximumSize(QSize(widthButton, heightButton));
    inputY->setPalette(palette);
    inputY->setMaxLength(5);

    // Создаём и настраиваем поле ввода координаты по z
    inputZ = new QLineEdit(QString().setNum(z), this);
    inputZ->move(2 * widthButton, 0);
    inputZ->setMaximumSize(QSize(widthButton, heightButton));
    inputZ->setPalette(palette);
    inputZ->setMaxLength(5);

    // Создаём таймер для преиодического обновления позиции наблюдателя
    timer = new QTimer();

    // Создаём и настраиваем кнопку запуска/рестарта
    restart = new QPushButton("Start", this);
    restart->move(0, heightButton);
    restart->setMinimumSize(QSize(3 * widthButton, heightButton));
    restart->setMaximumSize(QSize(3 * widthButton, heightButton));
    restart->setPalette(palette);

    // Сигнально-слотовые соединения
    connect(timer, &QTimer::timeout, this, &Observer::movingObserver);
    connect(restart, &QPushButton::clicked, this, &Observer::startOrStop);
    connect(inputX, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
    connect(inputY, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
    connect(inputZ, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
}

// Рисование содержимого виджета на экране
void Observer::paintEvent(QPaintEvent *event) {
    // Создаём объект для рисования и задаём сглаживание краёв, белый цвет пера и толщину в 1 пиксель
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 1));

    // Вызываем метод, отвечающий за рисование сцены на виджете
    scene.drawFor(coordObserver, painter, width(), height());
}

// Обновление координат наблюдателя
void Observer::newCoordObserver() {
    // Проверяем, что поля не пустые, иначе - выводим сообщение об ошибке
    if (inputX->text().isEmpty() || inputY->text().isEmpty() || inputZ->text().isEmpty()) {
        QMessageBox message(QMessageBox::Information, "Prism", "The entered string is empty!", QMessageBox::Ok);
        message.exec();
        return;
    }

    // Флаги для проверки преобразования из строки в число
    bool okX;
    bool okY;
    bool okZ;

    // Создаём новые координаты
    QVector3D newCoordObserver{inputX->text().toFloat(&okX), inputY->text().toFloat(&okY), inputZ->text().toFloat(&okZ)};

    // Если все координаты корректны, выполняем присваивание
    if (okX && okY && okZ) {
        coordObserver = newCoordObserver;
        update();
    } else {
        // Иначе выводим сообщение об ошибке
        QMessageBox message(QMessageBox::Information, "Prism", "The entered string is invalid!", QMessageBox::Ok);
        message.exec();
    }
}

// Запуск и остановка таймера
void Observer::startOrStop() {
    // Если таймер запущен, то останавливаем его и меняем текст кнопки
    if (timer->isActive()) {
        timer->stop();
        restart->setText("Start");

        return;
    }

    // Если таймер не запущен, то вычисляем радиус и угол для движения наблюдателя по окружности
    radius = qSqrt(coordObserver.y() * coordObserver.y() + coordObserver.z() * coordObserver.z());
    param = 0;

    // Запускаем таймер и меняем текст кнопки
    timer->start(20);
    restart->setText("Stop");
}

// Движение наблюдателя вокруг центра координат
void Observer::movingObserver() {
    // Увеличиваем параметр (угол в радианах) и проверяем, не провосходит ли он число пи, если да - обнуляем
    param += 0.01;

    if (param >= M_PI) {
        param = 0;
    }

    // Вычисляем и устанавливаем новые координаты
    coordObserver.setY(radius * qCos(param));
    inputY->setText(QString().setNum(coordObserver.y()));

    coordObserver.setZ(radius * qSin(param));
    inputZ->setText(QString().setNum(coordObserver.z()));

    // Перерисовываем сцену с новыки координатами наблюдателя
    update();
}
