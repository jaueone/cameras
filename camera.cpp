#include "camera.h"
#include <QDebug>
#include <QLabel>
//#define MAX_BUF_SIZE    (6576*4384)

static HKCamera *camera_1 = nullptr;
static HKCamera *camera_2 = nullptr;

HKCamera *HKCamera::getInterface(const int &index)
{
    if (index == 0){
        if (camera_1 == nullptr){
            camera_1 = new HKCamera(0);
            return camera_1;
        }
        else {
            return camera_1;
        }
    }

    else if (index == 1){
        if (camera_2 == nullptr){
            camera_2 = new HKCamera(1);
            return camera_2;
        }
        else {
            return camera_2;
        }
    }

}


HKCamera::HKCamera(const int &index) {
    this->index = index;
}

HKCamera::~HKCamera()
{
    this->stopCollect();
    this->closeDevice();
    handle= NULL;
}

int HKCamera::enumDevices()
{
    nRet = MV_CC_EnumDevices(MV_GIGE_DEVICE | MV_USB_DEVICE, &m_stDevList);
    if (MV_OK != nRet)
    {
        qDebug() << "error: EnumDevices fail" << nRet;
        return -1;
    }
    if (m_stDevList.nDeviceNum == 0)
    {
        qDebug() << "no camera found!" << nRet;
        return 2;
    }
    return 0;
}

int HKCamera::openDevice()
{
    if (this->isOpened()){
        qDebug() << "information camera opened";
        return 0;
    }
    qDebug() << 3 << index;
    memcpy(&m_stDevInfo, m_stDevList.pDeviceInfo[this->index], sizeof(MV_CC_DEVICE_INFO));
    nRet = MV_CC_CreateHandle(&handle, &m_stDevInfo);
    qDebug() << 3;
    if (MV_OK != nRet)
    {
        qDebug("error: CreateHandle fail[%x]", nRet);
        return -1;
    }
    qDebug() << 3;
    nRet = MV_CC_OpenDevice(handle);
    if (nRet  != MV_OK)
    {
        qDebug("error: OpenDevice fail [%x]", nRet);
        return -1;
    }
    qDebug() << 3;
    qDebug() << "information camera opened";
    qDebug() << MV_CC_SetBoolValue(this->handle,"GevPAUSEFrameReception",true);

    return 0;
}

int HKCamera::startCollect()
{
    if (!this->isOpened()){
        qDebug() << "camera not opened";
        return -1;
    }
    /************************************************************************/
    /* 4.开启抓图     MV_CC_StartGrabbing                                   */
    /************************************************************************/
    //开始采集图像
    if (this->is_start_collected){
        qDebug() << "start collect";
        return 0;
    }
    nRet = MV_CC_StartGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StartGrabbing fail [%x]\n", nRet);
        return -1;
    }
    //设置相机图像的像素格式
   //unsigned int enValue = PixelType_Gvsp_BayerRG8;
    //unsigned int enValue = PixelType_Gvsp_RGB8_Packed;
//    nRet = MV_CC_SetPixelFormat(handle, enValue);
//    if (MV_OK != nRet)
//    {
//        qDebug("error: SetPixelFormat fail [%x]\n", nRet);
//        return -1;
//    }
    this->is_start_collected = true;
    qDebug() << "start collect";
    return 0;
}

