#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QtSerialPort>

#define RX_FRAME_BYTES 4

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    struct dataRX_s{
        uint8_t start_transsmision_byte;
        uint16_t angle;
        uint8_t end_transsmision_byte;
    } __attribute__ ((__packed__));

    union frameRX_u{
        uint8_t *bytesRX;
        struct dataRX_s *values;
    }frameRX;

    QTimer *repaint_timer;
    QSerialPort *serial;
    QByteArray receivedData;


public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void update();
    void serialReceived();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
