//
// Created by jasonyin111 on 2019/12/29.
//

#ifndef TRY_AUTODETECTION_H
#define TRY_AUTODETECTION_H

#include "opencv2/opencv.hpp"
#include "iostream"
#include "string.h"

using namespace cv;
using namespace std;

class MainWindow;

enum Color
{
    BLUE = 0,
    RED = 1
};

struct Armor_param
{
    int     ColorThreshold        = 100;//BGR二值化
    int     BrightnessThreashold  = 120;//亮度二值化
    float   maxArmorLight_angle   = 155;//筛除灯条的最小角度
    float   minArmorLight_angle   = 25.0; //筛除灯条的最大角度
    float   minLightAspectRatio   = 3.0;//最小灯条长宽比
    float   maxLightAspectRatio   = 11.0;//最大灯条长宽比
    float   lightAngleDiff        = 6.0; //灯条角度差
    float   minAspectRatio        = 1.0; //最小装甲板长宽比
    float   maxAspectRatio        = 3.5;//最大装甲板长宽比
    float   maxRectangle          = 26.0;//最大角
    float   imageCenterX          = 960;
    float   imageCenterY          = 640;
};

class ArmorLight : public cv::RotatedRect
{
public:
    explicit ArmorLight(const cv::RotatedRect& rotatedRect, cv::Point2f* points);

    ~ArmorLight();

public:
    std::vector<cv::Point2f>    rotateRectPoints;
};
class Armor
{
public:
    Armor() = default;

    Armor(cv::Mat& armorImg, cv::RotatedRect& rotatedRect);

    Armor(Armor& armor);

    Armor(Armor&& armor) noexcept;

    Armor& operator=(Armor& armor);

    Armor& operator=(Armor&& armor) noexcept;

    ~Armor()= default;

public:
    cv::Mat                     armorImg;
    cv::RotatedRect             armorRotatedRect;
    //Armor(cv::Mat armorImg, cv::RotatedRect rotateRect);
};

class ArmorDetect
{
public:
    ArmorDetect() = default;

    //ArmorDetect(Widget *w);

    ~ArmorDetect() = default;

    void setEnemyColor(Color enemyColor);

    void setRoi(cv::Mat& inputImg);

    void AdjustColor(Mat& inputImg, Mat& ouputImg, int Mothed);

    void AdjustBrightness(Mat& inputImg, Mat& outputImg);

    bool findLights(Mat& ColorImg, Mat& BrightnessImg,Mat &img);

    bool findArmors(cv::Mat& srcImg);

    void ajustAngle(cv::RotatedRect& rect);

    bool makeRectSafe(cv::Rect& rect, cv::Size size);

    void armorPerspectiveTrans(cv::Mat& srcImg, cv::Mat& armorImg, cv::Rect2f& armorRoi, ArmorLight& leftLight, ArmorLight& rightLight);

    cv::RotatedRect boundingRotatedRect(const cv::RotatedRect& left, const cv::RotatedRect& right);

    Armor strikingDecision();

    Armor detect(cv::Mat& inputImg);

    void getcircle(cv::Mat& inputImg);

public:
    Color                           _enemyColor;
    std::vector<ArmorLight>         _armorLights;
    std::vector<Armor>              _armors;
    cv::Rect                        _detecArea;
    cv::RotatedRect                 _lastRotateRect;
    Armor_param                     _detectParam;
    int                             _lostCount;
    MainWindow                       *w_;

};























#endif //TRY_AUTODETECTION_H