int HKCamera::collectFrame(WId winid)
{

    /************************************************************************/
    /* 5.获取一帧并保存成图像  MV_CC_GetOneFrameTimeout  MV_CC_SaveImage    */
    /************************************************************************/
//    unsigned int    nTestFrameSize = 0;
//    unsigned char*  pFrameBuf = nullptr;
//    pFrameBuf = (unsigned char*)malloc(MAX_BUF_SIZE+2048);

//    MV_FRAME_OUT_INFO stInfo;
//    memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO));

//    MV_NETTRANS_INFO stNetTransInfo = {0};
    //获取一帧数据的大小
        MVCC_INTVALUE stIntvalue = {0};
        nRet = MV_CC_GetIntValue(handle, "PayloadSize", &stIntvalue);
        if (nRet != MV_OK)
        {
            qDebug("Get PayloadSize failed! nRet [%x]\n", nRet);
            return -1;
        }
        int nBufSize = stIntvalue.nCurValue; //一帧数据大小
        qDebug() << " size of image:"<< nBufSize;
       // unsigned int    nTestFrameSize = 0;
        unsigned char*  pFrameBuf = NULL;
        pFrameBuf = (unsigned char*)malloc(nBufSize);//显示数据
        unsigned char*  data = NULL;
        data = (unsigned char*)malloc(nBufSize);//转换到halcon数据使用
        MV_FRAME_OUT_INFO stInfo;
        memset(&stInfo, 0, sizeof(MV_FRAME_OUT_INFO));


        //上层应用程序需要根据帧率，控制好调用该接口的频率
        //此次代码仅供参考，实际应用建议另建线程进行图像帧采集和处理

        //pFrameBuf是相机采集到的一帧原始图像数据
        nRet = MV_CC_GetOneFrame(handle, pFrameBuf, nBufSize, &stInfo);
        MVCC_INTVALUE width,height;
        MV_CC_GetIntValue(handle, "Width",&width);
        MV_CC_GetIntValue(handle, "Height",&height);

        emit tell_window_Image_info(stInfo);

        int hgt = height.nCurValue;
        int wid = width.nCurValue;
        qDebug() << "hgt" << hgt << "wid" << wid;
        /*************************************显示图像**************************************/
        //获取窗口句柄
        HWND MainWndID = (HWND)winid;

        //显示图像
        //该接口需要在MV_CC_StartGrabbing之后调用，显示采集到的图像。如果相机当前采集图像是JPEG压缩的格式，则不支持调用该接口进行显示。
        nRet = MV_CC_Display(handle, MainWndID);
        if (MV_OK != nRet)
        {
            qDebug("error: Display fail [%x]\n", nRet);
            return -1;
        }
        /**************************unsigned char* 图像转换为HObject************************/
        memcpy(data, pFrameBuf, (size_t)nBufSize);
        GenImage1(&ho_Image, "byte", wid, hgt, (Hlong)(data));

//        WriteImage(ho_Image, "bmp", -1, HTuple("D:/photo/") + i);
        //Sleep(500);
        free(data);
        /**********************************************************************************/
    free(pFrameBuf);
    qDebug() << "colledted";
    i+= 1;
    return 0;
}

int HKCamera::stopCollect()
{
    /************************************************************************/
    /* 6. 停止抓图  MV_CC_StopGrabbing                                      */
    /************************************************************************/
    //停止采集图像
    if (!is_start_collected)
        return 0;
    nRet = MV_CC_StopGrabbing(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: StopGrabbing fail [%x]\n", nRet);
        return -1;
    }
    this->is_start_collected = false;
    qDebug() << "stop collect";
    return 0;
}

int HKCamera::closeDevice()
{
    /************************************************************************/
    /* 7. 关闭设备  MV_CC_CloseDevice                                       */
    /************************************************************************/
    //关闭设备，释放资源
    if (!this->isOpened()){
        return 0;
    }
    nRet = MV_CC_CloseDevice(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: CloseDevice fail [%x]\n", nRet);
        return -1;
    }
    if (MV_OK != this->destroyHandle()) {
        qDebug() << "destroy handle failed";
        return -1;
    }
    return 0;
}
int HKCamera::destroyHandle()
{
    /************************************************************************/
    /* 8. 销毁句柄  MV_CC_DestroyHandle                                     */
    /************************************************************************/
    //销毁句柄，释放资源
    nRet = MV_CC_DestroyHandle(handle);
    if (MV_OK != nRet)
    {
        qDebug("error: DestroyHandle fail [%x]\n", nRet);
        return -1;
    }
    qDebug() << "destroy Handle";
    return 0;
}


HObject HKCamera::getImage()
{
    return ho_Image;
}

int HKCamera::getIndex() const
{
    return index;
}

void HKCamera::setIndex(int value)
{
    index = value;
}

