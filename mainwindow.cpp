#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdebug.h>






MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    repaint_timer = new QTimer(this);
    repaint_timer->setInterval(100);
    connect(repaint_timer, SIGNAL(timeout()), this, SLOT(update()));
    repaint_timer->start();

    serial = new QSerialPort();
    serial->setPortName("ttyUSB0");
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    serial->open(QIODevice::ReadWrite);
    serial->write("ok");

    frameRX.bytesRX= new uint8_t [RX_FRAME_BYTES];
    connect(serial,SIGNAL(readyRead()), this, SLOT(serialReceived()));
    ui->label->setText("");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    ui->label->setText(QString::number(frameRX.values->angle, 10));
}

void MainWindow::on_pushButton_clicked()
{


}




void MainWindow::serialReceived(){
    receivedData.append(serial->read(RX_FRAME_BYTES));

    if(((uint8_t)(receivedData[receivedData.size()-1]) == 0x80) && (receivedData.size() >= 4)){
        if((receivedData.size() == 4) && ((uint8_t)(receivedData[0]) == 0x40)){

            for (int i = 0; i < receivedData.size(); i++) {
                frameRX.bytesRX[i] = (uint8_t) receivedData[i];
                //qDebug() << QString::number((uint8_t)datax[i], 10);
            }

        } else qDebug() << "Error!!!";

        receivedData.clear();
    }
}
