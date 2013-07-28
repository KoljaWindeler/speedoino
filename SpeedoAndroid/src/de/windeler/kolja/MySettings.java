package de.windeler.kolja;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.Button;
import android.widget.CheckBox;

public class MySettings extends Activity implements OnItemSelectedListener, OnClickListener {
	//Design
	CheckBox verify_upload;
	Button close;

	//vars
	public static final String PREFS_NAME = "SpeedoinoSettings";
	SharedPreferences settings;

	public void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.my_settings);
		settings = getSharedPreferences(PREFS_NAME, 0);

		/// url input 
		verify_upload=(CheckBox)findViewById(R.id.verify_upload);
		verify_upload.setChecked(settings.getBoolean("verify_active",true));


		/// button 
		close=(Button)findViewById(R.id.my_settings_close);
		close.setOnClickListener(this);
		
	}


	@Override
	public void onClick(View arg0) {
		Log.i("SEM","onClick");
		save_me();
		if(arg0.getId()==R.id.verify_upload){
			save_me();
		} else if(arg0.getId()==R.id.my_settings_close){
			save_me();
			finish();
		}
	}


	private void save_me() {	
		SharedPreferences.Editor editor = settings.edit();
		Boolean verify_value = verify_upload.isChecked();
		editor.putBoolean("verify_active",verify_value);
		editor.commit();
		return;
	}


	@Override
	public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,
			long arg3) {
		// TODO Auto-generated method stub
		
	}


	@Override
	public void onNothingSelected(AdapterView<?> arg0) {
		// TODO Auto-generated method stub
		
	}


}