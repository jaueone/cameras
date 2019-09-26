#include "worker.h"
#include "serial.h"


void sleep(unsigned int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void Worker::init_setting_and_connect()
{
    this->timer->setInterval(12000);
    this->status.E_Stop = false;
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    this->connect(this->serial,&QSerialPort::readyRead,this,&Worker::accept_read_data);
    this->connect(this->timer,&QTimer::timeout,this,&Worker::accept_timeout);
    this->connect(this->serial,&QSerialPort::errorOccurred, this,&Worker::accept_serial_error);
}

void Worker::query_status()
{
    qDebug() << this->is_Stoped_Work;
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 0)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::QueryStatus,data));
    qDebug() <<"send query"<< Worker::dump_data(Command::QueryStatus,data).toHex();
    timer->start();
}

void Worker::start_work()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 1)
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::StartWork,data));
    timer->start();
}

void Worker::response_check_command()
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 2)
        return;
    QByteArray data;
    char c;
    if (this->quality < 9)
        c = 0x01;
    else
        c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::ImageCapture,data));
    if (this->quality >= 9)
        this->quality = 0;
    this->quality += 1;
}


void Worker::response_wrap_result(const QByteArray &data)
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != 4)
        return;
    serial->write(Worker::dump_data(Command::WrapResult,data));
}

void Worker::response_continue_work()
{
    if(!serial->isOpen())
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::ContinueWork,data));
}

void Worker::response_suspend_work()
{
    if(!serial->isOpen())
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::SuspendWork,data));
}

void Worker::response_stop_work()
{
    if(!serial->isOpen())
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::StopWork,data));
}

void Worker::response_reset_work()
{
    if(!serial->isOpen())
        return;
    QByteArray data;
    char c = 0x00;
    data.append(c);
    serial->write(Worker::dump_data(Command::Reset,data));
}

Respond Worker::load_data(const QByteArray &data)
{
    Respond respond;
    if (data.length() <10)
         return respond;
    QByteArray ba(data);
    Package package;
    QDataStream in(&ba,QIODevice::ReadOnly);
    in >> package.head >> package.packageNumber >>package.packageLength >> package.commandCount >>package.command >> package.length;
    if (package.head != 0xFDFD)
        return respond;
    if(package.packageLength != data.size())
        return respond;
    if (package.command !=0){
        Command com = static_cast<Command>(package.command);
        if (!com) return respond;
    }
    uint8_t d;
    QByteArray data_;
    for(int i=0;i<package.length;i++){
        in >> d;
        data_.append((char)d);
    }
    in >> package.crc;
    uint16_t crc=0;
    for (int i = 0; i < 8+package.length; i++) {
        crc += (uint8_t)data.at(i);
    }
    if (crc != package.crc)
        return respond;
    respond.isGood = true;
    respond.command = (Command)package.command;
    respond.data = data_;
    return respond;
}

void Worker::set_motor_speed(Command com, uint16_t data)
{
    if(this->is_Stoped_Work)
        return;
    if(!serial->isOpen())
        return;
    if (this->step != -1)
        return;
    QByteArray da;
    QDataStream d(&da,QIODevice::ReadWrite);
    if (com == 1){
        d << status_speed.transport_motor_speed;
        serial->write(Worker::dump_data(Command::SetTransportMotorSpeed,da));
    }
    else if (com == 2){
        d << data;
        serial->write(Worker::dump_data(Command::SetRollMontorSpeed,da));
    }
    else if (com == 3){
        d << data;
        serial->write(Worker::dump_data(Command::SetRollerMotorSpeed,da));
    }
    else if (com == 4){
        d << data;
        serial->write(Worker::dump_data(Command::SetSlidingTableMotorSpeed,da));
    }
    timer->start();
}

