#include "driveseting.h"
#include "ui_driveseting.h"
#include "my_control.h"
#include "DefectsDetect.h"
#include <QMap>
#include <QString>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QSerialPort>


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

unsigned short int DriveSeting::CRC16(unsigned char *pchMsg, unsigned short int wDataLen)
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

DriveSeting::DriveSeting(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriveSeting)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    camera = HKCamera::getInterface(0);
    camera_2 = HKCamera::getInterface(1);

    this->ui->baudRateBox->setCurrentIndex(1);
    this->ui->dataBitsBox->setCurrentIndex(3);

    this->connect(this->ui->serialPortInfoListBox,&ComboBox::clicked, this, &DriveSeting::scan_serial);
    this->scan_serial();
    this->load_setting();

}

DriveSeting::~DriveSeting()
{
    delete ui;
}

bool DriveSeting::init()
{
    this->scan_serial();
    this->load_setting();

    if (this->init_camera() != "opened")
        this->Camera_OK = false;
    else
        this->Camera_OK = true;
    if (this->init_serial() != "opened")
        this->Serial_OK = false;
    else
        this->Serial_OK = true;

    if (Camera_OK && Serial_OK)
    {
        serial->close();
        return true;
    }
    else
    {
        serial->close();
        return false;
    }
}

void DriveSeting::display_init()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
    messageBox.setIcon(QMessageBox::Warning);
    QPushButton button(QString::fromLocal8Bit("确定"));
    messageBox.addButton(&button, QMessageBox::YesRole);
    if (!this->Camera_OK && !this->Serial_OK){
        messageBox.setText(QString::fromLocal8Bit("相机自检失败  \n串口自检失败   请联系管理员"));
        messageBox.exec();
    }
    else if (!this->Serial_OK){
        messageBox.setText(QString::fromLocal8Bit("串口自检失败   请联系管理员"));
        messageBox.exec();
    }
    else if (!this->Camera_OK){
        messageBox.setText(QString::fromLocal8Bit("相机自检失败   请联系管理员"));
        messageBox.exec();
    }
    else{
        messageBox.setWindowTitle("信息");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.setText(QString::fromLocal8Bit("系统自检成功"));
        messageBox.exec();
    }
}

QString DriveSeting::init_camera()
{
    if (MV_OK != camera->enumDevices())
        return "enum fail";
    if (MV_OK != camera->openDevice())
        return  "open fail";
    return "opened";
}

QString DriveSeting::init_serial()
{
    if (serial->isOpen())
        serial->close();
    SerialSetting setting = get_serial_setting();
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    if (serial->open(QIODevice::ReadWrite) && serial->isOpen()){
        return "opened";
    }
    return "opened fail";
}

SerialSetting DriveSeting::get_serial_setting()
{
    QMap<QString, int> map = {{"4800",QSerialPort::Baud4800},
                        {"9600",QSerialPort::Baud9600},
                        {"19200",QSerialPort::Baud19200},
                        {"38400",QSerialPort::Baud38400},
                        {"115200",QSerialPort::Baud115200},
                        {"5", QSerialPort::Data5},
                        {"6", QSerialPort::Data6},
                        {"7", QSerialPort::Data7},
                        {"8", QSerialPort::Data8},
                        {"None", QSerialPort::NoParity},
                        {"Even", QSerialPort::EvenParity},
                        {"Odd", QSerialPort::OddParity},
                        {"Mark", QSerialPort::MarkParity},
                        {"Space", QSerialPort::SpaceParity},
                        {"1", QSerialPort::OneStop},
                        {"1.5", QSerialPort::OneAndHalfStop},
                        {"2", QSerialPort::TwoStop},
                        {"None", QSerialPort::NoFlowControl},
                        {"RTS/CTS", QSerialPort::HardwareControl},
                        {"XON/XOFF", QSerialPort::SoftwareControl},
                       };

    SerialSetting setting;
    setting.name = this->ui->serialPortInfoListBox->currentText();

    setting.baudRate = static_cast<QSerialPort::BaudRate>(map[this->ui->baudRateBox->currentText()]);
    setting.stringBaudRate = QString::number(setting.baudRate);

    setting.dataBits = static_cast<QSerialPort::DataBits>(map[this->ui->dataBitsBox->currentText()]);
    setting.stringDataBits = this->ui->dataBitsBox->currentText();

    setting.parity = static_cast<QSerialPort::Parity>(map[this->ui->parityBox->currentText()]);
    setting.stringParity = this->ui->parityBox->currentText();

    setting.stopBits = static_cast<QSerialPort::StopBits>(map[this->ui->stopBitsBox->currentText()]);
    setting.stringStopBits = this->ui->stopBitsBox->currentText();

    setting.flowControl = static_cast<QSerialPort::FlowControl>(map[this->ui->flowControlBox->currentText()]);
    setting.stringFlowControl = this->ui->flowControlBox->currentText();

    return setting;
}



