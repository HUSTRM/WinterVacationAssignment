#include "armorfind.h"
#include "serial.h"
#include<string>
//#include<fstream>
int color_type=0;
vector<vector<Point> >contours;
Mat undistorted;
void contour_get(Mat &input)
{
    /*Mat thres_whole;
    Mat contourkernel = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
    vector<Mat> splited;
    //blur(input,input,Size(3,3));
    split(input,splited);
    cvtColor(input,thres_whole,COLOR_BGR2GRAY);
    threshold(thres_whole,thres_whole,100,255,THRESH_BINARY);
    //red
    subtract(splited[2],splited[0],binary);
    //threshold(binary,binary,thres,255,THRESH_BINARY);
    /*blue
    subtract(splited[0],splited[2],binary);
    threshold(binary,binary,thres,255,THRESH_BINARY);
    dilate(binary,binary,contourkernel);
    binary=binary&thres_whole;*/
    //m_contours_bin.clear();
    //m_candidate_contour_bad.clear();
    Mat output,m_mat_yuv,m_mat_blue,m_mat_red,m_mat_gray,m_color_area,m_mat_bin;
    double cameraMatrix[9] = {
						1309.2   ,  0 , 936.7 ,
						0,  1318.9   ,558.3,
						0, 0,   1.0000
	};
	Mat CameraMatrix = Mat(3, 3, CV_64FC1, cameraMatrix);
	double distCoeffD[5] = { -0.3840 ,0.1363,0,0,0 };
	Mat distortion_coefficients = Mat(5, 1, CV_64FC1, distCoeffD);
	undistort(input, output, CameraMatrix, distortion_coefficients);
    output.copyTo(undistorted);
    vector<Mat> m_mat_yuv_channels;
    int _channels = input.channels();
    cv::cvtColor(output, m_mat_yuv, CV_BGR2YUV);
    cv::split(m_mat_yuv, m_mat_yuv_channels);
    m_mat_blue = m_mat_yuv_channels[1] - m_mat_yuv_channels[2];
    m_mat_red = m_mat_yuv_channels[2] - m_mat_yuv_channels[1];
    m_mat_gray = m_mat_yuv_channels[0];
    Mat m_element_erode = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5, 5));
    cv::Mat test_gray;
    if(color_type==0)
    test_gray = m_mat_red;
    else
    test_gray=m_mat_blue;
    cv::threshold(test_gray, m_color_area, 50, 255, 0);
    cv::threshold(m_mat_gray, m_mat_bin, 80, 255, CV_THRESH_BINARY);
    cv::dilate(m_color_area, m_color_area, m_element_erode);
    m_color_area = m_mat_bin & m_color_area;
    findContours(m_color_area,contours,CV_RETR_LIST,CV_CHAIN_APPROX_SIMPLE );
}
int thres;
Mat frame;
Mat output;
Mat image_binary;
SerialCom outputing;
Point3f signpoint2;
vector<point_data> DATA;
void chasing();
void anglefit();
Point3f result3d;
vector<position> location;
int sign1=-1;
position data1;
vector<position_catching> chase;
int sign2=0;
position_catching data2;
double dpitch=0,dyaw=0,droll=0;
int num=5;
double yaw,pitch,roll;
double predictx,predicty;
Point2f signcenter;
//float a[3];
//ofstream put;
int main()
{
    VideoCapture cap("/dev/video0");
    //double fps;
    //char string[10];
    //put.open("positiondata.txt",ios::app);
    //double t=0;
    //cap.set(CV_CAP_PROP_FPS,2);
    //cap.set(CV_CAP_PROP_BRIGHTNESS,2);
    outputing.open_port();
    while(cap.isOpened())
    {
        //t=getTickCount();
        cap>>frame;
        //t=double(getTickCount()-t)/getTickFrequency();
        //fps=1.0/t;
        //sprintf(string,"%.2f",fps);
        //std::string fpsString("FPS:");
        //fpsString+=string;
        /*putText(frame,
        fpsString,
        Point(5,20),
        FONT_HERSHEY_SIMPLEX,
        0.5,
        Scalar(255,0,0));*/
        if(frame.empty()){cout<<"frame is empty"<<endl;break;}
        /*Mat kernel=(Mat_<float>(3,3) <<0,-1,0,0,5,0,0,-1,0);
        filter2D(frame,frame,CV_8UC3,kernel);
        GaussianBlur(frame,frame,Size(3,3),0,0);*/
        contour_get(ref(frame));//red
        Mat output;
        vector<Point>result;
        ArmorFind Finder;
        DATA=Finder.process(contours,ref(undistorted),ref(output),ref(result),ref(image_binary));
        contours.clear();
        if(DATA.size()==0)
        {
            cout<<"no result-------------"<<endl;
            //location.clear();
            //sign1=-1;
            //chase.clear();
            //sign2=0;
        }
        else
        {
            anglefit();
            chasing();
            predictx=(signcenter.x)/cos(roll)*cos(roll+droll);
            predicty=(signcenter.y)/sin(roll)*sin(roll+droll);
            outputing.x=predictx;
            outputing.y=predicty;
            outputing.getdata();
            cout<<"ORIGIN("<<signcenter.x<<","<<signcenter.y<<")"<<endl;
            cout<<"OUTPUT("<<outputing.x<<","<<outputing.y<<")"<<endl;
        }
        dpitch=0;dyaw=0;droll=0;
        imshow("undistorted",output);
        DATA.clear();
        //cout<<"result:"<<result<<endl;
        char c=(char)waitKey(50);
        if(c==27)break;
    }
    cap.release();
    return 0;
}
void chasing()
{
    data1.pitch=pitch;
    data1.yaw=yaw;
    data1.roll=roll;
    if(sign1<num-1)
    {
        location.push_back(data1);
        if(sign1!=-1)
        {
            data2.detapitch=data1.pitch-location[sign1].pitch;
            data2.detayaw=data1.yaw-location[sign1].yaw;
            data2.detaroll=data1.roll-location[sign1].roll;
            chase.push_back(data2);
            sign2++;
        }
        sign1++;
    }
    else
    {
        vector<position>::iterator k=location.begin();
        location.erase(k);
        location.push_back(data1);
        if(sign2<num)
        {
            data2.detapitch=data1.pitch-location[sign1].pitch;
            data2.detayaw=data1.yaw-location[sign1].yaw;
            data2.detaroll=data1.roll-location[sign1].roll;
            chase.push_back(data2);
            sign2++;
        }
        else
        {
            data2.detapitch=data1.pitch-location[sign1].pitch;
            data2.detaroll=data1.roll-location[sign1].roll;
            data2.detayaw=data1.yaw-location[sign1].yaw;
            vector<position_catching>::iterator m=chase.begin();
            chase.erase(m);
            chase.push_back(data2);
        }
    }
    for(int i=0;i<chase.size();i++)
    {
        dpitch+=chase[i].detapitch;
        dyaw+=chase[i].detayaw;
        droll+=chase[i].detaroll;
    }
    dpitch/=chase.size();
    dyaw/=chase.size();
    droll/=chase.size();
}
void anglefit()
{
    Point2i signpoint1;
    double focuspitch,focusyaw;
    float minx;
    for(int i=0;i<DATA.size();i++)
    {
        signpoint1=Point2i(DATA[i].center.x-frame.cols/2,DATA[i].center.y-frame.rows/2);
        double f=130.8678;
        if(DATA[i].center.x>0)
        {
            yaw=atan(DATA[i].center.x/f);
        }
        else
        {
            yaw=atan(-DATA[i].center.x/f);
        }
        if(DATA[i].center.y<0)
        {
            pitch=atan(DATA[i].center.y/f);
        }
        else
        {
            pitch=atan(-DATA[i].center.y/f);
        }
        roll=atan(DATA[i].center.y/DATA[i].center.x);
        if(i==0)
        {
            focusyaw=yaw;
            focuspitch=pitch;
            minx=signpoint1.x;
            signcenter=signpoint1;
        }
        if(signpoint1.x<minx)
        {
            focusyaw=yaw;
            focuspitch=pitch;
            minx=DATA[i].center.x;
            signcenter=signpoint1;
        }
    }
}
