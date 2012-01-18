package de.windeler.kolja;

import android.app.Activity;
import android.os.Bundle;
import android.widget.TextView;

public class DownloadActivity extends Activity {
    /** Called when the activity is first created. */
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.download);
        TextView textview = (TextView) findViewById(R.id.Download_textView);
        textview.setText("This is the Download tab");
    }
}