void DriveSeting::load_setting()
{
    QSqlDatabase *database = DB::getInterface();
    QJsonDocument ser_doc, cam_doc, cam2_doc,algori_doc;

    if (!database->open()) {
       qDebug() << "Error: Failed to connect database." << database->lastError();
       return;
    }
    else {
        QSqlQuery query(*database);
        QString temp = QString("SELECT data FROM config_table WHERE admin = \"%1\";").arg("admin");
        if (!query.exec(temp))
            return;

        query.next();
        ser_doc = QJsonDocument::fromJson(query.record().value("data").toByteArray());
        query.next();
        cam_doc = QJsonDocument::fromJson(query.record().value("data").toByteArray());
        query.next();
        cam2_doc = QJsonDocument::fromJson(query.record().value("data").toByteArray());
        query.next();
        algori_doc = QJsonDocument::fromJson(query.record().value("data").toByteArray());
        database->close();
    }

    QJsonObject ser_obj = ser_doc.object();

    this->ui->baudRateBox->setCurrentText(ser_obj["baudRate"].toString());
    this->ui->dataBitsBox->setCurrentText(ser_obj["dataBits"].toString());
    this->ui->parityBox->setCurrentText(ser_obj["parity"].toString());
    this->ui->stopBitsBox->setCurrentText(ser_obj["stopBits"].toString());
    this->ui->flowControlBox->setCurrentText(ser_obj["flowControl"].toString());
    this->ui->serialPortInfoListBox->setCurrentText(ser_obj["name"].toString());
    this->set_camera_json_setting(cam_doc.object());
    this->set_camera2_json_setting(cam2_doc.object());
    this->set_algorithmic_setting(algori_doc.object());
}


QJsonObject DriveSeting::get_algorithm()
{
    QJsonObject params;
    QList<QDoubleSpinBox*> list = this->ui->groupBox_8->findChildren<QDoubleSpinBox*>();
    for (auto object:list) {
        params[object->objectName()] = object->value();
    }
    return params;
}


void DriveSeting::scan_serial()
{
    this->ui->serialPortInfoListBox->clear();
    QString description;
    QString manufacturer;
    QString serialNumber;
    const auto infos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : infos) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);

        this->ui->serialPortInfoListBox->addItem(list.first(), list);
        qDebug() << list;
    }
    this->ui->serialPortInfoListBox->addItem(tr("Custom"));
}

