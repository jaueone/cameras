#ifndef DEFECTSDETECT_H
#define DEFECTSDETECT_H

#include <Halcon.h>
#include <HalconCpp.h>
#include <QImage>
#include <QJsonObject>

using namespace HalconCpp;

class DefectsDetect: public QObject
{
    Q_OBJECT

public:
	DefectsDetect();
    ~DefectsDetect();

    int run(HObject ho_Image,HObject deal_image, const int width, const int height, const Hlong &winid, QJsonObject params);

    int get_result();


private:
    int _iType=0;

};


#endif
