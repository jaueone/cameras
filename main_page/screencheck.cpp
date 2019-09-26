#include "screencheck.h"
#include "ui_screencheck.h"
#include "DefectsDetect.h"
#include "my_control.h"
#include <QSqlError>
#include <QDateTime>
#include <QScreen>
#include <QGuiApplication>
#include <QFileDialog>

void msleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

const unsigned char chCRCHTalbe[] =
{
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,// CRC 高位字节值表
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40
};

const unsigned char chCRCLTalbe[] =
{
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7,// CRC 低位字节值表
0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E,
0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09, 0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9,
0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC,
0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32,
0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D,
0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A, 0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38,
0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF,
0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1,
0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4,
0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F, 0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB,
0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA,
0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0,
0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97,
0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C, 0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E,
0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89,
0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83,
0x41, 0x81, 0x80, 0x40
};

unsigned short int CRC16(unsigned char *pchMsg, unsigned short int wDataLen)
{
    unsigned char chCRCHi = 0xFF;
    unsigned char chCRCLo = 0xFF;
    short wIndex;

       while (wDataLen--)
       {
           // 计算CRC
           wIndex = chCRCLo ^ *pchMsg++ ;
           chCRCLo = chCRCHi ^ chCRCHTalbe[wIndex];
           chCRCHi = chCRCLTalbe[wIndex] ;
       }

       return ((chCRCHi << 8) | chCRCLo) ;
}


ScreenCheck::ScreenCheck(QWidget *parent) :
    QWidget(parent),

    ui(new Ui::ScreenCheck)
{
    ui->setupUi(this);
    this->worker = new Worker;
    this->visionwork = new VisionWorker(HKCamera::getInterface(0));
    this->visionwork->setWinid(this->ui->preview->winId());
    this->worker_thread = new QThread;
    this->camera1_thread = new QThread;
    worker->moveToThread(worker_thread);
    this->visionwork->moveToThread(this->camera1_thread);

    qRegisterMetaType<SerialSetting>("SerialSetting");
    qRegisterMetaType<SerialSetting>("SerialSetting&");
    qRegisterMetaType<Status>("Status");
    qRegisterMetaType<Status>("Status&");
    qRegisterMetaType<Command>("Command");
    qRegisterMetaType<Command>("Command&");
    qRegisterMetaType<HObject>("HObject");
    qRegisterMetaType<HObject>("HObject&");
    qRegisterMetaType<Hlong>("Hlong");
    qRegisterMetaType<Hlong>("Hlong&");
    qRegisterMetaType<Error>("Error");
    qRegisterMetaType<Error>("Error&");

    this->connect(this, &ScreenCheck::tell_worker_start_work, this->worker, &Worker::accept_start_worker,Qt::QueuedConnection);
    this->connect(this, &ScreenCheck::tell_worker_stop_work, this->worker, &Worker::accept_stop_work,Qt::QueuedConnection);
    this->connect(this, &ScreenCheck::tell_worker_open_serial, this->worker, &Worker::accept_open_serial,Qt::QueuedConnection);
    this->connect(this, &ScreenCheck::tell_worker_stm_command, this->worker, &Worker::accept_command_to_stm,Qt::QueuedConnection);

    this->connect(this->worker, &Worker::tell_window_command,this, &ScreenCheck::accept_stm_command,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_stm_status,this, &ScreenCheck::accept_stm_status,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_work_step,this, &ScreenCheck::accept_worker_step,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_serial_status,this, &ScreenCheck::accept_worker_serial_status,Qt::QueuedConnection);
    this->connect(this->worker, &Worker::tell_window_stm_respond_timeout,this, &ScreenCheck::accept_stm_respond_timeout,Qt::QueuedConnection);
    this->connect(this,&ScreenCheck::tell_camera1_run,this->visionwork,&VisionWorker::accept_ImageCapture_Check,Qt::QueuedConnection);


    this->worker_thread->start();
    this->camera1_thread->start();
    qpixmap = new QPixmap;
    pixitem = new PixItem(qpixmap);
    graph_scene = new QGraphicsScene;
    graph_view_preview = new QGraphicsView;
    graph_view_preview->setScene(graph_scene);
    graph_scene->addItem(pixitem);

    this->ui->pushButton_7->setChecked(false);
}