void DriveSeting::save_serial_config()
{
    QString serial = Serial::get_serial_bin(this->get_serial_setting());
    QSqlDatabase *database = DB::getInterface();
    if (!database->open())
    {
        qDebug() << "Error: Failed to connect database." << database->lastError();
    }
    else
    {
        QSqlQuery query(*database);
        QString temp = QString::fromLocal8Bit("UPDATE config_table SET data = :data, time = :time WHERE type = :type;");
        query.prepare(temp);
        query.bindValue(":data",serial);
        query.bindValue(":time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":type","serial_config");
        if (query.exec()){
            QSqlRecord rec = query.record();
            qDebug() << "saved";
        }
        database->close();
    }
}

void DriveSeting::save_algorithmic_config()
{
    QJsonObject params;
    QSqlDatabase *db = DB::getInterface();
    if (!db->open()){
        qDebug() << "open database failed";
        return;
    }
    QList<QDoubleSpinBox*> list = this->ui->groupBox_8->findChildren<QDoubleSpinBox*>();
    for (auto object:list) {
        params[object->objectName()] = object->value();
    }
    QSqlQuery query(*db);
    QJsonDocument doc(params);
    QString temp = QString::fromLocal8Bit("UPDATE config_table SET data = :data, time = :time WHERE type = :type;");
    query.prepare(temp);
    query.bindValue(":data",doc.toJson());
    query.bindValue(":time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    query.bindValue(":type","algorithmic_parameters");

    if (!query.exec())
    {
        qDebug() << "exec failed";
    }
    else
    {
        qDebug() << "exec successed";
    }
    db->close();
}

void DriveSeting::check_self()
{
    this->init();
    this->display_init();
}

void DriveSeting::operation_open_camera(HKCamera *camera)
{
    qDebug() << 1;
    if (MV_OK != camera->enumDevices()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(QString::fromLocal8Bit("无法找到相机"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    qDebug() << 2;
    if (MV_OK != camera->openDevice()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(QString::fromLocal8Bit("相机打开失败"));
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.addButton(&button, QMessageBox::YesRole);
        messageBox.exec();
        return;
    }
    qDebug() << 3;
    if(camera->getIndex() == 0)
        this->ui->label_27->setText(QString::fromLocal8Bit("打开相机完成"));
    else if (camera->getIndex() == 1)
        this->ui->label_31->setText(QString::fromLocal8Bit("打开相机完成"));

}

void DriveSeting::operation_close_camera(HKCamera *camera)
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->stopCollect())
        return;
    if (MV_OK != camera->closeDevice())
        return;

    if(camera->getIndex() == 0){
        this->ui->label_18->clear();
        this->ui->label_27->setText(QString::fromLocal8Bit("关闭相机"));
    }
    else if (camera->getIndex() == 1)
    {
        this->ui->label_64->clear();
        this->ui->label_31->setText(QString::fromLocal8Bit("关闭相机"));
    }

}

void DriveSeting::operation_start_collect(HKCamera *camera)
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->startCollect())
        return;

    if(camera->getIndex() == 0){
        if (MV_OK != camera->collectFrame(this->ui->label_18->winId()))
            return;
        this->ui->label_27->setText(QString::fromLocal8Bit("开始预览"));
    }
    else if (camera->getIndex() == 1){
        if (MV_OK != camera->collectFrame(this->ui->label_64->winId()))
            return;
        this->ui->label_31->setText(QString::fromLocal8Bit("开始预览"));
    }
}

void DriveSeting::operation_stop_collect(HKCamera *camera)
{
    if (!camera->isOpened())
        return;
    if (MV_OK != camera->openDevice())
        return;
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->stopCollect())
        return;
    if(camera->getIndex() == 0)
        this->ui->label_27->setText(QString::fromLocal8Bit("停止抓图"));
    else if (camera->getIndex() == 1)
        this->ui->label_31->setText(QString::fromLocal8Bit("停止抓图"));

}

void DriveSeting::operation_collect_image(HKCamera *camera)
{
    QString save_name;

    if (MV_OK != camera->startCollect())
        return;

    if(camera->getIndex() == 0){
        save_name= QDateTime::currentDateTime().toString("camera1_yyyyMMddhhmmss");
        if (MV_OK != camera->collectFrame(this->ui->label_18->winId()))
            return;
    }
    else if (camera->getIndex() == 1){
        save_name= QDateTime::currentDateTime().toString("camera2_yyyyMMddhhmmss");
        if (MV_OK != camera->collectFrame(this->ui->label_64->winId()))
            return;
    }

    HTuple hv_name1 = save_name.toStdString().c_str();
    WriteImage(camera->getImage(), "bmp", 0, HTuple("./photo/") + hv_name1);
    if(camera->getIndex() == 0){
        this->ui->label_27->setText(QString::fromLocal8Bit("抓取一帧图片完成"));
    }
    else if (camera->getIndex() == 1){
        this->ui->label_31->setText(QString::fromLocal8Bit("抓取一帧图片完成"));
    }


}

void DriveSeting::operation_set_params(HKCamera *camera)
{
    if (!camera->isOpened()){
        HKCamera::camera_message_warn();
        return;
    }
    if (camera->isCollecting()){
        QMessageBox messageBox;
        messageBox.setWindowTitle(QString::fromLocal8Bit("警告"));
        messageBox.setIcon(QMessageBox::Warning);
        QPushButton button(QString::fromLocal8Bit("确定"));
        messageBox.setText(QString::fromLocal8Bit("先停止抓图在设置参数"));
        messageBox.exec();
        return;
    }
    camera->setParams(this->get_camera_setting_json());
    HKCamera::camera_message_done();
}

