#include<opencv2/opencv.hpp>
#include "SerialCom.h"
#include <ctime>
#define STANDARDWIDTH 480
#define STANDARDHEIGHT 480
using namespace std;
using namespace cv;
Mat warp_mat;
void unevenLightCompensate(Mat image, int blockSize)
{
    if (image.channels() == 3) cvtColor(image, image, 7);
    double average = mean(image)[0];
    int rows_new = ceil(double(image.rows) / double(blockSize));
    int cols_new = ceil(double(image.cols) / double(blockSize));
    Mat blockImage;
    blockImage = Mat::zeros(rows_new, cols_new, CV_32FC1);
    for (int i = 0; i < rows_new; i++)
    {
        for (int j = 0; j < cols_new; j++) {
            int rowmin = i * blockSize;
            int rowmax = (i + 1) * blockSize;
            if (rowmax > image.rows) rowmax = image.rows;
            int colmin = j * blockSize;
            int colmax = (j + 1) * blockSize;
            if (colmax > image.cols) colmax = image.cols;
            Mat imageROI = image(Range(rowmin, rowmax), Range(colmin, colmax));
            double temaver = mean(imageROI)[0];
            blockImage.at<float>(i, j) = temaver;
        }
    }
    blockImage = blockImage - average;
    Mat blockImage2;
    resize(blockImage, blockImage2, image.size(), (0, 0), (0, 0), INTER_CUBIC);
    Mat image2;
    image.convertTo(image2, CV_32FC1);
    Mat dst = image2 - blockImage2;
    dst.convertTo(image, CV_32FC3);
}
void find_squares(Mat& image,Mat &imgThresholded, vector<vector<Point> >& squares,int &iLowH,int &iHighH,int &iLowS,int &iHighS,int &iLowV,int &iHighV)
{
	vector<vector<Point> > contours;
	Mat gray,gray2,gray3,imgHSV;
    vector<Mat> hsvSplit;

    threshold(image,gray,220,255,CV_THRESH_BINARY);
    cvtColor(gray,gray,CV_BGR2GRAY);
    cvtColor(image,imgHSV, COLOR_BGR2HSV);
    unevenLightCompensate(imgHSV,25);
    cvtColor(image,gray2, COLOR_BGR2GRAY);
    split(imgHSV, hsvSplit);
    equalizeHist(hsvSplit[2],hsvSplit[2]);
    merge(hsvSplit,imgHSV);

    inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded); //Threshold the image
    Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));

    morphologyEx(imgThresholded, imgThresholded, MORPH_OPEN, element);
    morphologyEx(imgThresholded, imgThresholded, MORPH_CLOSE, element);
    findContours(imgThresholded, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
	vector<Point> approx;
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);
		if (approx.size() == 4
			&&fabs(contourArea(Mat(approx))) > 1000
			&& fabs(contourArea(Mat(approx))) > (gray.cols-1)*(gray.rows-1)*0.1
			&& isContourConvex(approx))
		{
		    squares.push_back(approx);
		}
	}
}

void debugSquares(vector<vector<Point> > squares, Mat &image)
{
	for (int i = 0; i < squares.size(); i++)
	{
		drawContours(image, squares, i, Scalar(255, 0, 0), 1, 8, vector<Vec4i>(), 0, Point());
	}
}

