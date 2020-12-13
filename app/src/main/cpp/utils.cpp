//
// Created by 王博 on 2019-08-01.
//

#define  LOG_TAG "Utils"

#include <android/bitmap.h>
#include <opencv2/imgproc.hpp>
#include "utlis.h"
#include "common.h"

void bitmap2Mat(JNIEnv *env, jobject bitmap, Mat *mat, bool needPremultiplyAlpha) {

    AndroidBitmapInfo info;
    void *pixels = 0;
    Mat &dst = *mat;

    //获得bitmap信息
    CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
    //必须是rgba8888 rab565
    CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
              info.format == ANDROID_BITMAP_FORMAT_RGB_565);
    //lock 获取数据
    CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
    CV_Assert(pixels);
    dst.create(info.height, info.width, CV_8UC4);
    if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        //LOGI()

        Mat tmp(info.height, info.width, CV_8UC4, pixels);

        if (needPremultiplyAlpha) cvtColor(tmp, dst, COLOR_mRGBA2RGBA);
        else tmp.copyTo(dst);

    } else {
        Mat tmp(info.height, info.width, CV_8UC2, pixels);
        cvtColor(tmp, dst, COLOR_BGR5652BGRA);
    }
    AndroidBitmap_unlockPixels(env, bitmap);

}

void mat2Bitmap(JNIEnv *env, Mat mat, jobject bitmap, bool needPremultiplyAlpha) {

    AndroidBitmapInfo info;
    void *pixels = 0;

    try {
        //获得bitmap信息
        CV_Assert(AndroidBitmap_getInfo(env, bitmap, &info) >= 0);
        //必须是rgba8888 rab565
        CV_Assert(info.format == ANDROID_BITMAP_FORMAT_RGBA_8888 ||
                  info.format == ANDROID_BITMAP_FORMAT_RGB_565);
        //lock 获取数据
        CV_Assert(mat.dims == 2 && info.height == (uint32_t) mat.rows &&
                  info.width == (uint32_t) mat.cols);
        CV_Assert(AndroidBitmap_lockPixels(env, bitmap, &pixels) >= 0);
        CV_Assert(pixels);


        if (info.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            //LOGI()

            Mat tmp(info.height, info.width, CV_8UC4, pixels);
            if (mat.type() == CV_8UC1) {
                cvtColor(mat, tmp, COLOR_GRAY2RGBA);
            } else if (mat.type() == CV_8UC3) {
                cvtColor(mat, tmp, COLOR_RGB2BGRA);
            } else if (mat.type() == CV_8UC3) {
                cvtColor(mat, tmp, COLOR_RGB2BGR565);
            } else {
                if (needPremultiplyAlpha) cvtColor(mat, tmp, COLOR_mRGBA2RGBA);
                else mat.copyTo(tmp);
            }


        } else {
            Mat tmp(info.height, info.width, CV_8UC2, pixels);
            if (mat.type() == CV_8UC1) {
                cvtColor(mat, tmp, COLOR_GRAY2RGBA);
            } else if (mat.type() == CV_8UC3) {
                cvtColor(mat, tmp, COLOR_RGB2BGRA);
            } else if (mat.type() == CV_8UC4) {
                cvtColor(mat, tmp, COLOR_RGB2BGR565);
            }
        }
    }catch (cv::Exception e){
        AndroidBitmap_unlockPixels(env, bitmap);
        jclass je = env->FindClass("org/opencv/core/CvException");
        if(!je) je = env->FindClass("java/lang/Exception");
        env->ThrowNew(je, e.what());

    }


}

jobject createBitmap(JNIEnv *env, Mat srcData, jobject config) {
    jclass java_bitmap_class = (jclass) env->FindClass("android/graphics/Bitmap");
    jmethodID mid = env->GetStaticMethodID(java_bitmap_class,
                                           "createBitmap", "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");

    jobject bitmap = env->CallStaticObjectMethod(java_bitmap_class, mid, srcData.cols, srcData.rows,
                                                 config);
    mat2Bitmap(env, srcData, bitmap, 0);
    return bitmap;

}