CameraSetting HKCamera::get_camera_setting()
{
    CameraSetting setting;
    if (!MV_CC_IsDeviceConnected(handle)){
        qDebug() << "camera no connected, get params fail";
        return setting;
    }
    MVCC_INTVALUE width,height,offsetX,offsetY,acquisitionLineRate, multiplier, preDivider;
    MVCC_FLOATVALUE gain, gamma, exposureTime;
    MVCC_ENUMVALUE gainAuto, gammaSelector, triggerSelector, triggerMode, triggerSource, lineSelector;

    MV_CC_GetIntValue(handle, "Width",&width);
    MV_CC_GetIntValue(handle, "Height",&height);
    MV_CC_GetIntValue(handle, "OffsetX",&offsetX);
    MV_CC_GetIntValue(handle, "OffsetY",&offsetY);
    MV_CC_GetIntValue(handle, "AcquisitionLineRate",&acquisitionLineRate);

    MV_CC_GetFloatValue(handle, "Gain", &gain);
    MV_CC_GetFloatValue(handle, "Gamma", &gamma);
    MV_CC_GetFloatValue(handle, "ExposureTime", &exposureTime);

    MV_CC_GetBoolValue(handle, "AcquisitionLineRateEnable", &setting.acquisitionLineRateEnable);
    MV_CC_GetBoolValue(handle, "GammaEnable", &setting.gammaEnable);
    MV_CC_GetBoolValue(handle, "NUCEnable", &setting.nucEnable);

    MV_CC_GetEnumValue(handle, "GainAuto", &gainAuto);
    MV_CC_GetEnumValue(handle, "GammaSelector", &gammaSelector);
    MV_CC_GetEnumValue(handle, "TriggerSelector", &triggerSelector);
    MV_CC_GetEnumValue(handle, "TriggerMode", &triggerMode);
    MV_CC_GetEnumValue(handle, "TriggerSource", &triggerSource);
    MV_CC_GetEnumValue(handle, "LineSelector", &lineSelector);

    MV_CC_GetIntValue(handle, "PreDivider",&preDivider);
    MV_CC_GetIntValue(handle, "Multiplier",&multiplier);

    setting.width = width.nCurValue;
    setting.height = height.nCurValue;
    setting.offsetX = offsetX.nCurValue;
    setting.offsetY = offsetY.nCurValue;
    setting.acquisitionLineRate = acquisitionLineRate.nCurValue;
    setting.preDivider = preDivider.nCurValue;
    setting.multiplier = multiplier.nCurValue;

    setting.gain = gain.fCurValue;
    setting.gamma = gamma.fCurValue;
    setting.exposureTime = exposureTime.fCurValue;

    setting.gainAuto = static_cast<GainAuto>(gainAuto.nCurValue);
    setting.gammaSelector = static_cast<GammaSelector>(gammaSelector.nCurValue);
    setting.triggerSelector = static_cast<TriggerSelector>(triggerSelector.nCurValue);
    setting.triggerMode = static_cast<TriggerMode>(triggerMode.nCurValue);
    setting.triggerSource = static_cast<TriggerSource>(triggerSource.nCurValue);
    setting.lineSelector = static_cast<LineSelector>(lineSelector.nCurValue);
    return setting;
}

