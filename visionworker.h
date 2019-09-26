#ifndef VISIONWORKER_H
#define VISIONWORKER_H
#include <QObject>
#include <QLabel>
#include <QJsonObject>
#include "DefectsDetect.h"
#include "camera.h"

enum Error{
    NoError,
    ImageCaptureError,
    AlgorithmError
};

class VisionWorker : public QObject
{
    Q_OBJECT

public:
    explicit VisionWorker(HKCamera *camera ,QObject *parent = nullptr);
    void setWinid(const WId &value);

signals:
    void tell_window_result(QJsonObject);
    void tell_window_error(Error);

public slots:
    void accept_ImageCapture_Check(QJsonObject algorithm,int width, int height);

private:
    HKCamera *camera;
    WId winid = 0;

    HObject ho_Image;
    HObject deal_Image;

    DefectsDetect defect;
};

#endif // VISIONWORKER_H
