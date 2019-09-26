#ifndef SERIAL_H
#define SERIAL_H
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QComboBox>
#include <QDebug>
#include <QMutex>

static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "None");
static QSerialPort *serial = new QSerialPort;
struct SerialSetting {
    QString name;
    int baudRate;
    QString stringBaudRate;
    QSerialPort::DataBits dataBits;
    QString stringDataBits;
    QSerialPort::Parity parity;
    QString stringParity;
    QSerialPort::StopBits stopBits;
    QString stringStopBits;
    QSerialPort::FlowControl flowControl;
    QString stringFlowControl;
    bool localEchoEnabled;
};

class Serial
{
public:
    static QSerialPort *getInterface(){
        return serial;
    }
    static bool open(const SerialSetting &setting, QIODevice::OpenMode model);
    static void scan_serial(QComboBox * serialPortInfoListBox);
    static QByteArray get_serial_bin(const SerialSetting &setting);

private:
    Serial();

};
#endif // SERIAL_H
