#include "observer.h"
#include <QPainter>
#include <QMessageBox>
#include <QtMath>

Observer::Observer(qreal x, qreal y, qreal z, QWidget *parent): QWidget(parent) {
    setStyleSheet("background-color: black");
    setMinimumSize(QSize(100, 100));
    resize(700, 500);

    coordObserver = QVector3D(x, y, z);

    QPalette palette;
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::ButtonText, Qt::white);

    qint32 widthButton = 40;
    qint32 heightButton = 20;

    inputX = new QLineEdit(QString().setNum(x), this);
    inputX->move(0, 0);
    inputX->setMaximumSize(QSize(widthButton, heightButton));
    inputX->setPalette(palette);
    inputX->setMaxLength(5);

    inputY = new QLineEdit(QString().setNum(y), this);
    inputY->move(widthButton, 0);
    inputY->setMaximumSize(QSize(widthButton, heightButton));
    inputY->setPalette(palette);
    inputY->setMaxLength(5);

    inputZ = new QLineEdit(QString().setNum(z), this);
    inputZ->move(2 * widthButton, 0);
    inputZ->setMaximumSize(QSize(widthButton, heightButton));
    inputZ->setPalette(palette);
    inputZ->setMaxLength(5);

    timer = new QTimer();

    restart = new QPushButton("Start", this);
    restart->move(0, heightButton);
    restart->setMinimumSize(QSize(3 * widthButton, heightButton));
    restart->setMaximumSize(QSize(3 * widthButton, heightButton));
    restart->setPalette(palette);

    connect(timer, &QTimer::timeout, this, &Observer::movingObserver);
    connect(restart, &QPushButton::clicked, this, &Observer::startOrStop);
    connect(inputX, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
    connect(inputY, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
    connect(inputZ, &QLineEdit::returnPressed, this, &Observer::newCoordObserver);
}

void Observer::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(Qt::white, 1));

    scene.drawFor(coordObserver, painter, width(), height());
}

void Observer::newCoordObserver() {
    if (inputX->text().isEmpty() || inputY->text().isEmpty() || inputZ->text().isEmpty()) {
        QMessageBox message(QMessageBox::Information, "Prism", "The entered string is empty!", QMessageBox::Ok);
        message.exec();
        return;
    }

    bool okX;
    bool okY;
    bool okZ;

    QVector3D newCoordObserver{inputX->text().toFloat(&okX), inputY->text().toFloat(&okY), inputZ->text().toFloat(&okZ)};

    if (okX && okY && okZ) {
        coordObserver = newCoordObserver;
        update();
    } else {
        QMessageBox message(QMessageBox::Information, "Prism", "The entered string is invalid!", QMessageBox::Ok);
        message.exec();
    }
}

void Observer::startOrStop() {
    if (timer->isActive()) {
        timer->stop();
        restart->setText("Start");

        return;
    }

    radius = qSqrt(coordObserver.y() * coordObserver.y() + coordObserver.z() * coordObserver.z());
    param = 0;
    timer->start(20);
    restart->setText("Stop");
}

void Observer::movingObserver() {
    param += 0.01;

    if (param >= M_PI) {
        param = 0;
    }

    coordObserver.setY(radius * qCos(param));
    inputY->setText(QString().setNum(coordObserver.y()));

    coordObserver.setZ(radius * qSin(param));
    inputZ->setText(QString().setNum(coordObserver.z()));

    update();
}