ScreenCheck::~ScreenCheck()
{
    delete qpixmap;
    delete ui;
    delete graph_view_preview;
    worker_thread->quit();
    worker_thread->wait();
    delete worker_thread;
    camera1_thread->quit();
    camera1_thread->wait();
    delete camera1_thread;
    bool exist = QFile::exists("./database/hobject/good.bmp");
    if (exist)
        QFile::remove("./database/hobject/good.bmp");
}


void ScreenCheck::addlabel(const QString &name, const QString &content)
{
    this->object_label_map[name] = new Label(this);
    this->object_label_map[name]->setText(content);
}

void ScreenCheck::start_check()
{
    emit ask_serial_setting();
    emit tell_worker_open_serial(this->serial_setting);
}

int ScreenCheck::getMaxID()
{
    int id;
    QSqlDatabase *database = DB::getInterface();
    if (!database->open())
    {
        qDebug() << "Error: Failed to connect database." << database->lastError();
        return -1;
    }
    else
    {
        QSqlQuery query(*database);
        QString temp = QString("SELECT max(product_id) FROM checked_result;");
        if (!query.exec(temp))
        {
            qDebug() << "query error" << database->lastError();
            return -2;
        }
        while (query.next())
        {
            QSqlRecord rec = query.record();
            id = rec.value(0).toInt();
        }
        return id;
    }
}

void ScreenCheck::HKInfoCallBack(const MV_FRAME_OUT_INFO &SInfo)
{
    qDebug() << "nLostPacket" << SInfo.nLostPacket;
    this->ui->label_12->setText(QString("%1").arg(SInfo.nLostPacket));
}

void ScreenCheck::setMen(const Meninfo &info)
{
    this->men_info = info;
}


void ScreenCheck::on_pushButton_4_released()
{
    emit tell_window_step_page(1);
    emit tell_worker_stop_work();
}


void ScreenCheck::setSerial(QSerialPort *serial_)
{
    this->serial = serial_;
}


void ScreenCheck::setDriversetting(DriveSeting *drivesetting)
{
    this->drivesetting = drivesetting;
}

void ScreenCheck::save_check_result(int product_id)
{
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "Error: Failed to connect database." << db->lastError();
        return;
    }
    QSqlQuery query(*db);
    QString str = QString("INSERT INTO checked_result (product_id,check_result,wrap_result,scratch,white_point,black_point,time,men_id,men_name) VALUES (\"%1\", \"%2\", \"%3\", \"%4\", \"%5\", \"%6\", \"%7\", \"%8\", \"%9\" );")
            .arg(product_id)
            .arg(current_check_result.isQualified_s)
            .arg(current_check_result.wrapedOver_s)
            .arg(current_check_result.scratch)
            .arg(current_check_result.whitePoint)
            .arg(current_check_result.blackPoint)
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
            .arg(men_info.number)
            .arg(men_info.name);

    if (!query.exec(str)){
          qDebug() << "insert failed." << db->lastError();
          db->close();
          return;
    }
    else
        qDebug() << "insert successed";
    db->close();
}

QLabel *ScreenCheck::camera_label()
{
    return this->ui->preview;
}

void ScreenCheck::ImageCapture()
{
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->collectFrame(this->ui->preview->winId()))
        return;
    ho_Image = camera->getImage();
    deal_Image = ho_Image;
    Hlong winid = (Hlong)this->ui->preview->winId();
}

bool ScreenCheck::workerSerialIsOpen()
{
    emit ask_worker_serial_status();
    msleep(1);
    return this->worker_thread_serial_status;
}

