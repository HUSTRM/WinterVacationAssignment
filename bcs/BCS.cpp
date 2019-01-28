// BCS.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "SerialPort.h"
//#include "pch.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <cmath>
#include "SerialPort.cpp"
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void drawCross(InputOutputArray img, CvPoint point, CvScalar color, int size, int thickness);

int main()
{
	/** Initialize the parameters */
	Mat hsv,hsv_findball, Blur, Blur_findball, mask, mask_findball, rotated, board;
	//int lowH = 0, lowS = 140, lowV = 140, highH = 20, highS = 255, highV = 255; /** Orange */
	int lowH = 90, lowS = 60, lowV = 60, highH = 124, highS = 255, highV = 255; /** Blue */
	int LowH = 0, LowS = 5, LowV = 240, HighH = 180, HighS = 18, HighV = 255; /** White (HighS = 10) */
	//int LowH = 0, LowS = 180, LowV = 180, HighH = 10, HighS = 255, HighV = 255; /** Red */
	Point2f dst[4];
	Point2f src[4];

	/** Initialize the serial port */
	CSerialPort mySerialPort;
	if (!mySerialPort.InitPort(5, CBR_115200, 'N', 8, 1, EV_RXCHAR)) // right:5, left:3
	{
		cout << "initPort fail !" << endl;
	}
	else
	{
		cout << "initPort success !" << endl;
	}

	/** Open the camera */
	VideoCapture capture(0);
	//capture.set(CAP_PROP_EXPOSURE, -8);
	if (capture.isOpened())
	{
		Mat frame;
		cout << "Capture is opened" << endl;
		while (1)
		{
			 capture >> frame;
			 if (frame.empty())
			  	 break;
			 double time0 = static_cast<double>(getTickCount());

			 /** Undistort the board frame */
			 /** Blur the frame and convert it to the HSV color space */
	         Mat framecopy = frame.clone();
			 GaussianBlur(framecopy, Blur, Size(9, 9), 0);
			 //Canny(mid, canny, 67, 200, 3);
			 cvtColor(Blur, hsv, CV_BGR2HSV);
			 vector<Mat> hsvSplit;
			 split(hsv, hsvSplit);
			 equalizeHist(hsvSplit[2], hsvSplit[2]);
		 	 merge(hsvSplit, hsv);

			 /** Construct a mask for the mark points, then perform a series of dilations and erosions to remove any small blobs left in the mask */
			 inRange(hsv, Scalar(lowH, lowS, lowV), Scalar(highH, highS, highV), mask);
			 Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
			 erode(mask, mask, element, Point(-1, -1), 2);
			 dilate(mask, mask, element, Point(-1, -1), 2);
			 //erode(mask, mask, element, Point(-1, -1), 2);
			 //dilate(mask, mask, element, Point(-1, -1), 2);

			 /** Find contours of mark points in the mask */
			 vector<vector<Point>> contours;
			 vector<Vec4i> hierarchy;
			 Mat maskcopy = mask.clone();
			 findContours(maskcopy, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

			 /** Only proceed if three or four contours were found and pair src[] and dst[] in order for undistorting then */
			 switch (contours.size())
			 {
			 case 4:
				 for (int i = 0; i < 4; i++)
				 {
					 Point2f center;
					 float radius = 0;
					 minEnclosingCircle(contours[i], center, radius);
					 circle(frame, center, radius, Scalar(0, 255, 0), 1);
					 //cout << "(" << center.x << ", " << center.y << ")" << endl;
					 src[i].x = center.x;
					 src[i].y = center.y;
				 }
				 for (int i = 0; i < 4; i++)
				 {
					 if (src[i].x > 240)
						 dst[i].x = 480;
					 else
						 dst[i].x = 0;
					 if (src[i].y > 240)
						 dst[i].y = 480;
					 else
						 dst[i].y = 0;
				 }
				 break;
			 case 3:
				 for (int i = 0; i < 3; i++)
				 {
					 Point2f center;
					 float radius = 0;
					 minEnclosingCircle(contours[i], center, radius);
					 circle(frame, center, radius, Scalar(0, 255, 0), 1);
					 src[i].x = center.x;
					 src[i].y = center.y;
				 }
				 for (int i = 0; i < 3; i++)
				 {
					 if (src[i].x > 240)
						 dst[i].x = 480;
					 else
						 dst[i].x = 0;
					 if (src[i].y > 240)
						 dst[i].y = 480;
					 else
						 dst[i].y = 0;
				 }
				 dst[3].x = 960 - dst[0].x - dst[1].x - dst[2].x;
				 dst[3].y = 960 - dst[0].y - dst[1].y - dst[2].y;
				 if((dst[3].x == 480) && (dst[3].y == 480))
					 src[3] = Point2f(489, 442);
				 if ((dst[3].x == 0) && (dst[3].y == 480))
					 src[3] = Point2f(110, 440);
				 if ((dst[3].x == 0) && (dst[3].y == 0))
				 	 src[3] = Point2f(110, 50);
				 if ((dst[3].x == 480) && (dst[3].y == 0))
				 	 src[3] = Point2f(498, 59);
				 break;
			 case 2:
				 for (int i = 0; i < 2; i++)
				 {
					 Point2f center;
					 float radius = 0;
					 minEnclosingCircle(contours[i], center, radius);
					 circle(frame, center, radius, Scalar(0, 255, 0), 1);
					 src[i].x = center.x;
					 src[i].y = center.y;
				 }
				 for (int i = 0; i < 2; i++)
				 {
					 if (src[i].x > 240)
						 dst[i].x = 480;
					 else
						 dst[i].x = 0;
					 if (src[i].y > 240)
						 dst[i].y = 480;
					 else
						 dst[i].y = 0;
				 }
				 if (src[0].y < 240 && src[1].y < 240)
				 {
					 dst[2] = Point2f(0, 480);
					 dst[3] = Point2f(480, 480);
					 src[2] = Point2f(110, 440);
					 src[3] = Point2f(489, 442);
				 }
				 if (src[0].x > 240 && src[1].x > 240)
				 {
					 dst[2] = Point2f(0, 0);
					 dst[3] = Point2f(0, 480);
					 src[2] = Point2f(100, 50);
					 src[3] = Point2f(110, 440);
				 }
				 if (src[0].y > 240 && src[1].y > 240)
				 {
					 dst[2] = Point2f(0, 0);
					 dst[3] = Point2f(480, 0);
					 src[2] = Point2f(100, 50);
					 src[3] = Point2f(498, 59);
				 }
				 if (src[0].x < 240 && src[1].x < 240)
				 {
					 dst[2] = Point2f(480, 0);
					 dst[3] = Point2f(480, 480);
					 src[2] = Point2f(498, 59);
					 src[3] = Point2f(489, 442);
				 }
				 if((src[0].x > 240 && src[0].y < 240) && (src[1].x < 240 && src[1].y > 240))
				 {
					 dst[2] = Point2f(0, 0);
					 dst[3] = Point2f(480, 480);
					 src[2] = Point2f(100, 50);
					 src[3] = Point2f(489, 442);
				 }
				 if ((src[0].x < 240 && src[0].y < 240) && (src[1].x > 240 && src[1].y > 240))
				 {
					 dst[2] = Point2f(480, 0);
					 dst[3] = Point2f(0, 480);
					 src[2] = Point2f(498, 59);
					 src[3] = Point2f(110, 440);
				 }
				 break;
			 case 1:
				 Point2f center;
				 float radius = 0;
				 minEnclosingCircle(contours[0], center, radius);
				 circle(frame, center, radius, Scalar(0, 255, 0), 1);
				 src[0].x = center.x;
				 src[0].y = center.y;
				 if (src[0].x > 240)
					 dst[0].x = 480;
				 else
					 dst[0].x = 0;
				 if (src[0].y > 240)
					 dst[0].y = 480;
				 else
					 dst[0].y = 0;
				 if (src[0].x < 240 && src[0].y < 240)
				 {
					 dst[1] = Point2f(480, 480);
					 dst[2] = Point2f(480, 0);
					 dst[3] = Point2f(0, 480);
					 src[1] = Point2f(489, 442);
					 src[2] = Point2f(498, 59);
					 src[3] = Point2f(110, 440);
				 }
				 if (src[0].x > 240 && src[0].y < 240)
				 {
					 dst[1] = Point2f(480, 480);
					 dst[2] = Point2f(0, 0);
					 dst[3] = Point2f(0, 480);
					 src[1] = Point2f(489, 442);
					 src[2] = Point2f(100, 50);
					 src[3] = Point2f(110, 440);
				 }
				 if (src[0].x < 240 && src[0].y > 240)
				 {
					 dst[1] = Point2f(480, 480);
					 dst[2] = Point2f(480, 0);
					 dst[3] = Point2f(0, 0);
					 src[1] = Point2f(489, 442);
					 src[2] = Point2f(498, 59);
					 src[3] = Point2f(100, 50);
				 }
				 if (src[0].x > 240 && src[0].y > 240)
				 {
					 dst[1] = Point2f(0, 0);
					 dst[2] = Point2f(480, 0);
					 dst[3] = Point2f(0, 480);
					 src[1] = Point2f(100, 50);
					 src[2] = Point2f(498, 59);
					 src[3] = Point2f(110, 440);
				 }
				 break;
			 }

			 /** Perform perspective transformation to undistort */
			 Mat warpMatrix = getPerspectiveTransform(src, dst);
			 warpPerspective(framecopy, rotated, warpMatrix, rotated.size(), INTER_LINEAR, BORDER_CONSTANT);
			 board = rotated(Range(0, 480), Range(0, 480));

			 /** Find and track the ball */
			 /** Blur the undistorted board frame and convert it to the HSV color space */
			 //resize(frame, frame, Size(640, 480));
			 GaussianBlur(board, Blur_findball, Size(9, 9), 0);
			 cvtColor(Blur_findball, hsv_findball, CV_BGR2HSV);
				 
			 /** Construct a mask for the color "white", then perform a series of dilations and erosions to remove any small blobs left in the mask */
			 vector<Mat> hsvSplit_findball;
			 split(hsv_findball, hsvSplit_findball);
			 equalizeHist(hsvSplit_findball[2], hsvSplit_findball[2]);
		 	 merge(hsvSplit_findball, hsv_findball);
			 inRange(hsv_findball, Scalar(LowH, LowS, LowV), Scalar(HighH, HighS, HighV), mask_findball);
		 	 Mat element_findball = getStructuringElement(MORPH_RECT, Size(3, 3));
			 erode(mask_findball, mask_findball, element_findball, Point(-1, -1), 2);
			 dilate(mask_findball, mask_findball, element_findball, Point(-1, -1), 2);
			 //erode(mask_findball, mask_findball, element_findball, Point(-1, -1), 2);
			 //dilate(mask_findball, mask_findball, element_findball, Point(-1, -1), 2);

			 /** Find contours in the mask */
			 vector<vector<Point>> contours_findball;
			 vector<Vec4i> hierarchy_findball;
		 	 Mat maskcopy_findball = mask_findball.clone();
			 findContours(maskcopy_findball, contours_findball, hierarchy_findball, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

			 /** Only proceed if at least one contour was found */
			 if (contours_findball.size() > 0)
			 {
				 /** Find the contour of ball in the mask, then use it to compute the minimum enclosing circle and the centroid and draw them out */
				 for (int i = 0; i < contours_findball.size(); i++)
				 {
					 /** Get the contour of ball */
					 double area = contourArea(contours_findball[i], false);
					 //cout << "area: " << area << endl;
					 if (area < 800 && area > 300)//50 200  //500 1000
					 {
						 Point2f center_findball;
						 float radius_findball = 0;
						 minEnclosingCircle(contours_findball[i], center_findball, radius_findball);
						 //circle(rotated, center_findball, radius_findball, Scalar(0, 255, 255), 1);
                             
						 /** Draw the centroid of the largest contour */
				         vector<Moments> M;
				         M.push_back(moments(contours_findball[i], false));
				         Point2f centroid = Point2f(M[0].m10 / M[0].m00, M[0].m01 / M[0].m00);
				         //cout << "error: ( " << fabs(center_findball.x - centroid.x) << ", " << fabs(center_findball.y - centroid.y) << ")" << endl;

						 /*Send the centroid coordinate to the Serial Interface*/
						 if(centroid.y < 450 && centroid.y > 20 && centroid.x > 20 && centroid.x < 450)
						 //if(!(centroid.y > 450 && centroid.x > 450))
						 {
							 cout << "area of contour: " << area << endl;
							 drawCross(rotated, centroid, Scalar(255, 255, 0), 5, 3);
							 drawCross(rotated, center_findball, Scalar(240, 255, 255), 5, 1);
							 circle(rotated, Point(75, 75), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(75, 240), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(75, 405), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(240, 75), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(240, 240), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(240, 405), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(405, 75), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(405, 240), 19, Scalar(255, 255, 255), 1);
							 circle(rotated, Point(405, 405), 19, Scalar(255, 255, 255), 1);
							 int x = (int)round(centroid.x);
						     int y = (int)round(centroid.y);
							 //int x = (int)round(center_findball.x);
							 //int y = (int)round(center_findball.y);
						     unsigned char x1 = x >> 8;
					 	     unsigned char x2 = x;
						     unsigned char y1 = y >> 8;
						     unsigned char y2 = y;
							 cout << "centorid of contour: (" << round(centroid.x) << ", " << round(centroid.y) << ") " << endl;
						     cout << "center of MEC: (" << round(center_findball.x)  << ", " << round(center_findball.y) << ")" << endl;
							 cout << "output: (" << x << ", " << y << ")" << endl;
						     unsigned char place[4] = { x1, x2, y1, y2 };
						     cout << mySerialPort.WriteData(place, 4) << endl;
						     //cout << mySerialPort.GetBytesInCOM() << endl;
						 }
					 }
				 }
			 }
			
			 /** Show the result */
			 time0 = ((double)getTickCount() - time0) / getTickFrequency();
			 namedWindow("mask");
			 imshow("mask", mask);
			 namedWindow("mask_findball");
			 imshow("mask_findball", mask_findball);
			 namedWindow("Original Image");
			 imshow("Original Image", frame);
			 namedWindow("Warp Perspective");
			 imshow("Warp Perspective", board);
			 cout << "time: " << time0 << "s" << endl;
			 cout << endl;
			 waitKey(20);
		 }
	}
	else
	{
		cout << "No capture" << endl;
		waitKey(0);
	}
	return 0;
}

void drawCross(InputOutputArray img, CvPoint point, CvScalar color, int size, int thickness)
{
	/** Draw the level */
	line(img, cvPoint(point.x - size / 2, point.y), cvPoint(point.x + size / 2, point.y), color, thickness, 8, 0);
	/** Draw the vertical line */
	line(img, cvPoint(point.x, point.y - size / 2), cvPoint(point.x, point.y + size / 2), color, thickness, 8, 0);
	return;
}