void Worker::analysis_MCStatus(const QByteArray &data)
{
    QByteArray ba(data);
    qDebug() << "this" << data.toHex();
    QDataStream in(&ba,QIODevice::ReadOnly);
    uint8_t stm_cylinder;
    uint8_t stm_status;
    stm_status = (uint8_t)data.at(0);
    stm_cylinder = (uint8_t)data.at(1);

    status.E_Stop = (stm_status & 0x01) == 0x01 ? true : false;
    status.transportMotorSpeedStatus = (stm_status & 0x02) == 0x02 ? true : false;
    status.rollMontorSpeedStatus = (stm_status & 0x04) == 0x04 ? true : false;
    status.rollerMotorSpeedStatus = (stm_status & 0x08) == 0x08 ? true : false;
    status.slidingTableMotorSpeedStatus = (stm_status & 0x10) == 0x10 ? true : false;
    status.isWorking = (stm_status & 0x20) == 0x20 ? true : false;
    status.isReseting = (stm_status & 0x40) == 0x40 ? true : false;
    status.isStoped = (stm_status & 0x80) == 0x80 ? true : false;

    status.grab_cylinder = (stm_cylinder & 0x01) == 0x01 ? true : false;
    status.magnetic_roller_positioning_cylinder = (stm_cylinder & 0x02) == 0x02 ? true : false;
    status.magnetic_roller_lifting_cylinder = (stm_cylinder & 0x04) == 0x04 ? true : false;
    status.slider_lift_cylinder = (stm_cylinder & 0x08) == 0x08 ? true : false;
    status.sucker_cylinder = (stm_cylinder & 0x10) == 0x10 ? true : false;
    status.pressure_cylinder = (stm_cylinder & 0x20) == 0x20 ? true : false;
    status.cutting_cylinder = (stm_cylinder & 0x40) == 0x40 ? true : false;
    status.waste_cylinder = (stm_cylinder & 0x80) == 0x80 ? true : false;

    in >> stm_status >> status.transport_motor_speed >> status.roll_motor_speed >> status.roller_motor_speed >> status.slidingtable_motor_speed >> stm_cylinder;


    qDebug() << status.E_Stop << status.transportMotorSpeedStatus << status.rollMontorSpeedStatus << status.rollerMotorSpeedStatus
             << status.slidingTableMotorSpeedStatus << status.isWorking << status.isReseting << status.isStoped
             << status.transport_motor_speed << status.roll_motor_speed << status.roller_motor_speed << status.slidingtable_motor_speed
             << status.grab_cylinder << status.magnetic_roller_positioning_cylinder << status.magnetic_roller_lifting_cylinder << status.slider_lift_cylinder
             << status.sucker_cylinder << status.pressure_cylinder << status.cutting_cylinder << status.waste_cylinder;
    if (status.E_Stop){
        timer->stop();
        this->is_Stoped_Work = true;
    }
    emit tell_window_stm_status(status);
}
void Worker::analysis_FaultCode(const QByteArray &data)
{
    QByteArray by(data);
    uint8_t stm_status = (uint8_t)by.at(0);
    uint8_t stm_cylinder = (uint8_t)by.at(1);
    qDebug() << stm_cylinder << stm_status << by.toHex();
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
    qDebug() << status.transportMotorSpeedStatus << status.rollMontorSpeedStatus << status.rollerMotorSpeedStatus << status.slidingTableMotorSpeedStatus
             << status.grab_cylinder << status.magnetic_roller_positioning_cylinder << status.magnetic_roller_lifting_cylinder << status.slider_lift_cylinder
             << status.sucker_cylinder << status.pressure_cylinder << status.cutting_cylinder << status.waste_cylinder;
    emit tell_window_stm_status(status);
}

QByteArray Worker::dump_data(const Command &command,const QByteArray &data)
{
    QByteArray ba;
    QDataStream out(&ba,QIODevice::WriteOnly);
    uint8_t len = (uint8_t)data.length();
    Package package;
    package.head = 0xFDFD;
    package.packageNumber = 0x00;
    package.packageLength = 10 + len;
    package.commandCount = 1;
    package.command = (uint16_t)command;
    package.length = len;
    out << package.head << package.packageNumber << package.packageLength << package.commandCount <<package.command << package.length;
    for (auto d:data){
        out << (uint8_t)d;
    }
    for(auto c:ba){
        package.crc += (uint8_t)c;
    }
    out << package.crc;
    return ba;
}