QJsonObject HKCamera::setting_to_object(CameraSetting setting)
{
    QMap<int,QString> gain_map {
        {0,"Off"},
        {1,"Once"},
        {2,"Continuous"},
    };
    QMap<int,QString> gamma_map {
        {1,"User"},
        {2,"sRGB"},
    };
    QMap<int,QString> lineSelector_map {
        {0,"Line0"},
        {1,"Line1"},
        {2,"Line2"},
        {3,"Line3"},
        {4,"Line4"},
    };
    QMap<int,QString> triggerSource_map {
        {0,"Line0"},
        {1,"Line1"},
        {2,"Line2"},
        {3,"Line3"},
        {4,"Counter0"},
        {7,"Software"},
        {8,"FrequencyConverter"},
    };
    QMap<int,QString> triggerSelector_map {
        {6,"FrameBurstStart"},
        {9,"LineStart"},
    };
    QMap<int,QString> triggerMode_map {
        {0,"Off"},
        {1,"On"},
    };

    QJsonObject camera_obj
    {
        {QString("Width_%1").arg(index),(int)setting.width},
        {QString("Height_%1").arg(index),(int)setting.height},
        {QString("OffsetX_%1").arg(index),(int)setting.offsetX},
        {QString("OffsetY_%1").arg(index),(int)setting.offsetY},
        {QString("AcquisitionLineRate_%1").arg(index),(int)setting.acquisitionLineRate},
        {QString("AcquisitionLineRateEnable_%1").arg(index),setting.acquisitionLineRateEnable},
        {QString("Gain_%1").arg(index),(double)setting.gain},
        {QString("GainAuto_%1").arg(index),gain_map[setting.gainAuto]},
        {QString("Gamma_%1").arg(index),(double)setting.gamma},
        {QString("GammaEnable_%1").arg(index),setting.gammaEnable},
        {QString("GammaSelector_%1").arg(index),gamma_map[setting.gammaSelector]},
        {QString("ExposureTime_%1").arg(index),(double)setting.exposureTime},
        {QString("NUCEnable_%1").arg(index),setting.nucEnable},
        {QString("TriggerMode_%1").arg(index),triggerMode_map[setting.triggerMode]},
        {QString("TriggerSelector_%1").arg(index),triggerSelector_map[setting.triggerSelector]},
        {QString("TriggerSource_%1").arg(index),triggerSource_map[setting.triggerSource]},
        {QString("LineSelector_%1").arg(index),lineSelector_map[setting.lineSelector]},
        {QString("Multiplier_%1").arg(index),(int)setting.multiplier},
        {QString("PreDivider_%1").arg(index),(int)setting.preDivider},
   };
    return camera_obj;
}

CameraSetting HKCamera::camera_json_to_setting(QJsonObject object)
{
    QMap<QString, int> map = {
        {"Off"  , 0},
        {"Once" , 1},
        {"Continuous", 2},  //GainAuto
        {"User", 1},
        {"sRGB", 2},    //GammaSelector
        {"Line0", 0},
        {"Line1", 1},
        {"Line2", 2},
        {"Line3", 3},
        {"Counter0", 4},
        {"Software", 7},
        {"FrequencyConverter", 8},  //TriggerSource
        {"FrameBurstStart", 6},
        {"LineStart", 9},       //TriggerSelector
    };
    QMap<QString, int> map_1{
        {"Off", 0},
        {"On", 1},      //TriggerMode
        {"Line0", 0},
        {"Line1", 1},
        {"Line2", 2},
        {"Line3", 3},
        {"Line4", 4},   //LineSelector
    };
    CameraSetting setting;
    QJsonObject::const_iterator it = object.constBegin();
    QJsonObject::const_iterator end = object.constEnd();

    while (end != it)
    {
        if (it.key().contains("Width"))  setting.width = (uint)it.value().toInt();
        if (it.key().contains("Height"))  setting.height = (uint)it.value().toInt();
        if (it.key().contains("OffsetX"))  setting.offsetX = (uint)it.value().toInt();
        if (it.key().contains("OffsetY"))  setting.offsetY = (uint)it.value().toInt();
        if (it.key().contains("AcquisitionLineRate"))  setting.acquisitionLineRate = (uint)it.value().toInt();
        if (it.key().contains("PreDivider"))  setting.preDivider = (uint)it.value().toInt();
        if (it.key().contains("Multiplier"))  setting.multiplier = (uint)it.value().toInt();

        if (it.key().contains("AcquisitionLineRateEnable"))  setting.acquisitionLineRateEnable = it.value().toBool();
        if (it.key().contains("GammaEnable"))  setting.gammaEnable = it.value().toBool();
        if (it.key().contains("NUCEnable"))  setting.nucEnable = it.value().toBool();

        if (it.key().contains("ExposureTime"))  setting.exposureTime = (float)it.value().toDouble();
        if (it.key().contains("Gain"))  setting.gain = (float)it.value().toDouble();
        if (it.key().contains("Gamma"))  setting.gamma = (float)it.value().toDouble();

        if (it.key().contains("GainAuto"))  setting.gainAuto = (GainAuto)(map[it.value().toString()]);
        if (it.key().contains("GammaSelector"))  setting.gammaSelector = (GammaSelector)(map[it.value().toString()]);

        if (it.key().contains("TriggerMode"))  setting.triggerMode = (TriggerMode)(map_1[it.value().toString()]);
        if (it.key().contains("TriggerSelector"))  setting.triggerSelector = (TriggerSelector)(map[it.value().toString()]);
        if (it.key().contains("TriggerSource"))  setting.triggerSource = (TriggerSource)(map[it.value().toString()]);
        if (it.key().contains("LineSelector"))  setting.lineSelector = (LineSelector)(map_1[it.value().toString()]);
        it++;
    }
    return setting;
}

