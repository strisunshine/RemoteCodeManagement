package com.example.testaudiocapturewiththreshold;

import android.content.Intent;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.os.AsyncTask;
import android.os.Environment;
import android.os.Handler;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Calendar;
import java.util.concurrent.TimeUnit;


public class TestAudioCaptureWithThreshold extends ActionBarActivity {
    private static final String TAG = TestAudioCaptureWithThreshold.class.getSimpleName();
    private static final int RECORDER_BPP = 16;
    private static final String AUDIO_RECORDER_FILE_EXT_WAV = ".wav";
    private static final String AUDIO_RECORDER_FOLDER = "AudioRecorder";
    private static final String AUDIO_RECORDER_TEMP_FILE = "record_temp.raw";
    private Button mStartStopButton;
    private TextView mRecordingTextView;

    FileOutputStream os = null;

    int bufferSize ;
    int frequency = 44100; //8000;
    int channelConfiguration = AudioFormat.CHANNEL_IN_MONO;
    int audioEncoding = AudioFormat.ENCODING_PCM_16BIT;
    boolean started = false;
    RecordAudio recordTask;

    short threshold=500;

    boolean debug=false;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.w(TAG, "onCreate");
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_test_audio_capture_with_threshold);

        mRecordingTextView = (TextView)findViewById(R.id.recording_text_view);

        mStartStopButton = (Button)findViewById(R.id.start_stop_button);

        mStartStopButton.setOnClickListener(new View.OnClickListener(){
            @Override
            public void onClick(View v){
                String x = (String) mStartStopButton.getText();

                if(mStartStopButton.getText()=="Stop Recording"){
                    mRecordingTextView.setText("Recording stopped. ");
                    //mRecordingTextView.append("Saved in file " + v);
                    mStartStopButton.setText("Start Recording");
                    stopAquisition();
                }
                else{
                    mRecordingTextView.setText("Recording...");
                    mStartStopButton.setText("Stop Recording");
                    startAquisition();
                }
            }
        });
    }


    @Override
    protected void onResume() {
        Log.w(TAG, "onResume");
        super.onResume();
    }

    @Override
    protected void onDestroy() {
        Log.w(TAG, "onDestroy");
        stopAquisition();
        super.onDestroy();

    }

    public class RecordAudio extends AsyncTask<Void, Double, Void> {

        @Override
        protected Void doInBackground(Void... arg0) {
            Log.w(TAG, "doInBackground");
            try {

                String filename = getTempFilename();
                os = null;

                bufferSize = AudioRecord.getMinBufferSize(frequency,
                        channelConfiguration, audioEncoding);

                AudioRecord audioRecord = new AudioRecord( MediaRecorder.AudioSource.MIC, frequency,
                        channelConfiguration, audioEncoding, bufferSize);

                short[] buffer = new short[bufferSize];

                audioRecord.startRecording();

                boolean isCountingTime = false;
                long starttime = 0, stoptime = 0, elapsedtime = 0;
                while (started) {

                    int bufferReadResult = audioRecord.read(buffer, 0,bufferSize);
                    if(AudioRecord.ERROR_INVALID_OPERATION != bufferReadResult){
                        //check signal
                        //put a threshold
                        int foundPeak=searchThreshold(buffer,threshold);
                        if (foundPeak>-1){
                            isCountingTime = false;

                            //if the output file stream doesn't exist, create it, else ignore this step
                            if(os==null){
                                try {
                                    os = new FileOutputStream(filename);
                                } catch (FileNotFoundException e) {
                                    e.printStackTrace();
                                }
                            }
                            //found signal
                            //record signal
                            Calendar c = Calendar.getInstance();
                            Log.w(TAG, "recorded at "+ c.get(Calendar.HOUR_OF_DAY) + ":" + c.get(Calendar.MINUTE) + ":" + c.get(Calendar.SECOND));
                            if(os!=null) {
                                byte[] byteBuffer = ShortToByte(buffer, bufferReadResult);
                                try {
                                    os.write(byteBuffer);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                            }
                        }else{
                            //count the time
                            //don't save signal
                            if(isCountingTime == false){
                                starttime = System.nanoTime();
                            }
                            isCountingTime = true;
                            stoptime = System.nanoTime();
                            elapsedtime = stoptime - starttime;
                            Log.w(TAG, "no voice detected for "+ TimeUnit.SECONDS.convert(elapsedtime, TimeUnit.NANOSECONDS) + " seconds");

                            //if the file output stream has been created and if the silent time is no more than 5 seconds, still record the blank
                            if(os!=null && elapsedtime<= 5000000000L){
                                byte[] byteBuffer =ShortToByte(buffer,bufferReadResult);
                                try {
                                    os.write(byteBuffer);
                                } catch (IOException e) {
                                    e.printStackTrace();
                                }
                                continue;
                            }
                            //if elapsed time is longer than 20 seconds
                            if(elapsedtime > 20000000000L) break;
                        }
                        //show results
                        //here, with publichProgress function, if you calculate the total saved samples,
                        //you can optionally show the recorded file length in seconds:      publishProgress(elsapsedTime,0);
                    }
                }

                audioRecord.stop();

                if(os!=null) {
                    copyWaveFile(getTempFilename(), getFilename());
                    deleteTempFile();
                    //close file
                    try {
                        os.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                //mRecordingTextView = (TextView)findViewById(R.id.recording_text_view);
                //mRecordingTextView.append("Saved in file " + getFilename());


            } catch (Throwable t) {
                t.printStackTrace();
                Log.e("AudioRecord", "Recording Failed");
            }
            return null;

        } //fine di doInBackground

        byte [] ShortToByte(short [] input, int elements) {
            int short_index, byte_index;
            int iterations = elements; //input.length;
            byte [] buffer = new byte[iterations * 2];

            short_index = byte_index = 0;

            for(/*NOP*/; short_index != iterations; /*NOP*/)
            {
                buffer[byte_index]     = (byte) (input[short_index] & 0x00FF);
                buffer[byte_index + 1] = (byte) ((input[short_index] & 0xFF00) >> 8);

                ++short_index; byte_index += 2;
            }

            return buffer;
        }


        int searchThreshold(short[]arr,short thr){
            int peakIndex;
            int arrLen=arr.length;
            for (peakIndex=0;peakIndex<arrLen;peakIndex++){
                if ((arr[peakIndex]>=thr) || (arr[peakIndex]<=-thr)){
                    //se supera la soglia, esci e ritorna peakindex-mezzo kernel.

                    return peakIndex;
                }
            }
            return -1; //not found
        }

    /*
    @Override
    protected void onProgressUpdate(Double... values) {
        DecimalFormat sf = new DecimalFormat("000.0000");
        elapsedTimeTxt.setText(sf.format(values[0]));

    }
    */

        private String getFilename(){
            String filepath = Environment.getExternalStorageDirectory().getPath();
            File file = new File(filepath,AUDIO_RECORDER_FOLDER);

            if(!file.exists()){
                file.mkdirs();
            }

            return (file.getAbsolutePath() + "/" + System.currentTimeMillis() + AUDIO_RECORDER_FILE_EXT_WAV);
        }


        private String getTempFilename(){
            String filepath = Environment.getExternalStorageDirectory().getPath();
            File file = new File(filepath,AUDIO_RECORDER_FOLDER);

            if(!file.exists()){
                file.mkdirs();
            }

            File tempFile = new File(filepath,AUDIO_RECORDER_TEMP_FILE);

            if(tempFile.exists())
                tempFile.delete();

            return (file.getAbsolutePath() + "/" + AUDIO_RECORDER_TEMP_FILE);
        }





        private void deleteTempFile() {
            File file = new File(getTempFilename());

            file.delete();
        }

        private void copyWaveFile(String inFilename,String outFilename){
            FileInputStream in = null;
            FileOutputStream out = null;
            long totalAudioLen = 0;
            long totalDataLen = totalAudioLen + 36;
            long longSampleRate = frequency;
            int channels = 1;
            long byteRate = RECORDER_BPP * frequency * channels/8;

            byte[] data = new byte[bufferSize];

            try {
                in = new FileInputStream(inFilename);
                out = new FileOutputStream(outFilename);
                totalAudioLen = in.getChannel().size();
                totalDataLen = totalAudioLen + 36;


                WriteWaveFileHeader(out, totalAudioLen, totalDataLen,
                        longSampleRate, channels, byteRate);

                while(in.read(data) != -1){
                    out.write(data);
                }

                in.close();
                out.close();
                Log.w(TAG, "has written to file "+ outFilename );
            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }

        private void WriteWaveFileHeader(
                FileOutputStream out, long totalAudioLen,
                long totalDataLen, long longSampleRate, int channels,
                long byteRate) throws IOException {

            byte[] header = new byte[44];

            header[0] = 'R';  // RIFF/WAVE header
            header[1] = 'I';
            header[2] = 'F';
            header[3] = 'F';
            header[4] = (byte) (totalDataLen & 0xff);
            header[5] = (byte) ((totalDataLen >> 8) & 0xff);
            header[6] = (byte) ((totalDataLen >> 16) & 0xff);
            header[7] = (byte) ((totalDataLen >> 24) & 0xff);
            header[8] = 'W';
            header[9] = 'A';
            header[10] = 'V';
            header[11] = 'E';
            header[12] = 'f';  // 'fmt ' chunk
            header[13] = 'm';
            header[14] = 't';
            header[15] = ' ';
            header[16] = 16;  // 4 bytes: size of 'fmt ' chunk
            header[17] = 0;
            header[18] = 0;
            header[19] = 0;
            header[20] = 1;  // format = 1
            header[21] = 0;
            header[22] = (byte) channels;
            header[23] = 0;
            header[24] = (byte) (longSampleRate & 0xff);
            header[25] = (byte) ((longSampleRate >> 8) & 0xff);
            header[26] = (byte) ((longSampleRate >> 16) & 0xff);
            header[27] = (byte) ((longSampleRate >> 24) & 0xff);
            header[28] = (byte) (byteRate & 0xff);
            header[29] = (byte) ((byteRate >> 8) & 0xff);
            header[30] = (byte) ((byteRate >> 16) & 0xff);
            header[31] = (byte) ((byteRate >> 24) & 0xff);
            header[32] = (byte) (channels * 16 / 8);  // block align
            header[33] = 0;
            header[34] = RECORDER_BPP;  // bits per sample
            header[35] = 0;
            header[36] = 'd';
            header[37] = 'a';
            header[38] = 't';
            header[39] = 'a';
            header[40] = (byte) (totalAudioLen & 0xff);
            header[41] = (byte) ((totalAudioLen >> 8) & 0xff);
            header[42] = (byte) ((totalAudioLen >> 16) & 0xff);
            header[43] = (byte) ((totalAudioLen >> 24) & 0xff);

            out.write(header, 0, 44);
        }

    } //Fine Classe RecordAudio (AsyncTask)

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        getMenuInflater().inflate(R.menu.menu_test_audio_capture_with_threshold,
                menu);
        return true;

    }


    public void resetAquisition() {
        Log.w(TAG, "resetAquisition");
        stopAquisition();
        //startButton.setText("WAIT");
        startAquisition();
    }

    public void stopAquisition() {
        Log.w(TAG, "stopAquisition");
        if (started) {
            started = false;
            recordTask.cancel(true);
        }
    }

    public void startAquisition(){
        Log.w(TAG, "startAquisition");
        Handler handler = new Handler();
        handler.postDelayed(new Runnable() {
            public void run() {

                //elapsedTime=0;
                started = true;
                recordTask = new RecordAudio();
                recordTask.execute();
                //startButton.setText("RESET");
            }
        }, 500);
    }

    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
    }

}
