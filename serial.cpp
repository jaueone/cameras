#include "serial.h"

bool Serial::open(const SerialSetting &setting, QIODevice::OpenMode model)
{
    if (serial->isOpen())
        serial->close();
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    return serial->open(model);
}

void Serial::scan_serial(QComboBox *serialPortInfoListBox)
{
    serialPortInfoListBox->clear();
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

        serialPortInfoListBox->addItem(list.first(), list);
        qDebug() << list;
    }
    serialPortInfoListBox->addItem("Custom");
}

QByteArray Serial::get_serial_bin(const SerialSetting &setting)
{
    QJsonObject serial
    {
        {"name",setting.name},
        {"baudRate",setting.stringBaudRate},
        {"dataBits",setting.stringDataBits},
        {"parity",setting.stringParity},
        {"stopBits",setting.stringStopBits},
        {"flowControl",setting.stringFlowControl},
    };
    QJsonDocument serial_doc = QJsonDocument(serial);
    QByteArray serial_data = serial_doc.toJson();
    return serial_data;
}

Serial::Serial()
{

}