void ScreenCheck::analysis_FaultCode(QVariant data, QString &str)
{
    qDebug() << data.toByteArray();
    uint8_t stm_cylinder = (uint8_t)data.toByteArray().at(1);
    uint8_t stm_status = (uint8_t)data.toByteArray().at(0);
    Status status;
    qDebug() << stm_cylinder << stm_status;
    status.transportMotorSpeedStatus = (stm_status & 0x01) == 0x01 ? true : false;
    status.rollMontorSpeedStatus = (stm_status & 0x02) == 0x02 ? true : false;
    status.rollerMotorSpeedStatus = (stm_status & 0x04) == 0x04 ? true : false;
    status.slidingTableMotorSpeedStatus = (stm_status & 0x08) == 0x08 ? true : false;
    status.packingbag_fault = (stm_status & 0x10) == 0x10 ? true : false;
    status.material_buffer_cylinde = (stm_status & 0x20) == 0x20 ? true : false;

    status.grab_cylinder = (stm_cylinder & 0x01) == 0x01 ? true : false;
    status.magnetic_roller_positioning_cylinder = (stm_cylinder & 0x02) == 0x02 ? true : false;
    status.magnetic_roller_lifting_cylinder = (stm_cylinder & 0x04) == 0x04 ? true : false;
    status.slider_lift_cylinder = (stm_cylinder & 0x08) == 0x08 ? true : false;
    status.sucker_cylinder = (stm_cylinder & 0x10) == 0x10 ? true : false;
    status.pressure_cylinder = (stm_cylinder & 0x20) == 0x20 ? true : false;
    status.cutting_cylinder = (stm_cylinder & 0x40) == 0x40 ? true : false;
    status.waste_cylinder = (stm_cylinder & 0x80) == 0x80 ? true : false;
    if (status.packingbag_fault)
        str.append(QString::fromLocal8Bit("包装袋故障（堵住） \n"));
    if (status.transportMotorSpeedStatus)
        str.append(QString::fromLocal8Bit("传动电机故障, 先断电排查\n"));
    if (status.rollMontorSpeedStatus)
        str.append(QString::fromLocal8Bit("滚动电机故障, 先断电排查\n"));
    if (status.rollerMotorSpeedStatus)
        str.append(QString::fromLocal8Bit("滚筒电机故障, 先断电排查\n"));
    if (status.slidingTableMotorSpeedStatus)
        str.append(QString::fromLocal8Bit("滑台电机故障, 先断电排查\n"));
    if (status.grab_cylinder)
        str.append(QString::fromLocal8Bit("抓取气缸故障, 先检查气压是否正常\n"));
    if (status.magnetic_roller_positioning_cylinder)
        str.append(QString::fromLocal8Bit("磁辊定位气缸故障, 先检查气压是否正常\n"));
    if (status.magnetic_roller_lifting_cylinder)
        str.append(QString::fromLocal8Bit("磁辊抬升气缸故障, 先检查气压是否正常\n"));
    if (status.slider_lift_cylinder)
        str.append(QString::fromLocal8Bit("滑台抬升气缸故障, 先检查气压是否正常\n"));
    if (status.sucker_cylinder)
        str.append(QString::fromLocal8Bit("吸盘气缸故障, 先检查气压是否正常\n"));
    if (status.pressure_cylinder)
        str.append(QString::fromLocal8Bit("压料气缸故障, 先检查气压是否正常\n"));
    if (status.cutting_cylinder)
        str.append(QString::fromLocal8Bit("切料气缸故障, 先检查气压是否正常\n"));
    if (status.waste_cylinder)
        str.append(QString::fromLocal8Bit("废料气缸故障, 先检查气压是否正常\n"));
    if (status.material_buffer_cylinde)
        str.append(QString::fromLocal8Bit("物料缓冲气缸故障, 先检查气压是否正常\n"));
    this->ui->motor_1->setChecked(!status.transportMotorSpeedStatus);
    this->ui->motor_2->setChecked(!status.rollMontorSpeedStatus);
    this->ui->motor_3->setChecked(!status.rollerMotorSpeedStatus);
    this->ui->motor_4->setChecked(!status.slidingTableMotorSpeedStatus);

    this->ui->cylinder_1->setChecked(!status.grab_cylinder);
    this->ui->cylinder_2->setChecked(!status.magnetic_roller_positioning_cylinder);
    this->ui->cylinder_3->setChecked(!status.magnetic_roller_lifting_cylinder);
    this->ui->cylinder_4->setChecked(!status.slider_lift_cylinder);
    this->ui->cylinder_5->setChecked(!status.sucker_cylinder);
    this->ui->cylinder_6->setChecked(!status.pressure_cylinder);
    this->ui->cylinder_7->setChecked(!status.cutting_cylinder);
    this->ui->cylinder_8->setChecked(!status.waste_cylinder);

    if(this->ui->motor_1->isChecked()) this->ui->motor_1->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_1->setText("\346\225\205\351\232\234");
    if(this->ui->motor_2->isChecked()) this->ui->motor_2->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_2->setText("\346\225\205\351\232\234");
    if(this->ui->motor_3->isChecked()) this->ui->motor_3->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_3->setText("\346\225\205\351\232\234");
    if(this->ui->motor_4->isChecked()) this->ui->motor_4->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_4->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_1->isChecked()) this->ui->cylinder_1->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_1->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_2->isChecked()) this->ui->cylinder_2->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_2->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_3->isChecked()) this->ui->cylinder_3->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_3->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_4->isChecked()) this->ui->cylinder_4->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_4->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_5->isChecked()) this->ui->cylinder_5->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_5->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_6->isChecked()) this->ui->cylinder_6->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_6->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_7->isChecked()) this->ui->cylinder_7->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_7->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_8->isChecked()) this->ui->cylinder_8->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_8->setText("\346\225\205\351\232\234");
}