bool HKCamera::isOpened()
{
    if (MV_CC_IsDeviceConnected(handle)){
        return true;
    }
    return false;
}

bool HKCamera::isCollecting()
{
    return this->is_start_collected;
}

void HKCamera::camera_message_done()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("信息");
    messageBox.setIcon(QMessageBox::Information);
    QPushButton button("确定");
    messageBox.setText("设置相机参数完成!");
    messageBox.exec();
}

void HKCamera::rgb3_to_interleaved(HObject ho_ImageRGB, HObject *ho_ImageInterleaved)
{
    HObject  ho_ImageAffineTrans, ho_ImageRed, ho_ImageGreen;
    HObject  ho_ImageBlue, ho_RegionGrid, ho_RegionMoved, ho_RegionClipped;


    HTuple  hv_PointerRed, hv_PointerGreen, hv_PointerBlue;
    HTuple  hv_Type, hv_Width, hv_Height, hv_HomMat2DIdentity;
    HTuple  hv_HomMat2DScale;

    GetImagePointer3(ho_ImageRGB, &hv_PointerRed, &hv_PointerGreen, &hv_PointerBlue,
                     &hv_Type, &hv_Width, &hv_Height);
    GenImageConst(&(*ho_ImageInterleaved), "byte", hv_Width*3, hv_Height);
    //
    HomMat2dIdentity(&hv_HomMat2DIdentity);
    HomMat2dScale(hv_HomMat2DIdentity, 1, 3, 0, 0, &hv_HomMat2DScale);
    AffineTransImageSize(ho_ImageRGB, &ho_ImageAffineTrans, hv_HomMat2DScale, "constant",
                         hv_Width*3, hv_Height);
    //
    Decompose3(ho_ImageAffineTrans, &ho_ImageRed, &ho_ImageGreen, &ho_ImageBlue);
    GenGridRegion(&ho_RegionGrid, 2*hv_Height, 3, "lines", hv_Width*3, hv_Height+1);
    MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 0);
    ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);

    ReduceDomain(ho_ImageRed, ho_RegionClipped, &ho_ImageRed);
    MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 1);
    ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);
    ReduceDomain(ho_ImageGreen, ho_RegionClipped, &ho_ImageGreen);
    MoveRegion(ho_RegionGrid, &ho_RegionMoved, -1, 2);
    ClipRegion(ho_RegionMoved, &ho_RegionClipped, 0, 0, hv_Height-1, (3*hv_Width)-1);
    ReduceDomain(ho_ImageBlue, ho_RegionClipped, &ho_ImageBlue);
    OverpaintGray((*ho_ImageInterleaved), ho_ImageRed);
    OverpaintGray((*ho_ImageInterleaved), ho_ImageGreen);
    OverpaintGray((*ho_ImageInterleaved), ho_ImageBlue);
    return;
}

