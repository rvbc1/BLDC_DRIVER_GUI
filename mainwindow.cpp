#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdebug.h>

#define DATA_FRAME_TX_SIZE 7
#define DATA_FRAME_RX_SIZE 6

#define START_CODE 0x40
#define END_CODE 0x80

#define TRACKING_INTERVAL 50

struct dataFrameTX{
    uint8_t start_code;
    uint8_t code;
    uint16_t angle;
    uint16_t torque;
    uint8_t end_code;
} __attribute__ ((__packed__));

struct dataFrameRX{
    uint8_t start_code;
    uint16_t angle;
    int16_t real_angle;
    uint8_t end_code;
} __attribute__ ((__packed__));

union{
    dataFrameTX data;
    char bytes[DATA_FRAME_TX_SIZE];
}frameTX;


union{
    dataFrameRX data;
    char bytes[DATA_FRAME_RX_SIZE];
}frameRX;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    serial = new QSerialPort();
    prepareSerial();

    recieve_timer = new QTimer(this);
    recieve_timer->setInterval(100);
    connect(recieve_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(recieve_timer, SIGNAL(timeout()), this, SLOT(serialReceived()));
    //repaint_timer->start();


    //connect(serial,SIGNAL(readyRead()), this, SLOT(serialReceived()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    ui->label->setText(QString::number(frameRX.data.angle, 10));
    ui->lcdNumber->display(double(frameRX.data.angle) / 2.0);
    ui->lcdNumber_2->display(frameRX.data.real_angle);
}

void MainWindow::on_pushButton_clicked()
{
    serial->close();
    if(ui->comboBox->count() > 0){
        serial->setPortName(ui->comboBox->currentText());
        serial->open(QIODevice::ReadWrite);
        //enableDataWidgets();
        ui->progressBar->setTextVisible(true);
        ui->progressBar->setValue(100);
        recieve_timer->start();
    } else {

    }

}

void MainWindow::addAvaibleSerials(){
    QList<QSerialPortInfo> lista = QSerialPortInfo::availablePorts();

    for (int i = 0; i < lista.size(); i++){
        ui->comboBox->addItem(lista[i].portName());
    }
}

void MainWindow::prepareSerial(){
    addAvaibleSerials();
    serial = new QSerialPort();
    serial->setBaudRate(QSerialPort::Baud115200);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::prepareData(){
    frameTX.data.start_code = START_CODE;

    frameTX.data.end_code = END_CODE;
}


void MainWindow::serialReceived(){
//    frameTX.data.torque = ui->spinBox->value();
//    frameTX.data.angle = ui->spinBox_2->value();
//    if(serial->isOpen())
//        serial->write(frameTX.bytes, DATA_FRAME_TX_SIZE);


    QByteArray ba = serial->readAll();
    for(int i =0; i < ba.size() - DATA_FRAME_RX_SIZE; i++){
        if((uint8_t(ba[i]) == START_CODE) && ((uint8_t(ba[i + DATA_FRAME_RX_SIZE - 1])) == END_CODE)){
            for(int j = 0; j < DATA_FRAME_RX_SIZE; j++){
                frameRX.bytes[j] = ba[i + j];
            }
            //            ba.remove(i, DATA_FRAME_RX_SIZE);
            //            qDebug() << ba.size() << "ok";
        }
    }

    //    receivedData.append(serial->read(RX_FRAME_BYTES));

    //    if(((uint8_t)(receivedData[receivedData.size()-1]) == 0x80) && (receivedData.size() >= 4)){
    //        if((receivedData.size() == 4) && ((uint8_t)(receivedData[0]) == 0x40)){

    //            for (int i = 0; i < receivedData.size(); i++) {
    //                frameRX.bytes[i] = (uint8_t) receivedData[i];
    //                //qDebug() << QString::number((uint8_t)datax[i], 10);
    //            }

    //        } else qDebug() << "Error!!!";

    //        receivedData.clear();
    //    }
}

void MainWindow::on_pushButton_2_pressed(){
    frameTX.data.torque = ui->spinBox->value();
    frameTX.data.angle = ui->spinBox_2->value();
    if(serial->isOpen())
        serial->write(frameTX.bytes, DATA_FRAME_TX_SIZE);
}