void ScreenCheck::accept_stm_command(Command com, QVariant data)
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
    messageBox.setIcon(QMessageBox::Critical);
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);

    if (com == Command::ImageCapture)
    {
        msleep(950);
        this->ImageCapture();
    }
    else if (com == Command::WrapResult)
    {
        this->current_check_result.wrapedOver = data.toInt();
        if (current_check_result.wrapedOver) this->current_check_result.wrapedOver_s = "good"; else this->current_check_result.wrapedOver_s = "bad";
        if (!data.toInt()){
            this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/ng.png);");
            this->ui->label_9->setText(QString::fromLocal8Bit("包装失败"));
        }
        else {
            this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);image: url(:/image/ok.png);");
            this->ui->label_9->setText(QString::fromLocal8Bit("包装成功"));
        }
        this->save_check_result(this->getMaxID()+1);
        emit this->tell_result_update_data();
    }
    else if (com == Command::Reset){
        this->ui->label->setStyleSheet("color:black");
        this->ui->label->setText(QString::fromLocal8Bit("复位完成后,重新点击开始检测"));
    }
    else if (com == Command::StopWork){
        this->ui->label->setText(QString::fromLocal8Bit("下位机停止"));
        messageBox.setText(QString::fromLocal8Bit("下位机停止"));
        messageBox.exec();
    }
    else if (com == Command::ContinueWork){
        this->ui->label->setStyleSheet("color:black");
        this->ui->label->setText(QString::fromLocal8Bit("开始工作"));
    }
    else if (com == Command::SuspendWork){
        this->ui->label->setText(QString::fromLocal8Bit("暂停中"));
        messageBox.setText(QString::fromLocal8Bit("下位机暂停按钮按下，先把按钮置为开始状态"));
        messageBox.exec();
    }
    else if (com == Command::EmergencyStop){
        this->ui->label->setStyleSheet("color:red");
        this->ui->label->setText(QString::fromLocal8Bit("下位机急停,先断电，断气检查机器"));
        messageBox.setText(QString::fromLocal8Bit("下位机急停,先断电，断气检查机器"));
        messageBox.exec();
    }
    else if (com == Command::STMReportFault) {
        QString str1;
        qDebug() << data.toByteArray().toHex();
        this->analysis_FaultCode(data,str1);
        if (str1.isEmpty()){qDebug() << "get none fault return"; return;}
        this->ui->label->setText(QString::fromLocal8Bit("下位机上报故障，已经停止工作，先排除故障后重新开始"));
        messageBox.setText(str1);
        messageBox.exec();
    }
    else if (com == Command::PackingBagLess) {
        this->ui->label->setText(QString::fromLocal8Bit("缺少包装袋"));
        messageBox.setText(QString::fromLocal8Bit("缺少包装袋"));
        messageBox.exec();
    }
}

