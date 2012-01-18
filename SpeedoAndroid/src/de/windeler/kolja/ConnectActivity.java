package de.windeler.kolja;

import android.app.Activity;
import android.os.Bundle;
import android.widget.Button;
import android.widget.TextView;

public class ConnectActivity extends Activity {
    /** Called when the activity is first created. */
	// Navigation
    private Button LeftBtn;
    private Button RightBtn;
    private Button UpBtn;
    private Button DownBtn;

    // anzeigen
    public TextView mStatus;
    public TextView mLog;
    public final String DEVICE_NAME = "device_name";
    
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.connect);
        mStatus = (TextView) findViewById(R.id.status_value);
        mLog = (TextView) findViewById(R.id.status_value);
        
    }
    
    @Override
	public synchronized void onResume() {
		super.onResume();
		if(SpeedoAndroidActivity.mSerialService.getState()==SpeedoAndroidActivity.mSerialService.STATE_CONNECTING){
        	setStatus("Connecting...");
        } else if(SpeedoAndroidActivity.mSerialService.getState()==SpeedoAndroidActivity.mSerialService.STATE_CONNECTED){
        	setStatus("Connected");
        }
    };
    
    public void setStatus(CharSequence status){
    	mStatus.setText(status);
    }
}
