#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include "armor/autodetection.h"

int main(int argc, char *argv[])
{
    ArmorDetect *b = new ArmorDetect;
    Mat img;
    VideoCapture frame;
    frame.open("/home/jasonyin111/ClionProjects/opencv/2.MOV");
    if(!frame.isOpened())
    {
        printf("errpr");
    }
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    b->w_ = &w;
    while (1)
    {
        frame>>img;
        b->detect(img);
        imshow("555",img);
        if(waitKey(1)==27)
        {
            break;
        }
    }
    delete b;
    return 0;

}