void ScreenCheck::accept_detect_result(int result, int itype, HObject deal_image)
{
    current_check_result.isOK = result;
    if (current_check_result.isOK == 0){
        current_check_result.isQualified_s = "ng";
        current_check_result.scratch = (itype & 0x01) == 0x01 ? "true" : "false";
        current_check_result.whitePoint = (itype & 0x02) == 0x02 ? "true" : "false";
        current_check_result.blackPoint = (itype & 0x04) == 0x04 ? "true" : "false";
    }
    else if (current_check_result.isOK == 2) {
        current_check_result.isQualified_s = "good";
        current_check_result.scratch = (itype & 0x01) == 0x01 ? "true" : "false";
        current_check_result.whitePoint = (itype & 0x02) == 0x02 ? "true" : "false";
        current_check_result.blackPoint = (itype & 0x04) == 0x04 ? "true" : "false";
    }
    else if (current_check_result.isOK == 1){
        current_check_result.isQualified_s = "ok";
        current_check_result.scratch = (itype & 0x01) == 0x01 ? "true" : "false";
        current_check_result.whitePoint = (itype & 0x02) == 0x02 ? "true" : "false";
        current_check_result.blackPoint = (itype & 0x04) == 0x04 ? "true" : "false";
    }
    if (current_check_result.isOK == 1){
        if (this->ui->checkBox_2->isChecked()){
            this->ok_count += 1;
            this->all_count += 1;
        }
        this->ui->label_5->setText(QString::fromLocal8Bit("低端"));
        this->filename = QDateTime::currentDateTime().toString("hhmmss_%1").arg("OK");
    }
    else if (current_check_result.isOK == 2){
        if (this->ui->checkBox_2->isChecked()){
            this->good_count += 1;
            this->all_count += 1;
        }
        this->ui->label_5->setText(QString::fromLocal8Bit("高端"));
        this->filename = QDateTime::currentDateTime().toString("hhmmss_%1").arg("GOOD");
    }
    else if(current_check_result.isOK == 0) {
        if (this->ui->checkBox_2->isChecked()){
            this->all_count += 1;
            this->ng_count += 1;
        }
        this->ui->label_5->setText(QString::fromLocal8Bit("次品"));
        this->filename = QDateTime::currentDateTime().toString("hhmmss_%1").arg("NG");
    }

    if(this->ui->checkBox->isChecked() && this->ui->checkBox_3->isChecked() && this->ui->checkBox_4->isChecked()){

        QDir good_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/GOOD").absolutePath());
        HTuple hv_name1 = filename.toStdString().c_str();
        if (good_image_path.exists())
        {
            qDebug() << "hello ";
            WriteImage(ho_Image, "bmp", 0, HTuple(QString(good_image_path.absolutePath()+"/").toStdString().c_str()) + hv_name1);
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片成功"));
        }
        else {
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片失败"));
        }

    }

    else if(this->ui->checkBox->isChecked() && this->ui->checkBox_3->isChecked() && this->ui->checkBox_5->isChecked()){
        QDir ok_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/OK").absolutePath());
        HTuple hv_name1 = filename.toStdString().c_str();
        if (ok_image_path.exists())
        {
            WriteImage(ho_Image, "bmp", 0, HTuple(QString(ok_image_path.absolutePath()+"/").toStdString().c_str()) + hv_name1);
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片成功"));
        }
        else {
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片失败"));
        }
    }

    else if(this->ui->checkBox->isChecked() && this->ui->checkBox_3->isChecked() && this->ui->checkBox_6->isChecked()){
        QDir ng_image_path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/NG").absolutePath());
        HTuple hv_name1 = filename.toStdString().c_str();
        if (ng_image_path.exists())
        {
            WriteImage(ho_Image, "bmp", 0, HTuple(QString(ng_image_path.absolutePath()+"/").toStdString().c_str()) + hv_name1);
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片成功"));
        }
        else {
            this->ui->label_8->setText(QString::fromLocal8Bit("保存图片失败"));
        }
    }
    if(this->ui->checkBox_7->isChecked()){
        QDir path(QDir("D:/qt_photo/"+QDate::currentDate().toString("yyyyMMdd")+"/DEAL/").absolutePath());
        QScreen *screen = QGuiApplication::primaryScreen();
        QPixmap pixmap = screen->grabWindow(this->ui->preview->winId());
        pixmap.save(path.filePath(filename+".bmp"),"BMP");
    }
    this->ui->label_14->setNum(this->ok_count);
    this->ui->label_16->setNum(this->ng_count);
    this->ui->label_18->setNum(this->good_count);
    this->ui->stackedWidget->setCurrentIndex(1);

    uint8_t send_data = 0;
    if (this->current_check_result.isOK == 0)
        send_data = 2;
    else if (this->current_check_result.isOK == 2) {
        send_data = 1;
    }
    else if (this->current_check_result.isOK == 1) {
        send_data = 0;
    }
    emit tell_worker_stm_command(Command::CheckResult,send_data);
}