void createStandard(Mat &imageThreshold,vector<vector<Point> > &contours)
{
    Point2f dstTri[4]={Point2f(0,STANDARDHEIGHT),Point2f(0,0),Point2f(STANDARDWIDTH,0),Point2f(STANDARDWIDTH,STANDARDHEIGHT)};
    Point2f srcTri[4];
    for(int i=0;i<contours.size();i++)
    {
        for(int j=0;j<4;j++)
        {
            int m=0;
            for(m=1;m<4;m++)
            {
                if(contours[i][j].x+contours[i][j].y>contours[i][(j+m)%4].x+contours[i][(j+m)%4].y)
                {
                    break;
                }
            }
            if(m==4)
                swap(contours[i][j],contours[i][1]);
            for(m=1;m<4;m++)
            {
                if(contours[i][j].x+contours[i][j].y<contours[i][(j+m)%4].x+contours[i][(j+m)%4].y)
                {
                    break;
                }
            }
            if(m==4)
                swap(contours[i][j],contours[i][3]);
        }
        if(contours[i][0].y<contours[i][2].y)
        {
            swap(contours[i][0],contours[i][2]);
        }
        for(int m=0;m<5;m++)
        {
            srcTri[m]=contours[i][m];
        }
    }
    warp_mat=getPerspectiveTransform(srcTri, dstTri);
    warpPerspective(imageThreshold,imageThreshold,warp_mat,Size(STANDARDWIDTH,STANDARDHEIGHT));
}
void intToString(unsigned char dst[],int start,int num)
{
    dst[start]=num/100+'0';
    dst[start+1]=num/10%10+'0';
    dst[start+2]=num%10+'0';
}
void find_circles(Mat &thresholded, Vec2i &circles)
{
    Mat dst = Mat::zeros(Size(STANDARDWIDTH,STANDARDHEIGHT),thresholded.type());
    warpPerspective(thresholded,dst,warp_mat,dst.size());
    int maxx=0,maxy=0,minx=10000,miny=10000;
    for(int i=30;i<dst.cols-30;i+=5)
    {
        for(int j=30;j<dst.rows-30;j+=5)
        {
            //cout<<"i="<<i<<"j="<<j<<endl;

            if(dst.at<uchar>(i,j)<=30)
            {
                if(maxx<i)maxx=i;
                if(maxy<j)maxy=j;
                if(minx>i)minx=i;
                if(miny>j)miny=j;
            }
        }
    }
    circles[0]=(maxx+minx)/2;
    circles[1]=(maxy+miny)/2;
    circle(dst,Point(circles[1],circles[0]),10, CV_RGB(255,0,0), 1);
}
int main()
{
	vector<vector<Point> > contours;
    Vec2i circles;
    circles[0]=STANDARDWIDTH/2;
    circles[1]=STANDARDHEIGHT/2;
	Mat video,dst,output,thresh;
    VideoCapture cap;
    SerialCom a;
    unsigned char m[6];
    int iLowH = 142;
    int iHighH = 180;
    int iLowS = 25;
    int iHighS = 124;
    int iLowV = 46;
    int iHighV = 255;
    int x,y;

    namedWindow("Control", CV_WINDOW_AUTOSIZE);

    cvCreateTrackbar("LowH", "Control", &iLowH, 179); //Hue (0 - 179)
    cvCreateTrackbar("HighH", "Control", &iHighH, 179);
    cvCreateTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
    cvCreateTrackbar("HighS", "Control", &iHighS, 255);
    cvCreateTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
    cvCreateTrackbar("HighV", "Control", &iHighV, 255);

    cap.open(0);
    cap>>video;

    a.init();
    a.open_port();
    while (!video.empty())
    {
        resize(video,video,Size(video.cols/2,video.rows/2));
        imshow("origion", video);
        contours.clear();
        find_squares(video,thresh, contours,iLowH,iHighH,iLowS,iHighS,iLowV,iHighV);
        debugSquares(contours, video);
        createStandard(video,contours);
        find_circles(thresh, circles);
        circle(video,Point((int)circles[1],(int)circles[0]),10, CV_RGB(255,0,0), 1);
        y=650*circles[0]/STANDARDWIDTH;
        x=650-650*circles[1]/STANDARDHEIGHT;
        intToString(m,0,x);
        intToString(m,3,y);
        //cout<<x<<"\t"<<y<<endl;
        // cout<<m<<endl;
        a.send(m,6);
        imshow("find", video);
        if (waitKey(0) == 27)
            break;
        cap>>video;
    }
    return 0;
}