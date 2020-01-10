//
// Created by jasonyin111 on 2019/12/29.
//



#include "autodetection.h"
#include "../mainwindow.h"

ArmorLight::ArmorLight(const cv::RotatedRect& rotatedRect,
                       cv::Point2f* points)
        :cv::RotatedRect(rotatedRect.center,rotatedRect.size,rotatedRect.angle)
{
    rotateRectPoints = { points[0],points[1] ,points[2] ,points[3] };

}

ArmorLight::~ArmorLight()
{


}

Armor::Armor(cv::Mat& armorImg, cv::RotatedRect& rotatedRect) :armorImg(armorImg)
{
    this->armorRotatedRect.size = rotatedRect.size;

    this->armorRotatedRect.center = rotatedRect.center;

    this->armorRotatedRect.angle = rotatedRect.angle;
}

Armor::Armor(Armor& armor)
{
    *this = armor;
}

Armor::Armor(Armor&& armor) noexcept
{
    *this = std::move(armor);
}

Armor& Armor::operator=(Armor& armor)
{
    this->armorRotatedRect = armor.armorRotatedRect;
    this->armorImg = armor.armorImg;
    return *this;
}

Armor& Armor::operator=(Armor&& armor) noexcept
{
    this->armorRotatedRect = std::move(armor.armorRotatedRect);
    this->armorImg = std::move(armor.armorImg);
    return *this;
}

void ArmorDetect::setEnemyColor(Color enemyColor)
{
    _enemyColor = enemyColor;
}

void ArmorDetect::setRoi(cv::Mat& inputImg)
{

    if (_lastRotateRect.center.x <= 0 ||
        _lastRotateRect.center.y <= 0)
    {
        _detecArea.x = 0;
        _detecArea.y = 0;
        return;
    }

    cv::Point finalAimCenter = _lastRotateRect.center;
    cv::Rect rect = _lastRotateRect.boundingRect();

    int detecRectWidth = rect.width * 2;
    int detecRectHeight = rect.height * 2;

    int tlx = std::max(finalAimCenter.x - detecRectWidth, 0);
    int tly = std::max(finalAimCenter.y - detecRectHeight, 0);
    cv::Point tl(tlx, tly);

    int brx = std::min(finalAimCenter.x + detecRectWidth, inputImg.cols);
    int bry = std::min(finalAimCenter.y + detecRectHeight, inputImg.rows);
    cv::Point br(brx, bry);

    _detecArea = cv::Rect(tl, br);
    makeRectSafe(_detecArea, inputImg.size());

    inputImg = inputImg(_detecArea);
    //cout << "_detecArea" << endl;
    //imshow("666", inputImg);
}

void ArmorDetect::AdjustColor(Mat& inputImg, Mat&outputImg , int Mothed)
{
    if (_enemyColor)
    {
        Mat srcImg;
        vector<Mat>BGR;
        split(inputImg, BGR);
        outputImg = BGR[2] - BGR[0];
        threshold(outputImg, outputImg, 40, 255, THRESH_BINARY);
        //imshow("Rcolor", outputImg);
    }

    else
    {
        Mat srcImg;
        vector<Mat>BGR;
        split(inputImg, BGR);
        outputImg = BGR[0] - BGR[2];
        threshold(outputImg, outputImg, 46, 255, THRESH_BINARY);
        //imshow("Bcolor", outputImg);
    }

}

void ArmorDetect::AdjustBrightness(Mat& inputImg, Mat& outputImg)
{
    cv::cvtColor(inputImg, outputImg,COLOR_BGR2GRAY);

    threshold(outputImg, outputImg,_detectParam.BrightnessThreashold,255,THRESH_BINARY);


}

