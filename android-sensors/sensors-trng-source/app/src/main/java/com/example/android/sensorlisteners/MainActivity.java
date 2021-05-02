package com.example.android.sensorlisteners;

import android.content.Context;
import android.content.Intent;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import org.bouncycastle.crypto.macs.HMac;
import org.bouncycastle.crypto.digests.SHA512Digest;
import org.bouncycastle.crypto.prng.BasicEntropySourceProvider;
import org.bouncycastle.crypto.prng.SP800SecureRandom;
import org.bouncycastle.crypto.prng.SP800SecureRandomBuilder;

import android.support.annotation.RequiresApi;
import android.support.v4.content.FileProvider;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.TextView;
import java.io.File;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.math.BigInteger;
import java.security.SecureRandom;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Locale;
import java.util.Queue;
import java.util.LinkedList;

public class MainActivity extends AppCompatActivity implements SensorEventListener {
    private Sensor Accelerometer;
    private TextView AccelerometerText;
    private Sensor Gyroscope;
    private TextView GyroscopeText;
    private Sensor Magnetometer;
    private TextView MagnetometerText;
    private TextView rand;
    private final StringBuilder csv_n = new StringBuilder();
    private SP800SecureRandom random;
    private Queue<BigInteger> entropy_queue;
    private BigInteger count;
    private String timestr;
    private String timestr_p;
    /*private int consecutiveEqualCount;
    private float prev;
    private int samplesCollected;
    private float sample;
    private int proportionCount;*/
    private byte[] bytes;
    private SensorManager sensorManager;
    private FileWriter writer;
    private SimpleDateFormat dateFormat;
    private Boolean hasStopped;

