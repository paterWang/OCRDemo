//
// Created by 王博 on 2019-08-01.
//

#ifndef OCRDEMO_UTLIS_H
#define OCRDEMO_UTLIS_H

#include <android/bitmap.h>
#include <opencv2/opencv.hpp>


using namespace cv;
extern "C"{
    void  bitmap2Mat(JNIEnv *env,jobject bitmap,Mat* mat,bool needPremultiplyAlpha=0);
    void  mat2Bitmap(JNIEnv *env,Mat mat,jobject bitmap,bool needPremultiplyAlpha=0);

jobject createBitmap(JNIEnv *env,Mat secData,jobject config);
jobject mat_to_bitmap(JNIEnv * env, Mat & src, bool needPremultiplyAlpha, jobject bitmap_config);
}


#endif //OCRDEMO_UTLIS_H

