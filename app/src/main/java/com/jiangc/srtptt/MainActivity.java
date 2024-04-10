package com.jiangc.srtptt;

import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.os.Environment;
import android.util.Log;
import android.view.View;

import com.jiangc.libavcodec.AvCodecManager;
import com.jiangc.srtptt.databinding.ActivityMainBinding;

public class MainActivity extends AppCompatActivity {
    private static final String TAG = "MainActivity";
    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());


        binding.btStartrecorder.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AvCodecManager.getInstance().debugFile(Environment.getExternalStorageDirectory().getAbsolutePath() + "/temp.pcm");
                int ret = AvCodecManager.getInstance().createAudioEngine();
                if (ret != 0) {
                    Log.e(TAG, "onClick: createAudioEngine fail");
                    return;
                }

                ret = AvCodecManager.getInstance().createAudioRecorder();
                if (ret != 0) {
                    Log.e(TAG, "onClick: createAudioRecorder fail");
                    AvCodecManager.getInstance().destroyAudioRecorder();
                    return;
                }

                ret = AvCodecManager.getInstance().startAudioRecorder();
                if (ret != 0) {
                    Log.e(TAG, "onClick: startAudioRecorder fail");
                    AvCodecManager.getInstance().destroyAudioRecorder();
                }

            }
        });


        binding.btStoprecorder.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                AvCodecManager.getInstance().destroyAudioRecorder();
            }
        });
    }


}