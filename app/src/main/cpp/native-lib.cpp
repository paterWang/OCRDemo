#include <jni.h>
#include <string>
#include <opencv2/opencv.hpp>
#include "utlis.h"
#include "common.h"

#define  LOG_TAG "LOG_TAG"
using namespace std;
using namespace cv;
#define  DEFAULT_CARD_WIGHT 640
#define  DEFAULT_CARD_HIGHT 400
#define  FIX_IDCARD_SIZE Size(DEFAULT_CARD_WIGHT,DEFAULT_CARD_HIGHT)
extern "C" JNIEXPORT jstring JNICALL
Java_com_example_ocrdemo_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_ocrdemo_MainActivity_findNember(JNIEnv *env, jobject instance, jobject bitmap,
                                                 jobject argb8888) {

    // TODO

    //1将bitmap转成mat
    //原图
    Mat src_img;
    //转换后的图
    Mat dst_img;
    bitmap2Mat(env, bitmap, &src_img);
    //归一化
    Mat dst;
    resize(src_img, dst, FIX_IDCARD_SIZE);
    //灰度化
    cvtColor(src_img, dst, COLOR_RGB2GRAY);
    //二值化
    threshold(dst, dst, 100, 255, THRESH_BINARY);
    //膨胀处理
    Mat erodeElement = getStructuringElement(MORPH_RECT, Size(40, 10));
    erode(dst, dst, erodeElement);
    //轮廓检测
    vector<vector<Point>> contours;
    vector<Rect> rects;
    findContours(dst, contours, RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));
    //获取身份证号码区域

    for (int i = 0; i < contours.size(); ++i) {
        //获取矩形区域
        Rect rect = boundingRect(contours.at(i));

        if (rect.width > rect.height * 8 && rect.width < rect.height * 16) {
            //需要的区域

            rects.push_back(rect);
        } else {
            if (i == contours.size() - 1) {
                rects.push_back(rect);
            }
        }
    }

    int lowPoint = 0;
    Rect finalRect;
    for (int i = 0; i < rects.size(); ++i) {
        Rect rect = rects.at(i);
        Point point = rect.tl();
        if (point.y > lowPoint) {
            lowPoint = point.y;
            finalRect = rect;
        }

    }
    //图形分割
    dst_img = src_img(finalRect);
    //返回身份证号码图片
    return createBitmap(env, dst_img, argb8888);


}