#include "visionworker.h"



VisionWorker::VisionWorker(HKCamera *camera, QObject *parent)
{
    this->camera = camera;
}

void VisionWorker::accept_ImageCapture_Check(QJsonObject algorithm, int width, int height)
{
    if (MV_OK != camera->startCollect())
        return;
    if (MV_OK != camera->collectFrame(this->winid)) {
        emit tell_window_error(ImageCaptureError);
        return;
    }
//    ho_Image = camera->getImage();
//    int result = defect.run(ho_Image,deal_Image,width,height,(Hlong)winid,algorithm);
//    if (result < 0) {
//        emit tell_window_error(AlgorithmError);
//        return;
//    }
//    emit tell_window_result({{"result",result}});
}

void VisionWorker::setWinid(const WId &value)
{
    winid = value;
}
