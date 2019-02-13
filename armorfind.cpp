#include "armorfind.h"
ArmorFind::ArmorFind()
{
    kernel = getStructuringElement(MORPH_ELLIPSE,Size(9,9));
}
/*bool ArmorFind::picture_ensure(Mat img,RotatedRect RRect)//red
{
    Point2f _pt[4];
    Point2f pt[4];
    RRect.points(_pt);
    for(int i=0;i<4;i++)
    {
        pt[i].x=(int)_pt[i].x;
        pt[i].y=(int)_pt[i].y;
    }
    float pro=0.1;
    Point2f sign1,sign2,sign3,sign4,sign5;
    int xlen,ylen,xexp,yexp;
    xlen=pro*fabs(pt[1].x-pt[3].x);
    ylen=pro*fabs(pt[0].y-pt[2].y);
    xexp=pro*fabs(pt[0].x-pt[2].x);
    yexp=pro*fabs(pt[1].y-pt[3].y);
    if(xexp<xlen)
    {
        if(pt[1].x>pt[3].x)
        {
            sign1=Point(pt[1].x+xlen/2,pt[1].y+yexp/2);
            sign2=Point(pt[3].x-xlen/2,pt[3].y-yexp/2);
            sign5.x=pt[3].x+xlen/2;
        }
        else
        {
            sign1=Point(pt[3].x+xlen/2,pt[3].y+yexp/2);
            sign2=Point(pt[1].x-xlen/2,pt[1].y-yexp/2);
            sign5.x=pt[1].x+xlen/2;
        }
        if(pt[0].y>pt[2].y)
        {
            sign3=Point(pt[0].x-xexp/2,pt[0].y+ylen/2);
            sign4=Point(pt[2].x+xexp/2,pt[2].y-ylen/2);
            sign5.y=pt[2].y+ylen/2;
        }
        else
        {
            sign3=Point(pt[2].x-xexp/2,pt[2].y+ylen/2);
            sign4=Point(pt[0].x+xexp/2,pt[0].y-ylen/2);
            sign5.y=pt[0].y+ylen/2;
        }
    }
    else
    {
        if(pt[0].x>pt[2].x)
        {
            sign1=Point(pt[0].x+xlen/2,pt[0].y+yexp/2);
            sign2=Point(pt[2].x-xlen/2,pt[2].y-yexp/2);
            sign5.x=pt[2].x+xlen/2;
        }
        else
        {
            sign1=Point(pt[2].x+xlen/2,pt[2].y+yexp/2);
            sign2=Point(pt[0].x-xlen/2,pt[0].y-yexp/2);
            sign5.x=pt[0].x+xlen/2;
        }
        if(pt[1].y>pt[3].y)
        {
            sign3=Point(pt[1].x-xexp/2,pt[1].y+ylen/2);
            sign4=Point(pt[3].x+xexp/2,pt[3].y-ylen/2);
            sign5.y=pt[3].y+ylen/2;
        }
        else
        {
            sign3=Point(pt[3].x-xexp/2,pt[3].y+ylen/2);
            sign4=Point(pt[1].x+xexp/2,pt[1].y-ylen/2);
            sign5.y=pt[1].y+ylen/2;
        }
    }
    vector<Mat>splited;
    split(img,splited);
    if(splited[0].at<uchar>(sign5.x,sign5.y) >10||splited[1].at<uchar>(sign5.x,sign5.y) >10||
    splited[2].at<uchar>(sign5.x,sign5.y) <130)
    return false;
    if(splited[2].at<uchar>(sign1.x,sign1.y) >130||splited[2].at<uchar>(sign2.x,sign2.y) >130
    ||splited[2].at<uchar>(sign3.x,sign3.y) >130||splited[2].at<uchar>(sign4.x,sign4.y) >130)
    return false;
    return true;
}*/
vector<point_data> DATA1;
point_data signpoint1;
/*bool ArmorFind::picture_ensure(Mat img,RotatedRect RRect)//red
{
    vector<Mat> splited;
    split(img,splited);
    /*if(splited[0].at<uchar>(RRect.center)>200)
    return false;
    if(splited[1].at<uchar>(RRect.center)>200)
    return false;
    if(splited[2].at<uchar>(RRect.center)>100)
    return true;
    Point2f points[4];
    RRect.points(points);
    Point2f signpoint3,signpoint4;
    signpoint3.x=(points[0].x+points[2].x)/2;
    signpoint3.y=(points[0].y+points[2].y)/2;
    signpoint4.x=(points[1].x+points[3].x)/2;
    signpoint4.y=(points[1].y+points[3].y)/2;
    if(int(splited[0].at<uchar>(RRect.center))<int(splited[2].at<uchar>(RRect.center))
    &&int(splited[2].at<uchar>(RRect.center))>int(splited[1].at<uchar>(RRect.center))
    &&(int(splited[2].at<uchar>(RRect.center))-int(splited[0].at<uchar>(RRect.center)))>30
    &&(int(splited[2].at<uchar>(RRect.center))-int(splited[0].at<uchar>(RRect.center)))>30
    &&int(splited[0].at<uchar>(signpoint3))<int(splited[2].at<uchar>(signpoint3))
    &&int(splited[2].at<uchar>(signpoint3))>int(splited[1].at<uchar>(signpoint3))
    &&int(splited[0].at<uchar>(signpoint4))<int(splited[2].at<uchar>(signpoint4))
    &&int(splited[2].at<uchar>(signpoint4))>int(splited[1].at<uchar>(signpoint4)))
    return true;
    else
    return false;
}*/
 vector<point_data> ArmorFind::process(vector<vector<Point> > contours,Mat &input,Mat &output,vector<Point> &result,Mat &image_binary)
 {
     vector<point_data> none;
     Clear();
     output=input.clone();
     RotatedRect RRect;
     for(int i=0;i<contours.size();i++){
        RRect = minAreaRect(contours[i]);
        if(((fabs(RRect.angle) < 45.0 && RRect.size.height > RRect.size.width)
                || (fabs(RRect.angle) > 45.0 && RRect.size.width > RRect.size.height))/*&& picture_ensure(input,RRect)*/
                ){
                RectfirstResult.push_back(RRect);
        }
    }
    if(RectfirstResult.size() < 2){
        ArmorCenters.clear();
        return none;
    }
    sort(RectfirstResult.begin(),RectfirstResult.end(),RotateRectSort);
    GetArmorLights();
    sort(RectResults.begin(),RectResults.end(),RotateRectSort);
    for(int i=0;i<RectResults.size();i++){
        /*vector<Mat> splited;
        split(output,splited);
        cout<<"pixel value 0:"<<int(splited[0].at<uchar>(RectResults[i].center.y,RectResults[i].center.x))<<endl;
        cout<<"pixel value 1:"<<int(splited[1].at<uchar>(RectResults[i].center.y,RectResults[i].center.x))<<endl;
        cout<<"pixel value 2:"<<int(splited[2].at<uchar>(RectResults[i].center.y,RectResults[i].center.x))<<endl;*/
        //std::cout<<"("<<RectResults[i].center.x<<","<<RectResults[i].center.y<<")"<<std::endl;
        ellipse(output,RectResults[i],Scalar(255,0,0),2);
        /*ostringstream ss;
        ss<<i;
        putText(output,ss.str(),Point(RectResults[i].center.x,RectResults[i].center.y-5),FONT_HERSHEY_COMPLEX_SMALL, 1, Scalar(0,255,255));
        */
    }
    GetArmors();
    for(int i=0;i<ArmorCenters.size();i++){
        DrawCross(output,ArmorCenters[i],20,Scalar(255,0,255),2);
        //imshow("TARGET",output);
    }
    result = ArmorCenters;
    return DATA1;
 }
 void ArmorFind::DrawCross(Mat &img,Point center,int size,Scalar color,int thickness){
    Point L1,L2,B1,B2;
    int xL = center.x - size > 0 ? center.x - size:0;
    int xR = center.x + size;
    int yL = center.y - size > 0 ? center.y - size:0;
    int yR = center.y + size;
    L1 = Point(xL,center.y);
    L2 = Point(xR,center.y);
    B1 = Point(center.x,yL);
    B2 = Point(center.x,yR);
    vector<Point>points;
    points.push_back(L1);
    points.push_back(L2);
    points.push_back(B1);
    points.push_back(B2);
    RotatedRect signRect=minAreaRect(Mat(points));
    ellipse(img,signRect,Scalar(0,255,0),2);
    line(img,L1,L2,color,thickness);
    line(img,B1,B2,color,thickness);
}
 void ArmorFind::GetArmorLights()
 {
     size_t size = RectfirstResult.size();
    vector<RotatedRect> Groups;
    int cellmaxsize;
    Groups.push_back(RectfirstResult[0]);
    cellmaxsize = RectfirstResult[0].size.height * RectfirstResult[0].size.width;
    if(cellmaxsize > 2500) cellmaxsize = 0;
    int maxsize;
    for(int i=1;i<size;i++){
        if(RectfirstResult[i].center.x - RectfirstResult[i-1].center.x <10){
            maxsize = RectfirstResult[i].size.height * RectfirstResult[i].size.width;
            if(maxsize > 2500) continue;
            if(maxsize > cellmaxsize) cellmaxsize = maxsize;
            Groups.push_back(RectfirstResult[i]);
        }else{
            Armorlists.push_back(Groups);
            CellMaxs.push_back(cellmaxsize);
            cellmaxsize = 0;
            maxsize = 0;
            Groups.clear();
            //if(RectfirstResult[i].size.height * RectfirstResult[i].size.width > 2500) continue;
            Groups.push_back(RectfirstResult[i]);
            cellmaxsize = RectfirstResult[i].size.height * RectfirstResult[i].size.width;
        }
        }
    Armorlists.push_back(Groups);
    CellMaxs.push_back(cellmaxsize);
    size = Armorlists.size();
    for(int i=0;i<size;i++){
        int Gsize = Armorlists[i].size();
        int GroupMax = CellMaxs[i];
        if(GroupMax > 5){
            for(int j=0;j<Gsize;j++){
                maxsize = Armorlists[i][j].size.height * Armorlists[i][j].size.width;
                if(maxsize == GroupMax){
                    RectResults.push_back(Armorlists[i][j]);
                }
            }
        }
    }
}
void ArmorFind::GetArmors()
{
    size_t size = RectResults.size();
    ArmorOldCenters = ArmorCenters;
    ArmorCenters.clear();
    if(size < 2){
        return;
    }
    Point2f L1,L2;
    float K,angleabs = 0.0,angleL1,angleL2;
    float divscale,areaL1,areaL2;
    float ydis = 0;
    float maxangle,xdis,heightmax,hwdiv;
    Point2f _pt[4],pt[4];
    auto ptangle = [](const Point2f &p1,const Point2f &p2){
        return fabs(atan2(p2.y-p1.y,p2.x-p1.x)*180.0/CV_PI);
    };
    auto GetAreaofp3 = [](const Point2f &p1,const Point2f &p2,const Point2f &p3){
        Mat matrix = (Mat_<double>(3,3)<<p1.x,p2.y,1,p2.x,p2.y,1,p3.x,p3.y,1);
        return 0.5*determinant(matrix);
    };

    for(int i=0;i<size-1;i++){
        angleL1 = fabs(RectResults[i].angle);
        L1 = RectResults[i].center;
        areaL1 = RectResults[i].size.height * RectResults[i].size.width;
        RectResults[i].points(_pt);
         /*pt
          * 0 2
          * 1 3
          * */
        if(angleL1 > 45.0){
         pt[0] = _pt[3];
         pt[2] = _pt[0];
        }else{
         pt[0] = _pt[2];
         pt[2] = _pt[3];
        }
        for(int j=i+1;j<size;j++){
            L2 = RectResults[j].center;
            if(L1.x != L2.x){
                K = GetK(L1,L2);
                if(L1.y > L2.y){
                    ydis = L1.y - L2.y;
                }else{
                    ydis = L2.y - L1.y;
                }
                areaL2 = RectResults[j].size.height * RectResults[j].size.width;
                if(areaL1 > areaL2){
                    divscale = areaL1 / areaL2;
                }else{
                    divscale = areaL2 / areaL1;
                }
                angleL2 = fabs(RectResults[j].angle);

                RectResults[j].points(_pt);
                if(angleL2 > 45.0){
                 pt[2] = _pt[2];
                 pt[3] = _pt[2];
                }else{
                 pt[2] = _pt[2];
                 pt[3] = _pt[0];
                }
                float a=0,b=0,s;
                if(angleL1>45)
                a+=RectResults[i].size.width/2;
                else
                a+=RectResults[i].size.height/2;
                if(angleL2>45)
                a+=RectResults[j].size.width/2;
                else
                a+=RectResults[j].size.height/2;
                b=sqrt((RectResults[i].center.x-RectResults[j].center.x)*(RectResults[i].center.x-RectResults[j].center.x)
                +(RectResults[i].center.y-RectResults[j].center.y)*(RectResults[i].center.y-RectResults[j].center.y));
                //float div1=a/55,div2=b/130,div3=a*b/(55*130);&&div1*div2/div3>0.8&&div1*div2/div3<1.4
                //maxangle = MAX(ptangle(pt[0],pt[2]),ptangle(pt[2],pt[3]));
                //std::cout<<"angle:"<<maxangle<<std::endl;
               // maxangle = 0;
                /*if(angleL1 > 45.0 && angleL2 < 45.0){
                    angleabs = 90.0 - angleL1 + angleL2;
                }else if(angleL1 <= 45.0 && angleL2 >= 45.0){
                    angleabs = 90.0 - angleL2 + angleL1;
                }else{
                    if(angleL1 > angleL2) angleabs = angleL1 - angleL2;
                    else angleabs = angleL2 - angleL1;
                }*/
                //xdis = fabs(L1.x - L2.x);
                //heightmax =MAX(MAX(RectResults[i].size.width,RectResults[j].size.width),MAX(RectResults[i].size.height,RectResults[j].size.height));
                //hwdiv = xdis/heightmax;
                if( divscale < 2){///*&& hwdiv < 10.0*/&& maxangle < 20.0fabs(K) < 0.5 && &&&& ydis < 0.5*heightmax   &&a/b>0.3&&a/b<1.7
                        //if(ismono){
                            //float armor_area = GetAreaofp3(pt[0],pt[2],pt[2]) + GetAreaofp3(pt[2],pt[2],pt[3]);
                            //std::cout<<"area:"<<armor_area<<std::endl;
                            Monodata pushdata;
                            //pushdata.area = armor_area;
                            pushdata.center = Point(0.5*(L1.x+L2.x),0.5*(L1.y+L2.y));
                            //if(hwdiv > 5.0){
                                //pushdata.armor = pushdata.big_armor;
                            //}else{
                                //pushdata.armor = pushdata.small_armor;
                            //}
                            monodata.push_back(pushdata);
                            ArmorCenters.push_back(Point(0.5*(L1.x+L2.x),0.5*(L1.y+L2.y)));
                            signpoint1.Rect1=RectResults[i];
                            signpoint1.Rect2=RectResults[j];
                            signpoint1.point1=L1;
                            signpoint1.point2=L2;
                            signpoint1.center=Point(0.5*(L1.x+L2.x),0.5*(L1.y+L2.y));
                            DATA1.push_back(signpoint1);
                    }
                }
            }
        }
    }
double ArmorFind::GetK(Point2f L1,Point2f L2){
    return (L1.y - L2.y) / (L1.x - L2.x);
}
void ArmorFind::Clear(void){
    monodata.clear();
    CellMaxs.clear();
    Armorlists.clear();
    RectfirstResult.clear();
    RectResults.clear();
    centers.clear();
    DATA1.clear();
}