#ifndef WORKER_H
#define WORKER_H

#include <QObject>
#include <QThread>
#include <QtSerialPort>
#include <QString>
#include <QTimer>
#include <QDebug>
#include <QList>
#include "camera.h"
#include "serial.h"

typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned int            uint32_t;

enum Command:short
{
    QueryStatus = 0x0000,
    StartWork = 0x0001,
    CheckResult = 0x0002,
    StopWork = 0x0003,
    Reset = 0x0004,
    SetTransportMotorSpeed = 0x0005,
    SetRollMontorSpeed = 0x0006,
    SetRollerMotorSpeed = 0x0007,
    SetSlidingTableMotorSpeed = 0x0008,
    SuspendWork = 0x0009,
    ContinueWork = 0x0010,

    RespondNO = 0x00FF,
    ImageCapture = 0x00FE,
    STM_WorkStatus = 0x00FD,
    WrapResult = 0x00FC,
    RespondOK = 0x00FB,
    EmergencyStop = 0x00FA,
    STMReportFault = 0x00F9,
    PackingBagLess = 0x00F8,
};

struct Package
{
    uint16_t head;                        //命令
    uint8_t packageNumber;                //帧序号
    uint8_t packageLength;                //长度
    uint8_t commandCount;                 //命令个数
    uint16_t command;                     //命令
    uint8_t length;                       //长度
    uint8_t *pBuffer;                     //数据
    uint16_t crc = 0;
};

struct Respond{
    bool isGood = false;
    Command command;
    QByteArray data;
};

struct Status{

    bool E_Stop;
    bool transportMotorSpeedStatus;
    bool rollMontorSpeedStatus;
    bool rollerMotorSpeedStatus;
    bool slidingTableMotorSpeedStatus;
    bool isWorking;
    bool isReseting;
    bool isStoped;

    bool grab_cylinder;
    bool magnetic_roller_positioning_cylinder;
    bool magnetic_roller_lifting_cylinder;
    bool slider_lift_cylinder;
    bool sucker_cylinder;
    bool pressure_cylinder;
    bool cutting_cylinder;
    bool waste_cylinder;
    bool packingbag_fault;
    bool material_buffer_cylinde;

    uint16_t transport_motor_speed;
    uint16_t roll_motor_speed;
    uint16_t roller_motor_speed;
    uint16_t slidingtable_motor_speed;
};

class Worker:public QObject
{
    Q_OBJECT

public:
    Worker(){
        this->serial = new QSerialPort(this);
        this->timer = new QTimer(this);
        this->init_setting_and_connect();
    }
    ~Worker(){}

    void init_setting_and_connect();

    void query_status();
    void start_work();
    void response_check_command();
    void response_wrap_result(const QByteArray &data);

    void response_continue_work();
    void response_suspend_work();
    void response_stop_work();
    void response_reset_work();
    void set_motor_speed(Command com,uint16_t data);

    void analysis_FaultCode(const QByteArray &data);
    void analysis_MCStatus(const QByteArray &data);
    static QByteArray dump_data(const Command &command,const QByteArray &data);
    static Respond load_data(const QByteArray &data);

signals:
    void tell_window_message(QString type, QString msg);
    void tell_window_stm_status(Status);
    void tell_window_stm_respond_timeout();
    void tell_window_command(Command,QVariant);
    void tell_window_serial_status(bool isopened);
    void tell_window_work_step(int step);

public slots:
    void accept_read_data();
    void accept_timeout();
    void accept_serial_error(const QSerialPort::SerialPortError &error);

    void accept_command_to_stm(Command com,int data);
    void accept_return_serial_status(){emit tell_window_serial_status(this->serial->isOpen());}

    void accept_open_serial(SerialSetting setting);
    void accept_start_worker(SerialSetting setting);
    void accept_stop_work();

private:
    int step = 0;

    bool is_Stoped_Work = false;
    bool do_noting = false;

    int quality = 0;
    QTimer *timer;
    QSerialPort *serial;
    Status status;
    SerialSetting setting;
    Status status_speed;
};


#endif