void Worker::accept_read_data()
{
    QByteArray data = this->serial->readAll();

    qDebug() << "******************************************************************************************************************";
    Respond resp = Worker::load_data(data);
    if (!resp.isGood)
        return;
    qDebug() << (uint8_t)resp.command << resp.data.toHex();
    switch (resp.command) {
        case ImageCapture:
            if (step != 2)
                return;
            response_check_command();
            emit tell_window_command(Command::ImageCapture,0);
            step = 3;
            emit tell_window_work_step(step);
            qDebug() << "send_get_check_command";
            break;

        case STM_WorkStatus:
            this->analysis_MCStatus(resp.data);
            if (status.E_Stop){
                this->is_Stoped_Work = true;
                return;
            }
            if (status.isStoped){
                this->is_Stoped_Work = true;
                return;
            }
            if (status.rollMontorSpeedStatus ||status.rollerMotorSpeedStatus || status.transportMotorSpeedStatus || status.slidingTableMotorSpeedStatus
                    || status.grab_cylinder || status.magnetic_roller_positioning_cylinder || status.magnetic_roller_lifting_cylinder || status.slider_lift_cylinder
                    || status.sucker_cylinder || status.pressure_cylinder || status.cutting_cylinder || status.waste_cylinder){
                this->is_Stoped_Work = true;
                return;
            }
            if(this->do_noting)
            {
                qDebug() << "do noting";
                this->do_noting = false;
                return;
            }
            if (step == 0){
                if(this->is_Stoped_Work)
                    return;
                step = 1;
                timer->stop();
                emit tell_window_work_step(step);
                start_work();
                qDebug() << "start_work";
            }
            else if(step == 1){
                if(this->is_Stoped_Work)
                    return;
                timer->stop();
                step = 2;
                emit tell_window_work_step(step);
                qDebug() << "wait_check_command";
            }
            else if (step == 3) {
                if(this->is_Stoped_Work)
                    return;
                step  = 4;
                timer->stop();
                emit tell_window_work_step(step);
                qDebug() << "wait_wrap_result";
            }
            else if (step == -1) {
                emit tell_window_command(Command::StopWork,0);
                timer->stop();
                qDebug() << "stop stm";
            }
            else if (step == -2) {
                emit tell_window_command(Command::Reset,0);
                timer->stop();
                qDebug() << "reset stm";
            }
            else if (step == -3) {
                emit tell_window_command(Command::RespondOK,step);
            }
            break;

        case WrapResult:
            if (step != 4)
                return;
            response_wrap_result(resp.data);
            emit tell_window_command(Command::WrapResult,(int)resp.data[0]);
            this->step = 5;
            emit tell_window_work_step(step);
            this->step = 0;
            emit tell_window_work_step(step);
            sleep(10);
            query_status();
            qDebug() << "next unit query_status";
            break;

        case RespondOK:
            qDebug() << "set speed ok";
            break;

        case Reset:
            this->is_Stoped_Work = false;
            status.E_Stop = false;
            emit tell_window_command(Command::Reset,0);
            emit tell_window_stm_status(status);
            this->response_reset_work();
            qDebug() << "ResetWork";
            break;

        case StopWork:
            if (status.E_Stop)
                return;
            this->is_Stoped_Work = true;
            this->response_stop_work();
            emit tell_window_command(Command::StopWork,0);
            this->do_noting = true;
            this->timer->stop();
            qDebug() << "StopWork";
            break;

        case ContinueWork:
            if (status.E_Stop)
                return;
            this->is_Stoped_Work = false;
//            this->response_continue_work();
            step = 0;
            emit tell_window_work_step(0);
            this->is_Stoped_Work = false;
            this->do_noting = false;
            this->query_status();
            qDebug() << "NEW start Work";
            break;

        case SuspendWork:
            if (status.E_Stop)
                return;
            this->is_Stoped_Work = true;
            this->response_suspend_work();
            emit tell_window_command(Command::SuspendWork,0);
            qDebug() << "SuspendWork";
            this->timer->stop();
            this->do_noting = true;
            break;

        case EmergencyStop:
            this->is_Stoped_Work = true;
            this->status.E_Stop = true;
            emit tell_window_stm_status(status);
            emit tell_window_command(Command::EmergencyStop,0);
            qDebug() << "EmergencyStop";
            break;

        case PackingBagLess:
            this->is_Stoped_Work = true;
            emit tell_window_command(Command::PackingBagLess,0);
            qDebug() << "PackingBagLess";
            break;

        case STMReportFault:
            timer->stop();
            this->is_Stoped_Work = true;
            this->analysis_FaultCode(resp.data);
            emit tell_window_command(Command::STMReportFault,resp.data);
            qDebug() << "STMReportFault";
            break;
    }
}

