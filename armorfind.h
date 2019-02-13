#ifndef ARMORFIND_H
#define ARMORFIND_H
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
typedef struct{
    cv::Point center;
}Monodata;
struct point_data
{
    Point2f point1;
    Point2f point2;
    Point2f center;
    RotatedRect Rect1;
    RotatedRect Rect2;
};
struct position_catching
{
    double detapitch;
    double detayaw;
    double detaroll;
};
struct position
{
    double yaw;
    double pitch;
    double roll;
};
static inline bool RotateRectSort(RotatedRect a1,RotatedRect a2){
    return a1.center.x < a2.center.x;
}
class ArmorFind
{
public:
    ArmorFind();
    vector<point_data> process(vector<vector<Point> > contours,Mat &input,Mat &output,vector<Point> &result,Mat &image_binary);
    void adjustRect(cv:: RotatedRect &rect);
private:
    void Clear();
    bool picture_ensure(Mat img,RotatedRect RRect);
    void GetArmorLights();
    void GetArmors();
    double GetK(Point2f L1,Point2f L2);
    void DrawCross(Mat &img,Point center,int size,Scalar color,int thickness = 2);
public:
    vector<Point> ArmorCenters,ArmorOldCenters;
    vector<Monodata> monodata;
private:
    Mat kernel;
    vector<vector<RotatedRect> > Armorlists;
    vector<int> CellMaxs;
    vector<Mat> splited;
    vector<Point> centers;
    vector<RotatedRect> RectfirstResult,RectResults;
    Mat armor_roi;
};
#endif