void ScreenCheck::accept_stm_respond_timeout()
{
    this->ui->label->setText(QString::fromLocal8Bit("下位机响应超时"));
    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
    messageBox.setIcon(QMessageBox::Critical);
    messageBox.setText(QString::fromLocal8Bit("下位机响应超时, 已停止工作 \n请查明并排除故障后,重新操作"));//\344\270\213\344\275\215\346\234\272\345\223\215\345\272\224\350\266\205\346\227\266，\345\267\262\347\273\217\345\201\234\346\255\242\345\267\245\344\275\234，\350\257\267\346\237\245\346\230\216\345\216\237\345\233\240\345\271\266\346\216\222\351\231\244\346\225\205\351\232\234\345\220\216\351\207\215\346\226\260\345\220\257\345\212\250"
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);
    messageBox.exec();
}

void ScreenCheck::accept_stm_status(Status status){

    this->ui->Estop_s->setChecked(!status.E_Stop);
    this->ui->motor_1->setChecked(!status.transportMotorSpeedStatus);
    this->ui->motor_2->setChecked(!status.rollMontorSpeedStatus);
    this->ui->motor_3->setChecked(!status.rollerMotorSpeedStatus);
    this->ui->motor_4->setChecked(!status.slidingTableMotorSpeedStatus);

    this->ui->cylinder_1->setChecked(!status.grab_cylinder);
    this->ui->cylinder_2->setChecked(!status.magnetic_roller_positioning_cylinder);
    this->ui->cylinder_3->setChecked(!status.magnetic_roller_lifting_cylinder);
    this->ui->cylinder_4->setChecked(!status.slider_lift_cylinder);
    this->ui->cylinder_5->setChecked(!status.sucker_cylinder);
    this->ui->cylinder_6->setChecked(!status.pressure_cylinder);
    this->ui->cylinder_7->setChecked(!status.cutting_cylinder);
    this->ui->cylinder_8->setChecked(!status.waste_cylinder);

    if(this->ui->Estop_s->isChecked()) this->ui->Estop_s->setText("\345\267\245\344\275\234\344\270\255"); else  this->ui->Estop_s->setText("\346\200\245\345\201\234\344\270\255");
    if(this->ui->motor_1->isChecked()) this->ui->motor_1->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_1->setText("\346\225\205\351\232\234");
    if(this->ui->motor_2->isChecked()) this->ui->motor_2->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_2->setText("\346\225\205\351\232\234");
    if(this->ui->motor_3->isChecked()) this->ui->motor_3->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_3->setText("\346\225\205\351\232\234");
    if(this->ui->motor_4->isChecked()) this->ui->motor_4->setText(QString::fromLocal8Bit("正常")); else  this->ui->motor_4->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_1->isChecked()) this->ui->cylinder_1->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_1->setText("\346\200\245\345\201\234\344\270\255");
    if(this->ui->cylinder_2->isChecked()) this->ui->cylinder_2->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_2->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_3->isChecked()) this->ui->cylinder_3->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_3->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_4->isChecked()) this->ui->cylinder_4->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_4->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_5->isChecked()) this->ui->cylinder_5->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_5->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_6->isChecked()) this->ui->cylinder_6->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_6->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_7->isChecked()) this->ui->cylinder_7->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_7->setText("\346\225\205\351\232\234");
    if(this->ui->cylinder_8->isChecked()) this->ui->cylinder_8->setText(QString::fromLocal8Bit("正常")); else  this->ui->cylinder_8->setText("\346\225\205\351\232\234");

    this->ui->motor_1_speed->setValue(status.transport_motor_speed);
    this->ui->motor_2_speed->setValue(status.roll_motor_speed);
    this->ui->motor_3_speed->setValue(status.roller_motor_speed);
    this->ui->motor_4_speed->setValue(status.slidingtable_motor_speed);
}

