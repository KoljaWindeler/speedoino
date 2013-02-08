package de.windeler.kolja;


import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemSelectedListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.Spinner;

public class ImageEditorSettings extends Activity implements OnItemSelectedListener, OnClickListener {
	// design 
	Spinner scale_spinner;
	Spinner color_invert_spinner;
	Spinner backgroud_color_spinner;
	Button close;

	//vars
	List<String> scale_list = new ArrayList<String>();
	List<String> color_invert_list = new ArrayList<String>();
	List<String> background_color_list = new ArrayList<String>();
	ArrayAdapter<String> dataAdapter;
	SharedPreferences settings;
	public static final String PREFS_NAME = "SpeedoAndroidImageEditorSettings";
	public static final String PREFS_SCALE = "scale";
	public static final String PREFS_INVERT = "invert";
	public static final String PREFS_BACK_COLOR = "back_color";
	boolean save_now;


	public void onCreate(Bundle savedInstanceState) {
		save_now=false;
		super.onCreate(savedInstanceState);
		setContentView(R.layout.image_editor_settings);
		settings = getSharedPreferences(PREFS_NAME, 0);


		/// spinner 
		scale_spinner=(Spinner) findViewById(R.id.image_editor_settings_scale_spinner);
		scale_spinner.setAdapter(ArrayAdapter.createFromResource(this, R.array.scale_values, android.R.layout.simple_spinner_item));
		scale_spinner.setOnItemSelectedListener(this);

		color_invert_spinner=(Spinner) findViewById(R.id.image_editor_settings_color_invert_spinner);
		color_invert_spinner.setAdapter(ArrayAdapter.createFromResource(this, R.array.color_invert_values, android.R.layout.simple_spinner_item));
		color_invert_spinner.setOnItemSelectedListener(this);
		
		backgroud_color_spinner=(Spinner) findViewById(R.id.image_editor_settings_background_color);
		backgroud_color_spinner.setAdapter(ArrayAdapter.createFromResource(this, R.array.background_color_values, android.R.layout.simple_spinner_item));
		backgroud_color_spinner.setOnItemSelectedListener(this);

		/// button 
		close=(Button)findViewById(R.id.image_editor_settings_close);
		close.setOnClickListener(this);

		// set the right selection, based on the sharedprefs
		scale_spinner.setSelection(settings.getInt(PREFS_SCALE, 0));
		color_invert_spinner.setSelection(settings.getInt(PREFS_INVERT, 0));
		backgroud_color_spinner.setSelection(settings.getInt(PREFS_BACK_COLOR, 0));
		save_now=true;
	}


	@Override
	public void onItemSelected(AdapterView<?> arg0, View arg1, int arg2,long arg3) {
		Log.i("SEM","ItemSelect()");
		save_me();
	}

	@Override
	public void onNothingSelected(AdapterView<?> arg0) {}

	@Override
	public void onClick(View arg0) {
		Log.i("SEM","onClick");
		finish();
	}


	private void save_me() {
		if(save_now){
			SharedPreferences.Editor editor = settings.edit();
			editor.putInt(PREFS_SCALE, scale_spinner.getSelectedItemPosition());
			editor.putInt(PREFS_INVERT, color_invert_spinner.getSelectedItemPosition());
			editor.putInt(PREFS_BACK_COLOR, backgroud_color_spinner.getSelectedItemPosition());
			editor.commit();
		};
		return;
	}
}
