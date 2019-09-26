#ifndef SCREENCHECK_H
#define SCREENCHECK_H

#include "my_control.h"
#include "camera.h"
#include "login.h"
#include "worker.h"
#include "pixitem.h"
#include "driveseting.h"
#include "DefectsDetect.h"
#include "visionworker.h"
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QWidget>
#include <QMap>
#include <QString>
#include <QtSerialPort>
#include <functional>

namespace Ui {
class ScreenCheck;
}

struct CurrentCheckResult
{
    int isOK;
    bool wrapedOver;
    QString isQualified_s;
    QString wrapedOver_s;
    QString scratch;
    QString whitePoint;
    QString blackPoint;
};

class ScreenCheck : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenCheck(QWidget *parent = nullptr);
    ~ScreenCheck();

    void addlabel(const QString &name,const QString &content);
    void start_check();
    int getMaxID();


    void setMen(const Meninfo &info);
    void setSerial(QSerialPort* serial_);

    void setDriversetting(DriveSeting *drivesetting);

    void save_check_result(int product_id);

    QLabel * camera_label();
    void ImageCapture();
    bool workerSerialIsOpen();

    void analysis_FaultCode(QVariant data,QString &str);

public slots:
    void accept_stm_command(Command com,QVariant data);
    void accept_stm_respond_timeout();

    void accept_stm_status(Status status);
    void accept_serial_setting(SerialSetting serialsetting){this->serial_setting = serialsetting;}
    void accept_worker_serial_status(bool isopened){this->worker_thread_serial_status = isopened; qDebug() << "ask and get serial:"<< worker_thread_serial_status;}
    void accept_worker_step(int step);

    void accept_detect_result(int result,int itype,HObject deal_image);
    void HKInfoCallBack(const MV_FRAME_OUT_INFO &SInfo);

signals:
    void tell_window_step_page(int page);
    void tell_window_start_check();
    void tell_window_stm_status(Status);

    void ask_serial_setting();
    void ask_worker_serial_status();

    void tell_worker_open_serial(SerialSetting);
    void tell_worker_start_work(SerialSetting);
    void tell_worker_stop_work();
    void tell_worker_stm_command(Command,int);
    void tell_result_update_data();
    void tell_camera1_run(QJsonObject,int,int);

private slots:
    void on_pushButton_4_released();

    void on_start_check_released();

    void on_pushButton_17_released();

    void on_pushButton_18_released();

    void on_pushButton_8_released();

    void on_checkBox_3_stateChanged(int arg1);

    void on_pushButton_9_released();

private:
    Ui::ScreenCheck *ui;
    QMap<QString ,Label *> object_label_map{};
    QSerialPort *serial;
    HKCamera *camera;
    Meninfo men_info;
    DriveSeting *drivesetting;

    QThread *worker_thread;
    Worker *worker;

    QThread *camera1_thread;
    VisionWorker *visionwork;

    HObject ho_Image;
    HObject deal_Image;

    CurrentCheckResult current_check_result;
    SerialSetting serial_setting;

    QPixmap *qpixmap;
    QGraphicsView *graph_view_preview;
    QGraphicsScene *graph_scene;
    PixItem *pixitem;

    QString filename;
    bool worker_thread_serial_status = false;
    unsigned char id= 1;

    int all_count = 0;
    int ok_count = 0;
    int ng_count = 0;
    int good_count = 0;
};

#endif // SCREENCHECK_H