void Worker::accept_command_to_stm(Command com, int data)
{
    QByteArray da;
    QDataStream stream(&da,QIODevice::WriteOnly);
    if(!serial->isOpen()){
        emit tell_window_serial_status(false);
        return;
    }
    if(status.E_Stop && com != Command::Reset){
        return;
    }
    if (com == Command::StopWork){
        if(this->is_Stoped_Work)
            return;
        this->timer->stop();
        stream << (uint8_t)data;
        this->is_Stoped_Work = true;
        step = -1;
        qDebug() << "stop work";
    }
    else if (com ==Command::Reset){
        if (!status.E_Stop)
            return;
        stream << (uint8_t)data;
        this->is_Stoped_Work = false;
        qDebug() << "reset";
        step = -2;
    }
    if(com == Command::SetRollMontorSpeed ||com ==Command::SetRollerMotorSpeed ||com ==Command::SetTransportMotorSpeed ||com ==Command::SetSlidingTableMotorSpeed){
        stream << (uint16_t)data;
        step = -3;
    }
    else if (com == Command::CheckResult){
        if (step != 3)
            return;
        stream << (uint8_t)data;
        qDebug() << "tell_stm_result";
    }
    serial->write(Worker::dump_data(com,da));
    timer->start();
}

void Worker::accept_stop_work()
{
    timer->stop();
    this->is_Stoped_Work = true;
    if (this->serial->isOpen())
        this->serial->close();
}

void Worker::accept_open_serial(SerialSetting setting)
{
    if (serial->isOpen()){
        serial->close();
    }
    this->setting = setting;
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    emit tell_window_serial_status(serial->open(QIODevice::ReadWrite));
    this->is_Stoped_Work = false;
    qDebug() << "open serial";
}

void Worker::accept_timeout()
{
    timer->stop();
    this->is_Stoped_Work = true;
    emit tell_window_stm_respond_timeout();
    qDebug() << "timeout";
}


void Worker::accept_serial_error(const QSerialPort::SerialPortError &error)
{
    if (error == QSerialPort::NoError)
        return;
    if (serial->isOpen())
        serial->close();
    timer->stop();
    emit tell_window_serial_status(false);
    this->is_Stoped_Work = true;
    qDebug() << "serial errorOccurred" << error;
}

void Worker::accept_start_worker(SerialSetting setting)
{
    if (serial->isOpen())
        serial->close();
    this->setting = setting;
    serial->setPortName(setting.name);
    serial->setBaudRate(setting.baudRate);
    serial->setDataBits(setting.dataBits);
    serial->setParity(setting.parity);
    serial->setStopBits(setting.stopBits);
    serial->setFlowControl(setting.flowControl);
    if (!serial->open(QIODevice::ReadWrite)){
        emit tell_window_serial_status(false);
        return;
    }
    step = 0;
    emit tell_window_serial_status(true);
    emit tell_window_work_step(0);
    this->is_Stoped_Work = false;
    this->do_noting = false;
    this->query_status();
}



