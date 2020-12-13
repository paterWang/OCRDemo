package com.example.ocrdemo;



import android.Manifest;
import android.annotation.SuppressLint;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Build;
import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import android.widget.Toast;


import com.googlecode.tesseract.android.TessBaseAPI;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;


public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static {
        System.loadLibrary("OpenCV");

    }

    private static final String LANGUAGE = "eng";
    private TessBaseAPI tessBaseAPI;
    private TextView textView;
    private ImageView imageView;
    private AsyncTask asyncTask;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        initPermision();
        imageView = findViewById(R.id.iv_card);
        textView = findViewById(R.id.tv_size);
        tessBaseAPI = new TessBaseAPI();
        initTess();

    }

    @SuppressLint("StaticFileLeak")
    private void initTess() {
        asyncTask = new AsyncTask<Void, Void, Boolean>() {

            @Override
            protected Boolean doInBackground(Void... params) {
                InputStream inputStream = null;
                FileOutputStream fileOutputStream = null;
                File file;
                try {
                    inputStream = getAssets().open(LANGUAGE + ".traineddata");
                    boolean sdCardExist = Environment.getExternalStorageState().equals(
                            android.os.Environment.MEDIA_MOUNTED); // 判断sd卡是否存在
                    if (sdCardExist) {
                        Environment.getExternalStorageDirectory();// 获取外存目录
                        file = new File(Environment.getExternalStorageDirectory().getPath()+"/tess/tessdata/" + LANGUAGE + ".traineddata");
                    } else {
                        file = new File(Environment.getDataDirectory() + "/tessdata/" + LANGUAGE + ".traineddata");
                    }

                    if (!file.exists()) {
                        file.getParentFile().mkdirs();
                        fileOutputStream = new FileOutputStream(file);
                        byte[] buffer = new byte[2048];
                        int len;
                        while ((len = inputStream.read(buffer)) != -1) {
                            fileOutputStream.write(buffer, 0, len);
                        }
                        fileOutputStream.close();

                    }
                    Log.e("log_tag","成功");
                    //Toast.makeText(MainActivity.this, "成功", Toast.LENGTH_SHORT).show();
                    if (sdCardExist) {
                        return tessBaseAPI.init(Environment.getExternalStorageDirectory().getPath()+"/tess", LANGUAGE);
                    } else {

                        return tessBaseAPI.init(Environment.getDataDirectory() + "/tess", LANGUAGE);
                    }

                } catch (Exception e) {
                    Log.e("log_tag","失败");
                   // Toast.makeText(MainActivity.this, "失败", Toast.LENGTH_SHORT).show();
                }

                return false;
            }
        }.execute();


    }

    public void pro(View view) {


    }

    public void next(View view) {

    }

    public void look(View view) {
        //文字识别

        Bitmap bitmap = BitmapFactory.decodeResource(getResources(), R.drawable.timg);
        Bitmap numberBitMap = findNember(bitmap, Bitmap.Config.ARGB_8888);
        bitmap.recycle();
        if (numberBitMap != null) {
            //Toast.makeText(this, "成功", Toast.LENGTH_SHORT).show();
            imageView.setImageBitmap(numberBitMap);
        } else {
            Toast.makeText(this, "失败", Toast.LENGTH_SHORT).show();
            return;
        }


        tessBaseAPI.setVariable(TessBaseAPI.VAR_CHAR_WHITELIST, "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"); // 识别白名单
        tessBaseAPI.setVariable(TessBaseAPI.VAR_CHAR_BLACKLIST, "!@#$%^&*()_+=-[]}{;:'\"\\|~`,./<>?"); // 识别黑名单
        tessBaseAPI.setPageSegMode(TessBaseAPI.PageSegMode.PSM_AUTO_OSD);//设置识别模式

        numberBitMap = numberBitMap.copy(Bitmap.Config.ARGB_8888, true);

        tessBaseAPI.setImage(numberBitMap);

        String text = tessBaseAPI.getUTF8Text();
        // Log.e(TAG,text);

        tessBaseAPI.clear();
        tessBaseAPI.end();
        textView.setText(text);

    }

    public static String getSDPath() {
        File sdDir = null;
        boolean sdCardExist = Environment.getExternalStorageState().equals(
                android.os.Environment.MEDIA_MOUNTED); // 判断sd卡是否存在
        if (sdCardExist) {
            sdDir = Environment.getExternalStorageDirectory();// 获取外存目录
        }
        return sdDir.toString();
    }

    private native Bitmap findNember(Bitmap bitmap, Bitmap.Config argb8888);


    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native String stringFromJNI();

    private void initPermision() {
        if (Build.VERSION.SDK_INT >= 23) {
//            if (checkSelfPermission(Manifest.permission.READ_EXTERNAL_STORAGE) != PackageManager.PERMISSION_GRANTED) {
//                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE,
//                        Manifest.permission.WRITE_EXTERNAL_STORAGE,Manifest.permission.CAMERA}, 1);
//            }


            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE,
                    Manifest.permission.CAMERA,
                    Manifest.permission.CALL_PHONE,
                    Manifest.permission.GET_ACCOUNTS,
                    Manifest.permission.CONTROL_LOCATION_UPDATES,
                    Manifest.permission.READ_EXTERNAL_STORAGE,
                    Manifest.permission.RECORD_AUDIO,
                    Manifest.permission.REQUEST_INSTALL_PACKAGES,
                    Manifest.permission.READ_PHONE_STATE,
                    Manifest.permission.SYSTEM_ALERT_WINDOW,
                    Manifest.permission.SET_DEBUG_APP,
                    Manifest.permission.READ_LOGS,
                    Manifest.permission.ACCESS_COARSE_LOCATION

            }, 10002);
        }


    }
}
