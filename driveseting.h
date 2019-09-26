#ifndef DRIVESETING_H
#define DRIVESETING_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QGridLayout>
#include "my_control.h"
#include "camera.h"
#include "serial.h"
#include "worker.h"

namespace Ui {
class DriveSeting;
}

class DriveSeting : public QWidget
{
    Q_OBJECT

public:
    explicit DriveSeting(QWidget *parent = nullptr);
    ~DriveSeting();

    bool init();

    void display_init();

    QString init_camera();

    QString init_serial();

    SerialSetting get_serial_setting();

    QJsonObject get_algorithm();

    QSerialPort *Serial();

    void load_setting();

    void check_self();

    unsigned short int CRC16(unsigned char* pchMsg, unsigned short int wDataLen);

    void scan_serial();

    void save_serial_config();
    void save_algorithmic_config();
    void operation_open_camera(HKCamera *camera);
    void operation_close_camera(HKCamera * camera);
    void operation_start_collect(HKCamera * camera);
    void operation_stop_collect(HKCamera * camera);
    void operation_collect_image(HKCamera * camera);
    void operation_set_params(HKCamera * camera);
    void operation_get_params(HKCamera * camera);

    void operation_save_config(HKCamera * camera);

    void accept_return_serial_setting();
    void accept_serial_status(bool isopened);

    QJsonObject get_camera_setting_json();
    QJsonObject get_camera2_setting_json();

    void set_camera_json_setting(QJsonObject json);
    void set_camera2_json_setting(QJsonObject json);
    void set_algorithmic_setting(QJsonObject json);

signals:
    void tell_window_step_page(int);
    void tell_screencheck_setting(SerialSetting);

private slots:
    void on_serialPortInfoListBox_currentIndexChanged(int index);

    void on_pushButton_released();

    void on_pushButton_6_released();

    void on_pushButton_5_released();

    void on_pushButton_7_released();

    void on_pushButton_2_released();

    void on_pushButton_18_released();

    void on_pushButton_9_released();

    void on_pushButton_17_released();

    void on_pushButton_10_released();

    void on_pushButton_12_released();

    void on_pushButton_14_released();

    void on_pushButton_11_clicked();

    void on_pushButton_4_released();

    void on_pushButton_31_released();

    void on_pushButton_30_released();

    void on_pushButton_32_released();

    void on_pushButton_33_released();

    void on_pushButton_34_released();

    void on_pushButton_29_released();

    void on_pushButton_36_released();


private:
    Ui::DriveSeting *ui;
    QSerialPort *serial;
    HKCamera *camera;
    HKCamera *camera_2;

    bool Serial_OK;
    bool Camera_OK;

    bool worker_serial_status;
};

#endif // DRIVESETING_H
