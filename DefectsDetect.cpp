#include "DefectsDetect.h"
#include <QDebug>
#include <QTime>


void estimate_background_illumination (HObject ho_Image, HObject *ho_IlluminationImage)
{

    // Local iconic variables
    HObject  ho_ImageFFT, ho_ImageGauss, ho_ImageConvol;

    // Local control variables
    HTuple  hv_Width, hv_Height;

    GetImageSize(ho_Image, &hv_Width, &hv_Height);
    RftGeneric(ho_Image, &ho_ImageFFT, "to_freq", "none", "complex", hv_Width);
    GenGaussFilter(&ho_ImageGauss, 50, 50, 0, "n", "rft", hv_Width, hv_Height);
    ConvolFft(ho_ImageFFT, ho_ImageGauss, &ho_ImageConvol);
    RftGeneric(ho_ImageConvol, &(*ho_IlluminationImage), "from_freq", "none", "byte",hv_Width);
    return;
}

DefectsDetect::DefectsDetect()
{

}

DefectsDetect::~DefectsDetect()
{

}

int DefectsDetect::run(HObject ho_Image, HObject deal_image, const int width, const int height, const Hlong &winid, QJsonObject params)
{
    // Local iconic variables
      HObject  ho_Image0, ho_GrayImage, ho_Rectangle0;
      HObject  ho_ImageReduced0, ho_ImageFFT1, ho_ImageSub, ho_ImageSub1;
      HObject  ho_ImageResult, ho_Image2, ho_Rectangle1, ho_ImageReduced1;
      HObject  ho_Image1, ho_Rectangle2, ho_ImageReduced2, ho_Regions;
      HObject  ho_Rectangle, ho_ImageReduced, ho_ConnectedRegions;
      HObject  ho_SelectedRegions, ho_RegionUnion, ho_Regions1;
      HObject  ho_ConnectedRegions1, ho_SelectedRegions1, ho_RegionUnion1;

      // Local control variables
      HTuple  hv_saveimage, hv_saveFile0, hv_studyFile0;
      HTuple  hv_studyFile1, hv_studyFile2, hv_train, hv_writemlp;
      HTuple  hv_readmlp, hv_test, hv_numbleOK, hv_numbleNG, hv_numbleLightOK;
      HTuple  hv_numbleLightNG, hv_redlight, hv_greenlight, hv_Index112;
      HTuple  hv_name, hv_datanumber, hv_Classes, hv_inspectOnLine;
      HTuple  hv_medianDegree, hv_MLPHandle, hv_ImageFiles, hv_Index;
      HTuple  hv_Width0, hv_Height0, hv_WindowHandle0, hv_Energy;
      HTuple  hv_Correlation, hv_Homogeneity, hv_Contrast, hv_AbsoluteHisto;
      HTuple  hv_FeaturesExtended, hv_NumFeatures, hv_FeatureVector;
      HTuple  hv_Result, hv_ImageFiles2, hv_ImageFiles1, hv_Index1;
      HTuple  hv_Width2, hv_Height2, hv_WindowHandle1, hv_Energy1;
      HTuple  hv_Correlation1, hv_Homogeneity1, hv_Contrast1;
      HTuple  hv_AbsoluteHisto1, hv_FeaturesExtended1, hv_NumFeatures1;
      HTuple  hv_FeatureVector1, hv_Result2, hv_Width1, hv_Height1;
      HTuple  hv_Result1, hv_Error, hv_ErrorLog, hv_FileHandle;
      HTuple  hv_WindowHandle2, hv_Value, hv_Value1, hv_Class112;
      HTuple  hv_Energy2, hv_Correlation2, hv_Homogeneity2, hv_Contrast2;
      HTuple  hv_AbsoluteHisto2, hv_FeaturesExtended112, hv_NumFeatures112;
      HTuple  hv_FeatureVector112, hv_Confidence112;

    //Image Acquisition 01: Code generated by Image Acquisition 01
    //Image Acquisition 01: be set in a specific order (e.g., image resolution vs. offset).
      double wihte_area_min = params["wihte_area_min"].toDouble();
      double black_area_min = params["black_area_min"].toDouble();
      double black_threshold = params["black_threshold"].toDouble();
      double white_threshold = params["white_threshold"].toDouble();
      double black_min = params["black_min"].toDouble();
      double white_min = params["white_min"].toDouble();

      //保存图片使能
        hv_saveimage = 0;
        //保存图片到文件0
        hv_saveFile0 = 0;
        //学习文件0内样本
        hv_studyFile0 = 1;
        //学习文件1内样本
        hv_studyFile1 = 1;
        //学习文件2内样本
        hv_studyFile2 = 1;
        hv_train = 1;
        //保存训练结果
        hv_writemlp = 0;
        //读取训练结果
        hv_readmlp = 0;
        //报告表编号
        hv_test = 1;

        hv_numbleOK = 0;
        hv_numbleNG = 0;
        hv_numbleLightOK = 0;
        hv_numbleLightNG = 0;
        hv_redlight = 0;
        hv_greenlight = 0;
        hv_Index112 = 0;

        hv_name = 0;
        hv_datanumber = 0;
        hv_Classes.Clear();
        hv_Classes[0] = "OK";
        hv_Classes[1] = "notOK";
        hv_Classes[2] = "NG";
        hv_inspectOnLine = 0;



        hv_medianDegree = 1.5;
/*
 * OpenWindow(0,0,width,height,winid,"","",&hv_WindowHandle2);
 *
*/

    if (0 != hv_test)
    {
        ReadClassMlp("StickDetect.gmc", &hv_MLPHandle);
        {
            //ReadImage(&ho_Image2, HTuple(hv_ImageFiles[hv_Index]));
            ho_Image2 = ho_Image;


            GetImageSize(ho_Image2, &hv_Width2, &hv_Height2);
            if (HDevWindowStack::IsOpen())
                CloseWindow(HDevWindowStack::Pop());
            SetWindowAttr("background_color","black");
            OpenWindow(0,0,width,height,winid,"","",&hv_WindowHandle2);
            HDevWindowStack::Push(hv_WindowHandle2);
            if (HDevWindowStack::IsOpen())
                SetPart(HDevWindowStack::GetActive(),0, 0, hv_Height2, hv_Width2);
            if (HDevWindowStack::IsOpen())
                DispObj(ho_Image2, HDevWindowStack::GetActive());
            //disp_message (WindowHandle2, '带分辨', 'window', 10, 10, 'black', 'true')
            //draw_rectangle1 (WindowHandle2, Row2, Column2, Row21, Column21)
            //gen_rectangle1 (Rectangle2, Row2, Column2, Row21, Column21)

            GenRectangle1(&ho_Rectangle2, 1, 1, hv_Height2-2, hv_Width2-2);
            ReduceDomain(ho_Image2, ho_Rectangle2, &ho_ImageReduced2);



            estimate_background_illumination(ho_ImageReduced2, &ho_ImageFFT1);
            SubImage(ho_ImageReduced2, ho_ImageFFT1, &ho_ImageSub, 2, 0);
            SubImage(ho_ImageFFT1, ho_ImageReduced2, &ho_ImageSub1, 2, 0);
            AddImage(ho_ImageSub, ho_ImageSub1, &ho_ImageResult, 2, 0);


            Threshold(ho_ImageResult, &ho_Regions, white_threshold, 255);//黑色阈值
            if (HDevWindowStack::IsOpen())
              SetDraw(HDevWindowStack::GetActive(),"margin");
            GenRectangle1(&ho_Rectangle, 1, 20, hv_Height2-2, hv_Width2-10);
            ReduceDomain(ho_Regions, ho_Rectangle, &ho_ImageReduced);

            Connection(ho_ImageReduced, &ho_ConnectedRegions);
            SelectShape(ho_ConnectedRegions, &ho_SelectedRegions, "area", "and", white_min,       //单点多大
                99999);
            Union1(ho_SelectedRegions, &ho_RegionUnion);
            RegionFeatures(ho_RegionUnion, "area", &hv_Value);

            Threshold(ho_ImageResult, &ho_Regions1, black_threshold, 255);//白色阈值
            GenRectangle1(&ho_Rectangle1, 1, 1, hv_Height2-2, 19);
            ReduceDomain(ho_Regions1, ho_Rectangle1, &ho_ImageReduced1);
            Connection(ho_ImageReduced1, &ho_ConnectedRegions1);
            SelectShape(ho_ConnectedRegions1, &ho_SelectedRegions1, "area", "and", black_min,      //单点多大
                99999);
            Union1(ho_SelectedRegions1, &ho_RegionUnion1);
            RegionFeatures(ho_RegionUnion1, "area", &hv_Value1);
            if (HDevWindowStack::IsOpen())
              ClearWindow(HDevWindowStack::GetActive());
            if (HDevWindowStack::IsOpen())
              DispObj(ho_Image2, HDevWindowStack::GetActive());
            if (HDevWindowStack::IsOpen())
              DispObj(ho_RegionUnion, HDevWindowStack::GetActive());
            if (HDevWindowStack::IsOpen())
              DispObj(ho_RegionUnion1, HDevWindowStack::GetActive());
            if (0 != (HTuple(hv_Value>wihte_area_min).TupleOr(hv_Value1>black_area_min)))//白色面积下限  白色面积上限
            {
              hv_Class112 = 2;
            }
            else
            {
                CoocFeatureImage(ho_Rectangle2, ho_ImageResult, 6, 90, &hv_Energy2, &hv_Correlation2,
                          &hv_Homogeneity2, &hv_Contrast2);

                GrayHistoAbs(ho_Rectangle2, ho_ImageResult, 8, &hv_AbsoluteHisto2);
                //***************************
                hv_FeaturesExtended112.Clear();
                hv_FeaturesExtended112.Append(hv_Energy2);
                hv_FeaturesExtended112.Append(hv_Correlation2);
                hv_FeaturesExtended112.Append(hv_Homogeneity2);
                hv_FeaturesExtended112.Append(hv_Contrast2);
                hv_FeaturesExtended112.Append(hv_AbsoluteHisto2);
                hv_NumFeatures112 = hv_FeaturesExtended112.TupleLength();
                hv_FeatureVector112 = hv_FeaturesExtended112.TupleReal();
                ClassifyClassMlp(hv_MLPHandle, hv_FeatureVector112, 1, &hv_Class112, &hv_Confidence112);

            }

            if (0 != (hv_Class112==0))
            {
            //    disp_message(hv_WindowHandle2, "良品"+hv_Class112, "window", 10, 10, "black",
            //              "true");
            }
            else if (0 != (hv_Class112==2))
            {

            //    disp_message(hv_WindowHandle2, "废品"+hv_Class112, "window", 10, 10, "black",
            //              "true");
            }
            else if (0 != (hv_Class112==1))
            {

            //    disp_message(hv_WindowHandle2, "次品"+hv_Class112, "window", 10, 10, "black",
            //              "true");
            }
//            FwriteString(hv_FileHandle, (((hv_Index+1)+":")+hv_Class112)+"\n");
            //stop ()
        }
    }
    // stop(); only in hdevelop

//**********************************************

    //**********************************************
    int i = hv_Class112.I();

    if (i == 0)
    {
        return 2;
    }
    else if (i == 1)
    {
        return 1;
    }
    else if (i == 2)
    {
        return 0;
    }
    else
    {
        return -1;
    }

}

int DefectsDetect::get_result()
{
    return 0;
}