void HKCamera::HObjectToQImage(HObject himage, QImage **qimage)
{
    HTuple hChannels;
    HTuple   width,height;
    width=height=0;
    HTuple htype;
    HTuple hpointer;

    ConvertImageType(himage,&himage,"byte");//将图片转化成byte类型
    CountChannels(himage,&hChannels);       //判断图像通道数

    if(hChannels[0].I()==1)//单通道图
    {
        unsigned char *ptr;

        GetImagePointer1(himage,&hpointer,&htype,&width,&height);

        ptr=(unsigned char *)hpointer[0].L();
        *(*qimage)=QImage(ptr,width,height,width,QImage::Format_Indexed8);//不知道是否已自动4字节对齐
    }
    else if(hChannels[0].I()==3)//三通道图
    {
        unsigned char *ptr3;
        HObject ho_ImageInterleaved;
        rgb3_to_interleaved(himage, &ho_ImageInterleaved);

        GetImagePointer1(ho_ImageInterleaved, &hpointer, &htype, &width, &height);

        ptr3=(unsigned char *)hpointer[0].L();
        *(*qimage)=QImage(ptr3,width/3,height,width,QImage::Format_RGB888);
    }
}

void HKCamera::camera_message_warn()
{
    QMessageBox messageBox;
    messageBox.setWindowTitle("警告");
    messageBox.setIcon(QMessageBox::Warning);
    QPushButton button("确定");
    messageBox.setText("相机连接失败,\n请先打开相机!");
    messageBox.exec();
}


int HKCamera::setParam(DType type,const char *params, QVariant value)
{
    if (type == DType::Bool){
        return  MV_CC_SetBoolValue(handle, params,value.toBool());
    }
    else if (type == DType::Int){
        return MV_CC_SetIntValue(handle, params,value.toUInt());
    }
    else if (type == DType::Float){
        return MV_CC_SetFloatValue(handle, params,value.toFloat());
    }
    else if (type == DType::Enum){
        return MV_CC_SetEnumValue(handle, params,value.toUInt());
    }
    else if (type == DType::String){
        return MV_CC_SetStringValue(handle, params, value.toString().toStdString().c_str());
    }
    return 0;
}

int HKCamera::setCameraSetting(CameraSetting setting)
{
    qDebug("%x",this->setParam(DType::Int, "Width", setting.width));
    qDebug("%x",this->setParam(DType::Int, "Height", setting.height));
    qDebug("%x",this->setParam(DType::Int, "OffsetX", setting.offsetX));
    qDebug("%x",this->setParam(DType::Int, "OffsetY", setting.offsetY));
    qDebug("%x",this->setParam(DType::Int, "AcquisitionLineRate", setting.acquisitionLineRate));
    qDebug("%x",this->setParam(DType::Bool, "AcquisitionLineRateEnable", setting.acquisitionLineRateEnable));
    qDebug("%x",this->setParam(DType::Float, "Gain", setting.gain));
    qDebug("%x",this->setParam(DType::Enum, "GainAuto", setting.gainAuto));
    qDebug("%x",this->setParam(DType::Float, "Gamma", setting.gamma));
    qDebug("%x",this->setParam(DType::Bool, "GammaEnable", setting.gammaEnable));
    qDebug("%x",this->setParam(DType::Enum, "GammaSelector", setting.gammaSelector));
    qDebug("%x",this->setParam(DType::Float, "ExposureTime", setting.exposureTime));
    qDebug("%x",this->setParam(DType::Bool, "NUCEnable", setting.nucEnable));
    qDebug("%x",this->setParam(DType::Enum, "TriggerMode", setting.triggerMode));
    qDebug("%x",this->setParam(DType::Enum, "TriggerSelector", setting.triggerSelector));
    qDebug("%x",this->setParam(DType::Enum, "TriggerSource", setting.triggerSource));
    qDebug("%x",this->setParam(DType::Enum, "LineSelector", setting.lineSelector));
    qDebug("%x",this->setParam(DType::Int, "PreDivider", setting.preDivider));
    qDebug("%x",this->setParam(DType::Int, "Multiplier", setting.multiplier));
    return 0;
}

QJsonObject HKCamera::getParams()
{
    return this->setting_to_object(this->get_camera_setting());
}

void HKCamera::setParams(QJsonObject parmas)
{
    setCameraSetting(this->camera_json_to_setting(parmas));
}


