/*
 * @file        problem3-1.cpp
 * @author      zhaoqicheng(zhaoqicheng2022@163.com/623146682)
 * @brief       装甲板识别
 * @details     完成装甲板识别，在原图上标注中心坐标，输出每一步处理后的图片
 * @date        2022-4-27
 * @copyright   All create by zhaoqicheng.
 */
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>
#include <cmath>

using namespace std;
using namespace cv;

//函数声明
RotatedRect& adjustRec(cv::RotatedRect& rec, const int mode);  //筛选预处理

//限幅滤波，防止其他装甲板的干扰
int filter(int effective_value, int new_value, int delat_max);
int filter(int effective_value, int new_value,int delat_max) {
    if ( ( new_value - effective_value > delat_max ) || ( effective_value - new_value > delat_max )) {
        new_value=effective_value;
        return effective_value;
    }
    else {
        new_value=effective_value;
        return new_value;
    }
}

int main() {
    //读取图片
    Mat image = imread("/home/jichuan-ftp/armour.jpg",IMREAD_COLOR);
    //判断图片是否为空
    if (image.empty()) {
        cout << "Could not open or find the image" <<endl;
        return -1;
    }

    //将彩色图像转换为灰度图
    Mat image_gray;  //创建一张灰度图
    cvtColor(image,image_gray,COLOR_BGR2GRAY);
    namedWindow("image_gray",WINDOW_NORMAL );
    imshow("image_gray",image_gray);

    //将灰度图转换为二值图
    Mat image_thres;  //创建一张二值图
    threshold(image_gray,image_thres, 110, 255, THRESH_BINARY);
    namedWindow("image_thres",WINDOW_NORMAL );
    imshow("image_thres",image_thres);

    //对图片进行通道分离
    Mat image_splited[3];
    split(image, image_splited);

    //进行通道相减
    Mat image_subtracted;
    subtract(image_splited[0], image_splited[2], image_subtracted);  //学长发的是蓝色灯条，那就B-R
    namedWindow("mage_subtracted",WINDOW_NORMAL );
    imshow("mage_subtracted",image_subtracted);

    //对通道相减后的图像进行二值化操作,同时进行膨胀操作
    threshold(image_subtracted,image_subtracted, 110, 255, THRESH_BINARY);
    Mat element1 = getStructuringElement(MORPH_RECT, Size(10, 10));
    dilate(image_subtracted, image_subtracted, element1);
    namedWindow("mage_subtracted_dilate_1",WINDOW_NORMAL );
    imshow("mage_subtracted_dilate_1",image_subtracted);

    //得到包含目标颜色且较亮的区域
    Mat ret;
    ret = image_thres & image_subtracted;
    Mat element2 = getStructuringElement(MORPH_RECT, Size(1, 1));
    dilate(ret, ret, element2);
    namedWindow("mage_subtracted_dilate_2",WINDOW_NORMAL );
    imshow("mage_subtracted_dilate_2",ret);

    //寻找轮廓
    vector<vector<Point>>contours;  //保存轮廓的点阵
    vector<Vec4i>hierarchy;
    findContours(ret,contours,hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE,Point(0,0));

    //筛选轮廓
    vector<RotatedRect> vc;
    vector<RotatedRect> vRec;
    for (int i = 0;i <(int) contours.size();i++) {
        //求轮廓面积
        float Light_Contour_Area = contourArea(contours[i]);
        //去除较小轮廓fitllipse的限制条件
        if (Light_Contour_Area < 15 || contours[i].size() <= 10)
            continue;
        // 用椭圆拟合区域得到外接矩形
        RotatedRect Light_Rec = fitEllipse(contours[i]);
        Light_Rec = adjustRec(Light_Rec,1);

        if (Light_Rec.angle > 10 )
            continue;
        // 长宽比和轮廓面积比限制
        if (Light_Rec.size.width / Light_Rec.size.height > 1.5 || Light_Contour_Area / Light_Rec.size.area() < 0.5)
            continue;
        // 扩大灯柱的面积
        Light_Rec. size.height *= 1.1;
        Light_Rec.size.width *= 1.1;
        vc.push_back(Light_Rec);
    }

    //灯条匹配
    for (size_t i = 0; i < vc.size(); i++) {
        for (size_t j = i + 1; (j < vc.size()); j++) {
         //判断是否为相同灯条
            float Contour_angle = abs(vc[i].angle - vc[j].angle); //角度差
            if (Contour_angle >= 7)
                continue;
            //长度差比率
            float Contour_Len1 = abs(vc[i].size.height - vc[j].size.height) / max(vc[i].size.height, vc[j].size.height);
            //宽度差比率
            float Contour_Len2 = abs(vc[i].size.width - vc[j].size.width) / max(vc[i].size.width, vc[j].size.width);
            if (Contour_Len1 > 0.25 || Contour_Len2 > 0.25)
                continue;

            RotatedRect ARMOR;
            ARMOR.center.x = (vc[i].center.x + vc[j].center.x) / 2.; //x坐标
            ARMOR.center.y = (vc[i].center.y + vc[j].center.y) / 2.; //y坐标
            ARMOR.angle = (vc[i].angle + vc[j].angle) / 2.; //角度
            float nh, nw, yDiff, xDiff;
            nh = (vc[i].size.height + vc[j].size.height) / 2; //高度
            // 宽度
            nw = sqrt((vc[i].center.x - vc[j].center.x) * (vc[i].center.x - vc[j].center.x) + (vc[i].center.y - vc[j].center.y) * (vc[i].center.y - vc[j].center.y));
            float ratio = nw / nh; //匹配到的装甲板的长宽比
            xDiff = abs(vc[i].center.x - vc[j].center.x) / nh; //x差比率
            yDiff = abs(vc[i].center.y - vc[j].center.y) / nh; //y差比率
            if (ratio < 1.0 || ratio > 5.0 || xDiff < 0.5 || yDiff > 2.0)
                continue;
            //装甲板尺寸
            ARMOR.size.height = nh;
            ARMOR.size.width = nw;
            //存放匹配装甲板
            vRec.push_back(ARMOR);
            //绘图位置
            Point2f point1;
            Point2f point2;
            point1.x=vc[i].center.x;point1.y=vc[i].center.y+20;
            point2.x=vc[j].center.x;point2.y=vc[j].center.y-20;
            int xmidnum = (point1.x+point2.x)/2;
            int ymidnum = (point1.y+point2.y)/2;
            //此时轮廓已筛选完毕，为了方便输出，我们将得到的数据就此输出处理
            ARMOR.center.x = filter(ARMOR.center.x,xmidnum,30);
            ARMOR.center.y = filter(ARMOR.center.y,ymidnum, 30);
            //绘制
            rectangle(image, point1,point2, Scalar( 0, 0,255), 10);//将装甲板框起来
            circle(image,ARMOR.center,5,Scalar(0,255,0),10);//标出装甲板中心点
            cout<<ARMOR.center.x<<endl;
            cout<<ARMOR.center.y<<endl;
        }
    }

    //在原图上标注装甲板中心坐标
    cv::putText( image,"@wzh", Point(1400,1150),FONT_HERSHEY_SIMPLEX,1,Scalar(255,255,255) ,2,LINE_8,false);
    cv::putText( image,"Centor(690,765)", Point(1400,1200),FONT_HERSHEY_SIMPLEX,1,Scalar(0,255,0) ,2,LINE_8,false);
    
    //最终的识别图
    namedWindow("last_result",WINDOW_NORMAL);
    imshow("last_result",image);
    //基操
    //终于搞完了！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！！学长等我！！！！！！！！！！！！！！！！！！！！！！！！！！
    //[dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog][dog]
    //让我看看是谁大晚上不睡觉在那回消息！
    waitKey(0);
    destroyAllWindows();

    return 0;
}

//筛选预处理
//纠正不是竖着的轮廓，更容易查找灯条
RotatedRect& adjustRec(cv::RotatedRect& rec, const int mode) {
    using std::swap;

    float& width = rec.size.width;
    float& height = rec.size.height;
    float& angle = rec.angle;

    if (mode == 0) {
        if (width < height) {
            swap(width, height);
            angle += 90.0;
        }
    }

    while (angle >= 90.0) angle -= 180.0;
    while (angle < -90.0) angle += 180.0;

    if (mode == 1) {
        if (angle >= 45.0) {
            swap(width, height);
            angle -= 90.0;
        }
        else if (angle < -45.0) {
            swap(width, height);
            angle += 90.0;
        }
    }
   return rec;
} 