#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qdebug.h>

#define DATA_FRAME_TX_SIZE 7
#define DATA_FRAME_RX_SIZE 20

#define START_CODE 0x40
#define END_CODE 0x80

#define TRACKING_INTERVAL 50

struct dataRX{
    uint16_t angle;
    int16_t speed_pv;
    int16_t sp;
    int16_t e;
    int16_t u;
    int16_t dAngle;
    uint32_t dt;
    uint16_t fi;
};

struct dataFrameTX{
    uint8_t start_code;
    uint8_t code;
    uint16_t angle;
    uint16_t torque;
    uint8_t end_code;
} __attribute__ ((__packed__));

struct dataFrameRX{
    uint8_t start_code;
    dataRX data;
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
    serial_updater_timer = new QTimer(this);
    serial_updater_timer->setInterval(1000);
    connect(recieve_timer, SIGNAL(timeout()), this, SLOT(update()));
    connect(recieve_timer, SIGNAL(timeout()), this, SLOT(serialReceived()));
    connect(serial_updater_timer, SIGNAL(timeout()), this, SLOT(updateAvaibleSerials()));

    serial_updater_timer->start();
    //repaint_timer->start();

    for(int i =0; i < 2; i ++){
        for(int j =0; j < 2; j ++){
            QLabel *l = new QLabel();
            //ui->gridLayout->addWidget(l, i, j);
            // l->setPixmap(QPixmap("/home/rvbc-/Downloads/myAvatar.png"));
        }
    }



    //connect(serial,SIGNAL(readyRead()), this, SLOT(serialReceived()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    //ui->label->setText(QString::number(frameRX.data.angle, 10));
    ui->lcdNumber->display(frameRX.data.data.angle);
    ui->lcdNumber_2->display(frameRX.data.data.speed_pv);
    ui->lcdNumber_3->display(frameRX.data.data.sp);
    ui->lcdNumber_4->display((int)frameRX.data.data.e);
    ui->lcdNumber_5->display((int)frameRX.data.data.u);
    ui->lcdNumber_6->display(frameRX.data.data.fi);
    ui->lcdNumber_7->display(frameRX.data.data.dAngle);
    ui->lcdNumber_8->display(uint16_t(frameRX.data.data.dt));
    ui->label_9->setText(QString::number(frameRX.data.data.dt));
}



void MainWindow::on_pushButton_clicked(){
    if(serial->isOpen()){
        closeSerial();
    } else {
        openSerial();
    }
}

void MainWindow::updateAvaibleSerials(){
    QString selected_serial = ui->comboBox->currentText();
    ui->comboBox->clear();
    addAvaibleSerials();
    if(ui->comboBox->findText(selected_serial) != -1){
        ui->comboBox->setCurrentIndex(ui->comboBox->findText(selected_serial));
    }


    if(serial->isOpen() && (ui->comboBox->findText(opened_serial) == -1)){
        //serial->close();
        closeSerial();
        //void disconnect
    }
}

void MainWindow::openSerial(){
    serial->close();
    if(ui->comboBox->count() > 0){
        serial->setPortName(ui->comboBox->currentText());
        if(serial->open(QIODevice::ReadWrite)){
            opened_serial = ui->comboBox->currentText();
            //enableDataWidgets();
            ui->progressBar->setTextVisible(true);
            ui->progressBar->setValue(100);
            ui->pushButton->setText("Disconnect");
            recieve_timer->start();
        }
    } else {

    }
}

void MainWindow::closeSerial(){
    serial->close();
    ui->progressBar->setTextVisible(false);
    ui->progressBar->setValue(0);
    recieve_timer->stop();
    ui->pushButton->setText("Connect");
}

void MainWindow::addAvaibleSerials(){
    QList<QSerialPortInfo> lista = QSerialPortInfo::availablePorts();
    for (int i = 0; i < lista.size(); i++){
        ui->comboBox->addItem(lista[i].portName());
    }
}

void MainWindow::prepareSerial(){
    updateAvaibleSerials();
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
    if(serial->isOpen()){
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
    }
}

void MainWindow::on_pushButton_2_pressed(){
    frameTX.data.torque = ui->spinBox->value();
    frameTX.data.angle = ui->spinBox_2->value();
    if(serial->isOpen())
        serial->write(frameTX.bytes, DATA_FRAME_TX_SIZE);
}