void DriveSeting::operation_get_params(HKCamera *camera)
{
    if(camera->getIndex() == 0){
        this->set_camera_json_setting(camera->getParams());
        this->ui->label_27->setText(QString::fromLocal8Bit("获取参数"));
    }
    else if (camera->getIndex() == 1){
        this->set_camera2_json_setting(camera->getParams());
        this->ui->label_31->setText(QString::fromLocal8Bit("获取参数"));
    }
}


void DriveSeting::operation_save_config(HKCamera *camera)
{
    QString str_camera,type_id;
    if (camera->getIndex() == 0){
        str_camera = QJsonDocument(this->get_camera_setting_json()).toJson();
        type_id = "camera1_config";

    }
    else if (camera->getIndex() == 1) {
        str_camera = QJsonDocument(this->get_camera2_setting_json()).toJson();
        type_id = "camera2_config";
    }
    QSqlDatabase *database = DB::getInterface();
    qDebug() << str_camera;
    if (!database->open()){
        qDebug() << "Error: Failed to connect database." << database->lastError();
    }
    else{
        QSqlQuery query(*database);
        QString temp = QString::fromLocal8Bit("UPDATE config_table SET data = :data, time = :time WHERE type = :type;");
        query.prepare(temp);
        query.bindValue(":data",str_camera);
        query.bindValue(":time",QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
        query.bindValue(":type",type_id);
        if (query.exec()){
            QSqlRecord rec = query.record();
            qDebug() << "saved";
        }
        database->close();
    }
    DMessageBox::box(QMessageBox::Information,QString::fromLocal8Bit("信息"),QString::fromLocal8Bit("参数保存成功"));
}

void DriveSeting::accept_return_serial_setting()
{
    emit tell_screencheck_setting(this->get_serial_setting());
}


QSerialPort *DriveSeting::Serial()
{
    return serial;
}


void DriveSeting::accept_serial_status(bool isopened)
{
    this->worker_serial_status = isopened;
    if (isopened){
        this->ui->pushButton_7->setChecked(true);
        this->ui->pushButton_7->setText(QString::fromLocal8Bit("关闭串口"));
    }
    else {
        this->ui->pushButton_7->setChecked(false);
        this->ui->pushButton_7->setText(QString::fromLocal8Bit("打开串口"));
    }
}

QJsonObject DriveSeting::get_camera_setting_json()
{
    QJsonObject camera1;
    for (auto item:this->ui->widget->findChildren<QSpinBox*>()) {
        camera1[item->objectName()] = item->value();
    }
    for (auto item:this->ui->widget->findChildren<QDoubleSpinBox*>()) {
        camera1[item->objectName()] = item->value();
    }
    for (auto item:this->ui->widget->findChildren<QCheckBox*>()) {
        camera1[item->objectName()] = item->isChecked();
    }
    for (auto item:this->ui->widget->findChildren<QComboBox*>()) {
        camera1[item->objectName()] = item->currentText();
    }
    qDebug() <<camera1;
    return camera1;
}

QJsonObject DriveSeting::get_camera2_setting_json()
{
    QJsonObject camera2;
    for (auto item:this->ui->widget_3->findChildren<QSpinBox*>()) {
        camera2[item->objectName()] = item->value();
    }
    for (auto item:this->ui->widget_3->findChildren<QDoubleSpinBox*>()) {
        camera2[item->objectName()] = item->value();
    }
    for (auto item:this->ui->widget_3->findChildren<QCheckBox*>()) {
        camera2[item->objectName()] = item->isChecked();
    }
    for (auto item:this->ui->widget_3->findChildren<QComboBox*>()) {
        camera2[item->objectName()] = item->currentText();
    }

   qDebug() <<camera2;
    return camera2;
}

void DriveSeting::set_camera_json_setting(QJsonObject json)
{
    if (json.isEmpty())
        return;
    for (auto item:this->ui->widget->findChildren<QSpinBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setValue(json[item->objectName()].toInt());
    }
    for (auto item:this->ui->widget->findChildren<QDoubleSpinBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setValue(json[item->objectName()].toInt());
    }
    for (auto item:this->ui->widget->findChildren<QCheckBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setChecked(json[item->objectName()].toBool());
    }
    for (auto item:this->ui->widget->findChildren<QComboBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setCurrentText(json[item->objectName()].toString());
    }
}

void DriveSeting::set_camera2_json_setting(QJsonObject json)
{
    if (json.isEmpty())
        return;
    for (auto item:this->ui->widget_3->findChildren<QSpinBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setValue(json[item->objectName()].toInt());
    }
    for (auto item:this->ui->widget_3->findChildren<QDoubleSpinBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setValue(json[item->objectName()].toInt());
    }
    for (auto item:this->ui->widget_3->findChildren<QCheckBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setChecked(json[item->objectName()].toBool());
    }
    for (auto item:this->ui->widget_3->findChildren<QComboBox*>()) {
        if (json[item->objectName()].isNull()) continue;
        item->setCurrentText(json[item->objectName()].toString());
    }
}

void DriveSeting::set_algorithmic_setting(QJsonObject json)
{
    if (json.isEmpty())
        return;
    for (auto object:this->ui->groupBox_8->findChildren<QDoubleSpinBox*>()) {
        if(!json[object->objectName()].toDouble())
            continue;
        object->setValue(json[object->objectName()].toDouble());
    }
}


void DriveSeting::on_serialPortInfoListBox_currentIndexChanged(int index)
{
    if (index == -1)
        return;
    const bool isCustomPath = !this->ui->serialPortInfoListBox->itemData(index).isValid();
    this->ui->serialPortInfoListBox->setEditable(isCustomPath);
    if (isCustomPath)
        this->ui->serialPortInfoListBox->clearEditText();
    const QStringList list = this->ui->serialPortInfoListBox->itemData(index).toStringList();
    this->ui->descriptionLabel->setText(tr("Description: %1").arg(list.count() > 1 ? list.at(1) : tr(blankString)));
    this->ui->manufacturerLabel->setText(tr("Manufacturer: %1").arg(list.count() > 2 ? list.at(2) : tr(blankString)));
    this->ui->serialNumberLabel->setText(tr("Serial number: %1").arg(list.count() > 3 ? list.at(3) : tr(blankString)));
    this->ui->locationLabel->setText(tr("Location: %1").arg(list.count() > 4 ? list.at(4) : tr(blankString)));
    this->ui->vidLabel->setText(tr("Vendor Identifier: %1").arg(list.count() > 5 ? list.at(5) : tr(blankString)));
    this->ui->pidLabel->setText(tr("Product Identifier: %1").arg(list.count() > 6 ? list.at(6) : tr(blankString)));
}

void DriveSeting::on_pushButton_released()
{
    emit tell_window_step_page(1);
}


void DriveSeting::on_pushButton_6_released()
{
    this->save_algorithmic_config();
}


void DriveSeting::on_pushButton_5_released()
{
    this->on_pushButton_released();
}


void DriveSeting::on_pushButton_7_released()
{
    this->save_serial_config();
}

void DriveSeting::on_pushButton_2_released()
{
    this->operation_save_config(this->camera);
}

void DriveSeting::on_pushButton_9_released()
{
    this->operation_open_camera(this->camera);
}

void DriveSeting::on_pushButton_17_released()
{
    this->operation_close_camera(this->camera);
}

void DriveSeting::on_pushButton_10_released()
{
    this->operation_get_params(this->camera);
}

void DriveSeting::on_pushButton_12_released()
{
    this->operation_set_params(this->camera);
}

void DriveSeting::on_pushButton_18_released()
{
    this->operation_start_collect(this->camera);
}

void DriveSeting::on_pushButton_14_released()
{
    this->operation_stop_collect(this->camera);
}

void DriveSeting::on_pushButton_11_clicked()
{
    this->operation_collect_image(this->camera);
}



void DriveSeting::on_pushButton_4_released()
{
    this->operation_save_config(this->camera_2);
}

void DriveSeting::on_pushButton_31_released()
{
    this->operation_open_camera(this->camera_2);
}

void DriveSeting::on_pushButton_30_released()
{
    this->operation_close_camera(this->camera_2);
}

void DriveSeting::on_pushButton_32_released()
{
    this->operation_set_params(this->camera_2);
}

void DriveSeting::on_pushButton_33_released()
{
    this->operation_get_params(this->camera_2);
}

void DriveSeting::on_pushButton_34_released()
{
    this->operation_start_collect(this->camera_2);
}

void DriveSeting::on_pushButton_29_released()
{
    this->operation_stop_collect(this->camera_2);
}

void DriveSeting::on_pushButton_36_released()
{
    this->operation_collect_image(this->camera_2);
}


