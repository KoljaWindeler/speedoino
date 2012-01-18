package de.windeler.kolja;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class UploadActivity extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.upload);
        TextView textview = (TextView) findViewById(R.id.Upload_textView);
        textview.setText("This is the Upload tab");

    }
}