void ScreenCheck::accept_worker_step(int step)
{
    if (step == 0){
        if (HDevWindowStack::IsOpen())
            CloseWindow(HDevWindowStack::Pop());
        this->ui->label->setStyleSheet("color:black");
        this->ui->label_5->setText(QString::fromLocal8Bit("无"));
        this->ui->label_6->setStyleSheet("border:1px solid black;background-color: rgb(255, 255, 255);");
        this->ui->pushButton_7->setChecked(true);
        this->ui->label->setText("\345\207\206\345\244\207\345\267\245\344\275\234");
        this->ui->label->adjustSize();
        this->ui->stackedWidget->setCurrentIndex(0);
    }
    else if (step == 1) {
        this->ui->label->setStyleSheet("color:black");
        this->ui->pushButton_5->setChecked(true);
        this->ui->label->setText("\346\237\245\350\257\242\344\270\213\344\275\215\346\234\272\347\212\266\346\200\201");
        this->ui->label->adjustSize();
    }
    else if (step == 2) {
        this->ui->label->setStyleSheet("color:black");
        this->ui->pushButton_2->setChecked(true);
        this->ui->label->setText("\345\274\200\345\247\213\345\267\245\344\275\234\345\271\266\347\255\211\345\276\205\345\233\276\345\203\217\351\207\207\351\233\206");
        this->ui->label->adjustSize();
    }
    else if (step == 3) {
        this->ui->label->setStyleSheet("color:black");
        this->ui->pushButton->setChecked(true);
        this->ui->label->setText("\344\270\213\345\217\221\351\207\207\351\233\206\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }
    else if (step == 4) {
        this->ui->label->setStyleSheet("color:black");
        this->ui->pushButton_3->setChecked(true);
        this->ui->label->setText("\344\270\213\345\217\221\345\256\214\346\210\220\345\271\266\347\255\211\345\276\205\345\214\205\350\243\205\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }
    else if (step == 5) {
        this->ui->label->setStyleSheet("color:black");
        this->ui->pushButton_6->setChecked(true);
        this->ui->label->setText("\346\224\266\345\210\260\345\214\205\350\243\205\347\273\223\346\236\234");
        this->ui->label->adjustSize();
    }

}


void ScreenCheck::on_start_check_released()
{
    if (!this->camera->isOpened()) {
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(QString::fromLocal8Bit("先完成系统自检"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    camera->collectFrame(this->ui->preview->winId());
    emit ask_serial_setting();
    emit tell_worker_start_work(this->serial_setting);
    msleep(10);
    if (!this->worker_thread_serial_status && !camera->isOpened()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(QString::fromLocal8Bit("系统自检失败，先检查相机和串口"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    else if (!this->worker_thread_serial_status) {
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(QString::fromLocal8Bit("系统自检失败，先检查串口"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    else if (!this->camera->isOpened()) {
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Critical);
        messageBox.setText(QString::fromLocal8Bit("系统自检失败，先检查相机"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
}

//delete qpixmap;
//delete pixitem;
//qpixmap = new QPixmap("./database/hobject/" + this->filename + ".bmp");
//pixitem = new PixItem(qpixmap);

//graph_scene->clear();
//graph_scene->addItem(pixitem);
//graph_view_preview->resize(qpixmap->width(), qpixmap->height());
//graph_view_preview->show();

void ScreenCheck::on_pushButton_17_released()
{
    emit tell_worker_stm_command(Command::StopWork,0);
}

void ScreenCheck::on_pushButton_18_released()
{
    emit tell_worker_stm_command(Command::Reset,0);
}

void ScreenCheck::on_pushButton_8_released()
{
    this->ui->stackedWidget->setCurrentIndex(0);
}

void ScreenCheck::on_checkBox_3_stateChanged(int arg1)
{
    if(arg1){
        this->ui->checkBox->setChecked(true);
        this->ui->checkBox_2->setChecked(true);
        this->ui->checkBox_4->setChecked(true);
    }
}

void ScreenCheck::on_pushButton_9_released()
{
    QString filename;
    filename=QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(filename.isEmpty())
        return;
    ReadImage(&ho_Image, HTuple(filename.toStdString().c_str()));

    emit tell_camera1_run(drivesetting->get_algorithm(),this->ui->preview->width(),this->ui->preview->height());
}