    @Override // android.support.v7.app.AppCompatActivity, android.support.v4.app.SupportActivity, android.support.v4.app.FragmentActivity
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Context context = getApplicationContext();
        File file = new File(context.getExternalFilesDir(null), "random-numbers-file-" + Calendar.getInstance().getTimeInMillis() + ".csv");
        try {
            this.writer = new FileWriter(file);
        } catch (IOException e) {
            e.printStackTrace();
        }
        this.hasStopped = false;
        this.dateFormat = new SimpleDateFormat("h:mm:ss a", Locale.ENGLISH);
        this.csv_n.append("Numbers generated\n");
        this.count = new BigInteger("0");
        this.timestr = "";
        /*this.consecutiveEqualCount = 0;
        this.samplesCollected = 0;
        this.prev = 0;*/
        this.random = new SP800SecureRandomBuilder().buildHMAC(new HMac(new SHA512Digest()), (new BasicEntropySourceProvider(new SecureRandom(), false)).get(32).getEntropy(), false);
        this.rand = findViewById(R.id.rand);
        this.AccelerometerText = findViewById(R.id.label_accelerometer);
        this.GyroscopeText = findViewById(R.id.label_gyroscope);
        this.MagnetometerText = findViewById(R.id.label_magnetometer);
        this.sensorManager = (SensorManager) getSystemService(SENSOR_SERVICE);
        if (sensorManager != null) {
            this.Accelerometer = sensorManager.getDefaultSensor(1);
            this.Gyroscope = sensorManager.getDefaultSensor(4);
            this.Magnetometer = sensorManager.getDefaultSensor(2);
        }
        if (this.Gyroscope == null) {
            this.GyroscopeText.setText(getResources().getString(R.string.error_no_sensor, "gyroscope"));
        }
        if (this.Accelerometer == null) {
            this.AccelerometerText.setText(getResources().getString(R.string.error_no_sensor, "accelerometer"));
        }
        if (this.Magnetometer == null) {
            this.MagnetometerText.setText(getResources().getString(R.string.error_no_sensor, "magnetic field"));
        }
        this.bytes = new byte[4];
        this.entropy_queue = new LinkedList<>();
    }

    @Override // android.support.v7.app.AppCompatActivity, android.support.v4.app.FragmentActivity
    public void onStart() {
        super.onStart();
        this.timestr_p = this.timestr;
        if(this.hasStopped)
            this.rand.setText(getResources().getString(R.string.rand, this.count.toString() + " (resuming since: " + this.timestr_p + ")"));
        Sensor sensor = this.Accelerometer;
        if (sensor != null) {
            this.sensorManager.registerListener(this, sensor, SensorManager.SENSOR_DELAY_FASTEST);
        }
        Sensor sensor2 = this.Gyroscope;
        if (sensor2 != null) {
            this.sensorManager.registerListener(this, sensor2, SensorManager.SENSOR_DELAY_FASTEST);
        }
        Sensor sensor3 = this.Magnetometer;
        if (sensor3 != null) {
            this.sensorManager.registerListener(this, sensor3, SensorManager.SENSOR_DELAY_FASTEST);
        }
    }

    /* access modifiers changed from: protected */
    @Override // android.support.v7.app.AppCompatActivity, android.support.v4.app.FragmentActivity
    public void onStop() {
        super.onStop();
        this.hasStopped = true;
        try {
            this.writer.flush();
        } catch (IOException e) {
            e.printStackTrace();
        }
        //this.mSensorManager.unregisterListener(this);
    }

    @RequiresApi(api = Build.VERSION_CODES.O)
    public void onSensorChanged(SensorEvent sensorEvent) {
        float currentValue_0 = sensorEvent.values[0];
        float currentValue_1 = sensorEvent.values[1];
        float currentValue_2 = sensorEvent.values[2];
        for (float v : new float[]{currentValue_0, currentValue_1, currentValue_2}) {
            /*if(v == this.prev)
                ++this.consecutiveEqualCount;
            else
            {
                if(this.consecutiveEqualCount >= 4)
                    Log.d("health test", "Repetition count test failed: " + v + " seen " + this.consecutiveEqualCount + " times");
                this.consecutiveEqualCount = 0;
            }
            this.prev = v;
            if(this.samplesCollected == 0)
            {
                this.sample = v;
                this.samplesCollected = 1;
            }
            else if(this.samplesCollected == 512)
            {
                if(this.proportionCount >= 21)
                    Log.d("health test", "Adaptive proportion test failed: " + this.sample + " seen " + this.proportionCount + " times");
                this.proportionCount = 1;
                this.samplesCollected = 0;
            }
            else
            {
                if(v == this.sample)
                    ++this.proportionCount;
                ++this.samplesCollected;
            }*/
            String v_str = String.valueOf(v);
            if(v_str.contains("E"))
                v_str = v_str.substring(v_str.indexOf('.')+1,v_str.indexOf('E'));
            else
                v_str = v_str.substring(v_str.indexOf('.')+1);
            int l = Integer.parseInt(v_str);
            this.entropy_queue.add(BigInteger.valueOf(l & 255));
            if(entropy_queue.size() == 4) {
                this.random.nextBytes(bytes);
                //BigInteger prng_out = new BigInteger("0");
                BigInteger trng_out = new BigInteger("0");
                for (int i = 0; i < 4; ++i) {
                    BigInteger b = new BigInteger(Integer.toUnsignedString(Byte.toUnsignedInt(bytes[i])));
                    //prng_out = prng_out.add(b.shiftLeft(8*i));
                    trng_out = trng_out.add(b.xor(entropy_queue.remove()).shiftLeft(8*i));
                }
                this.csv_n.append(trng_out).append("\n");
                try {
                    writer.append(trng_out.toString()).append("\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
                this.count = this.count.add(BigInteger.ONE);
                this.timestr = dateFormat.format(Calendar.getInstance().getTime());
                if(this.hasStopped)
                    this.rand.setText(getResources().getString(R.string.rand, this.count.toString() + " (resuming since: " + this.timestr_p + ")"));
                else
                    this.rand.setText(getResources().getString(R.string.rand, this.count.toString()));
            }
        }
        int sensorType = sensorEvent.sensor.getType();
        if (sensorType == 1) {
            this.AccelerometerText.setText(getResources().getString(R.string.label_accelerometer, currentValue_0, currentValue_1, currentValue_2));
        } else if (sensorType == 2) {
            this.MagnetometerText.setText(getResources().getString(R.string.label_magnetometer, currentValue_0, currentValue_1, currentValue_2));
        } else if (sensorType == 4) {
            this.GyroscopeText.setText(getResources().getString(R.string.label_gyroscope, currentValue_0, currentValue_1, currentValue_2));
        }
    }

    public void onAccuracyChanged(Sensor sensor, int i) {
    }

    public void logNumbers(View view) {
        try {
            FileOutputStream out = openFileOutput("Random-Numbers.csv", 0);
            out.write(this.csv_n.toString().getBytes());
            out.close();
            Uri path = FileProvider.getUriForFile(getApplicationContext(),
                    BuildConfig.APPLICATION_ID,
                    new File(getFilesDir(), "Random-Numbers.csv"));
            Intent file_intent = new Intent("android.intent.action.SEND");
            file_intent.setType("text/csv");
            file_intent.putExtra("android.intent.extra.SUBJECT", "Sensor-Data");
            file_intent.addFlags(Intent.FLAG_GRANT_READ_URI_PERMISSION);
            file_intent.putExtra("android.intent.extra.STREAM", path);
            startActivity(Intent.createChooser(file_intent, "Save this file..."));
        } catch (Exception E) {
            E.printStackTrace();
        }
    }
}