bool ArmorDetect::findLights(cv::Mat& binColor, cv::Mat& binBright, cv::Mat& img)
{
    assert(binColor.channels() == 1 && binBright.channels() == 1);
    Mat SrcImg;
    static std::vector<std::vector<cv::Point>>contours;
    static cv::Point2f rotateRectPoints[4];
    cv::Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    SrcImg = binColor & binBright;
    dilate(SrcImg, SrcImg, kernel);
    //imshow("shuchu", SrcImg);
    findContours(SrcImg, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
    for (size_t i = 0; i < contours.size(); i++)
    {
        float area = contourArea(contours[i]);
        if (area <=20 || contours[i].size() < 5)
        {
            continue;
        }
        cv::RotatedRect rotatedRect = fitEllipse(contours[i]);

        float L_angle  = rotatedRect.angle;
        float L_width  = rotatedRect.size.width;
        float L_height = rotatedRect.size.height;
        float L_ratio    = L_height/L_width;
        if(L_ratio >_detectParam.maxLightAspectRatio
           ||L_ratio <_detectParam.minLightAspectRatio)
        {
            continue;
        }
        if(L_angle>_detectParam.minArmorLight_angle
           &&L_angle<_detectParam.maxArmorLight_angle)
        {
            continue;
        }
        //ajustAngle(rotatedRect);
        rotatedRect.points(rotateRectPoints);
        _armorLights.emplace_back(rotatedRect, rotateRectPoints);
        /* for (int i = 0; i < 4; i++)
         {
             line(img, rotateRectPoints[i], rotateRectPoints[(i + 1) % 4], Scalar(0, 255, 255), 3);
             putText(img,cv::format(("%.0f %.0f %.0f %.0f "),L_angle,L_ratio,L_width,L_height),rotateRectPoints[2],FONT_HERSHEY_COMPLEX,0.5,Scalar(0,0,255));
         }*/
    }
    contours.clear();
    return _armorLights.size() >=2 ;
}

bool ArmorDetect::findArmors(cv::Mat& srcImg)
{
    int armorLightsSize = static_cast<int>(_armorLights.size());
    //sort by armor light x
    std::sort(_armorLights.begin(), _armorLights.end(), [](ArmorLight& a1, ArmorLight& a2) -> bool
    {
        return a1.center.x < a2.center.x;
    });
    for (int i = 0; i < armorLightsSize - 1; i++)
    {
        for (int j = i + 1; j < armorLightsSize; j++)
        {

            ArmorLight &left = _armorLights[i];
            ArmorLight &right = _armorLights[j];



            float left_angle = left.angle;
            float right_angle = right.angle;
            float left_w = left.size.width;
            float left_h = left.size.height;
            float right_w = right.size.width;
            float right_h = right.size.height;
            float heightdiff = fabs(left_h - right_h)/max(left_h,right_h);
            float widthdiff  = fabs(left_w - right_w)/max(left_w,left_h);

            if(heightdiff>0.10)
            {
                continue;
            }

            float angleDiff = std::fabs(std::fabs(left_angle) - std::fabs(right_angle));
            if (angleDiff >= _detectParam.lightAngleDiff)
            {
                continue;
            }

            float x = right.center.x - left.center.x;
            float y = right.center.y - left.center.y;
            if (x < (left.size.width + right.size.width) * 2)
            {
                
                continue;
            }
            cv::RotatedRect armorRotatedRect = boundingRotatedRect(left, right);
            ajustAngle(armorRotatedRect);
            float angle = armorRotatedRect.angle;
            if(fabs(angle)>_detectParam.maxRectangle)
            {
                break;
            }
            float width = std::max(armorRotatedRect.size.width, armorRotatedRect.size.height);
            float height = std::min(armorRotatedRect.size.width, armorRotatedRect.size.height);
            float ratio = width / height;
            if(_detectParam.minAspectRatio >= ratio ||ratio >= _detectParam.maxAspectRatio)
            {
                break;
            }

            cv::Rect roi = armorRotatedRect.boundingRect();
            makeRectSafe(roi, srcImg.size());
            cv::Mat armorImg(srcImg, roi);
            cv::cvtColor(armorImg, armorImg, cv::COLOR_BGR2GRAY);
            _armors.emplace_back(armorImg, armorRotatedRect);
        }
    }
    return !_armors.empty();
}

void ArmorDetect::ajustAngle(cv::RotatedRect& rect)
{
    if (rect.angle >= 90)//确保在[-90,0]
    {
        rect.angle -= 180;
    }
}

void ArmorDetect::armorPerspectiveTrans(cv::Mat& srcImg, cv::Mat& armorImg, cv::Rect2f& armorRoi, ArmorLight& leftLight, ArmorLight& rightLight)
{
    static cv::Mat transferMatrix;
    static std::vector<cv::Point2f> srcPoint(4);
    static std::vector<cv::Point2f> dstPoint = { cv::Point2f(0, 0),      //top left
                                                 cv::Point2f(0,50),      //bottom left
                                                 cv::Point2f(50, 0),     //top right
                                                 cv::Point2f(50, 50) };   //bottom right

    srcPoint[0] = leftLight.rotateRectPoints[1];    //left light top left
    srcPoint[1] = leftLight.rotateRectPoints[0];    //left light bottom left
    srcPoint[2] = rightLight.rotateRectPoints[2];   //right light top right
    srcPoint[3] = rightLight.rotateRectPoints[3];   //right light bottom right

    transferMatrix = cv::getPerspectiveTransform(srcPoint, dstPoint);
    cv::warpPerspective(srcImg, armorImg, transferMatrix, cv::Size(armorRoi.width, armorRoi.height));
}

bool ArmorDetect::makeRectSafe(cv::Rect& rect, cv::Size size)
{
    if (rect.width <= 0 || rect.height <= 0)
    {
        return false;
    }

    if (rect.x < 0)
    {
        rect.x = 0;
    }

    if (rect.x + rect.width > size.width)
    {
        rect.width = size.width - rect.x;
    }

    if (rect.y < 0)
    {
        rect.y = 0;
    }
    if (rect.y + rect.height > size.height)
    {
        rect.height = size.height - rect.y;
    }

    return true;
}

cv::RotatedRect ArmorDetect::boundingRotatedRect(const cv::RotatedRect& left, const cv::RotatedRect& right)
{

    const cv::Point& pl = left.center, & pr = right.center;
    cv::Point2f center = (pl + pr) / 2.0;
    double width_l = left.size.width;
    double width_r = right.size.width;
    double height_l = left.size.height;
    double height_r = right.size.height;
    float width = std::sqrt((pl.x - pr.x) * (pl.x - pr.x) + (pl.y - pr.y) * (pl.y - pr.y)) - (width_l + width_r) / 2.0;
    float height = std::max(height_l, height_r);
    float angle = std::atan2(right.center.y - left.center.y, right.center.x - left.center.x);
    //atan2 = y/x
    return cv::RotatedRect(center, cv::Size2f(width, height), angle * 180 / CV_PI);

}

Armor ArmorDetect::strikingDecision()
{
    if (_armors.size() == 1)
    {
        return _armors[0];
    }
    std::sort(_armors.begin(), _armors.end(),
              [](Armor& a1, Armor& a2)-> bool
              {
                  //float a1Height = std::min(a1.armorRotatedRect.size.height, a1.armorRotatedRect.size.width);
                  //float a2Height = std::min(a2.armorRotatedRect.size.height, a2.armorRotatedRect.size.width);
                  float a1Area = a1.armorRotatedRect.size.height * a1.armorRotatedRect.size.width;
                  float a2Area = a2.armorRotatedRect.size.height * a2.armorRotatedRect.size.width;
                  return a1Area > a2Area;
              });
    size_t finalIndex = 0;
    float finalDistance = std::sqrt(((_armors[finalIndex].armorRotatedRect.center.x - _detectParam.imageCenterX) * \
		(_armors[finalIndex].armorRotatedRect.center.x - _detectParam.imageCenterX)) + \
		((_armors[finalIndex].armorRotatedRect.center.y - _detectParam.imageCenterY) * \
		(_armors[finalIndex].armorRotatedRect.center.y - _detectParam.imageCenterY)));

    for (size_t i = 1; i < _armors.size(); ++i)
    {
        float distance = std::sqrt(((_armors[i].armorRotatedRect.center.x - _detectParam.imageCenterX) * \
			(_armors[i].armorRotatedRect.center.x - _detectParam.imageCenterX)) + \
			((_armors[i].armorRotatedRect.center.y - _detectParam.imageCenterY) * \
			(_armors[i].armorRotatedRect.center.y - _detectParam.imageCenterY)));

        if (std::fabs(_armors[i].armorRotatedRect.angle) < std::fabs(_armors[finalIndex].armorRotatedRect.angle))//fabsÇóŸø¶ÔÖµ
        {
            if (std::fabs(_armors[finalIndex].armorRotatedRect.angle) - std::fabs(_armors[i].armorRotatedRect.angle) < 3.0f)
            {
                if (finalDistance > distance)
                {
                    finalDistance = distance;
                    finalIndex = i;
                }
            }
            else
            {
                finalIndex = i;
            }
        }
    }
    return _armors[finalIndex];
}

Armor ArmorDetect::detect(cv::Mat& inputImg)
{
    Armor get_Armor;
    cv::Mat binColor;
    cv::Mat binBright;
    cv::Mat srcImg;
    srcImg = inputImg.clone();
    setRoi(srcImg);
    AdjustColor(srcImg, binColor,BLUE);
    AdjustBrightness(srcImg, binBright);
    findLights(binColor, binBright,srcImg);
    if (findArmors(srcImg))
    {
        get_Armor = strikingDecision();
        cv::Point2f p[4];
        get_Armor.armorRotatedRect.points(p);
        float Armor_angle = get_Armor.armorRotatedRect.angle;
        float Armor_ratio =get_Armor.armorRotatedRect.size.width/
                           get_Armor.armorRotatedRect.size.height;

        for (int i = 0; i < 4; ++i)
        {
            cv::line(srcImg, p[i], p[(i + 1) % 4], cv::Scalar(0, 255, 0), 3);
            /*
            cv::putText(srcImg,cv::format(("%.0f %.0f"),Armor_angle,Armor_ratio),
                                p[2],FONT_HERSHEY_COMPLEX,0.5,Scalar(0,255,0));*/
        }
        imshow("5555",srcImg );
        get_Armor.armorRotatedRect.center.x += _detecArea.x;
        get_Armor.armorRotatedRect.center.y += _detecArea.y;
        _lastRotateRect = get_Armor.armorRotatedRect;
        w_->addPoint(_lastRotateRect.center.x,1);
        w_->plot();
        //cout<<_lastRotateRect.center.x<<endl;
        //cout<<_lastRotateRect.center.y<<endl;
        _lostCount = 0;
    }
    else
    {
        ++_lostCount;
        if (_lostCount < 8)
        {
            _lastRotateRect.size = cv::Size2f(_lastRotateRect.size.width, _lastRotateRect.size.height);
        }
        if (_lostCount == 9)
        {
            _lastRotateRect.size = cv::Size2f(_lastRotateRect.size.width * 1.5, _lastRotateRect.size.height * 1.5);
        }

        if (_lostCount == 12)
        {
            _lastRotateRect.size = cv::Size2f(_lastRotateRect.size.width * 2, _lastRotateRect.size.height * 2);
        }

        if (_lostCount == 15)
        {
            _lastRotateRect.size = cv::Size2f(_lastRotateRect.size.width * 1.5, _lastRotateRect.size.height * 1.5);
        }

        if (_lostCount == 18)
        {
            _lastRotateRect.size = cv::Size2f(_lastRotateRect.size.width * 1.5, _lastRotateRect.size.height * 1.5);
        }
        if (_lostCount > 33)
        {
            _lastRotateRect = cv::RotatedRect();
        }
    }

    _armorLights.clear();
    _armors.clear();
    return get_Armor;
}

void ArmorDetect::getcircle(cv::Mat &inputImg)
{
    Mat SrcImg;
    static std::vector<std::vector<cv::Point>>contours;
    static cv::Point2f rotateRectPoints[4];
    cv::Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
    SrcImg = inputImg.clone();
    vector<Vec3f>circles;
    cvtColor(SrcImg,SrcImg,COLOR_BGR2GRAY);
    threshold(SrcImg,SrcImg,150,180,THRESH_BINARY);
    imshow("55",SrcImg);
    HoughCircles(SrcImg,circles,HOUGH_GRADIENT,1.6,100,230,50,150,200);
    for(size_t i = 0; i < circles.size(); i++)//把霍夫变换检测出的圆画出来
    {
        Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);
        cv::circle( inputImg, center, 0, Scalar(255, 0, 255), -1, 8, 0 );
        cv::circle( inputImg, center, radius, Scalar(255, 255, 0), 1, 8, 0 );
        cout << cvRound(circles[i][0]) << "\t" << cvRound(circles[i][1]) << "\t"
             << cvRound(circles[i][2]) << endl;//在控制台输出圆心坐标和半径
    }
    imshow("特征提取",inputImg);
    //imwrite("dst.png", dst);
    waitKey(1);
}

