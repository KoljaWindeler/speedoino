package com.jkw.smartspeedo;

import com.google.android.gms.maps.GoogleMap;
import com.google.android.gms.maps.MapFragment;
import com.google.android.gms.maps.model.LatLng;

import android.app.FragmentManager;
import android.content.Context;
import android.support.v4.app.Fragment;
import android.util.AttributeSet;
import android.widget.Button;

public class Layout_map implements Layout {

	Context mContext; 

	// surface
	GaugeCustomView rpm_speed_gear;
	GaugeCustomView temp;
	GaugeCustomView airTemp;
	LinearCustomView engine_temp;

	// surface 2
	ControllCustomView ctrl;
	GoogleMap map;

	public Layout_map() {	}


	public int get_layout(){
		return R.layout.layout_map;
	}

	public void unregister_elements(SmartSpeedoMain smartSpeedoMain){
		android.app.Fragment xmlFragment = smartSpeedoMain.getFragmentManager().findFragmentById(R.id.map);
	    if (xmlFragment != null) {
	    	smartSpeedoMain.getFragmentManager().beginTransaction().remove(xmlFragment).commit();
	    }
	}
	
	public void find_elements(SmartSpeedoMain smartSpeedoMain){
		//views
		rpm_speed_gear=(GaugeCustomView)smartSpeedoMain.findViewById(R.id.rpm_speed_gear);
		temp=(GaugeCustomView)smartSpeedoMain.findViewById(R.id.temp);
		ctrl=((ControllCustomView)smartSpeedoMain.findViewById(R.id.ctrl));
		engine_temp=((LinearCustomView)smartSpeedoMain.findViewById(R.id.engineTemp));
		airTemp=((GaugeCustomView)smartSpeedoMain.findViewById(R.id.airTemp));

		map = ((MapFragment)smartSpeedoMain.getFragmentManager().findFragmentById(R.id.map)).getMap();
		if(map!=null){
			map.setMapType(GoogleMap.MAP_TYPE_NORMAL);
			map.setMyLocationEnabled(true);
			map.setTrafficEnabled(true);
		}


		airTemp.setLimits(0, 50);
		airTemp.setLayout(180, 270, 10, 1);
		airTemp.setValue(0);
		airTemp.setType(GaugeCustomView.TYPE_TEMP);

		engine_temp.setLimits(40, 120);
		engine_temp.setLayout(0, 0, 15, 5, 70,90);
		engine_temp.setValue(40);

		rpm_speed_gear.setLimits(0, 5500);
		rpm_speed_gear.setLayout(180, 270, 1000, 50);
		//		rpm.setLimits(0, 18000);
		//		rpm.setLayout(180, 270, 1000, 200);
		rpm_speed_gear.setValue(3255); // rpm
		rpm_speed_gear.setType(GaugeCustomView.TYPE_KMH_RPM);
		rpm_speed_gear.setSecondValue(84,2); // km/h
		rpm_speed_gear.setSecondValue(4,3); // gang


		temp.setLimits(40, 120);
		temp.setLayout(240, 240, 10, 2,60, 100);
		temp.setValue(40);
		temp.setSecondValue(40,2);
		temp.setType(GaugeCustomView.TYPE_TEMP);

		// buttons
		((Button)smartSpeedoMain.findViewById(R.id.button1)).setOnClickListener(smartSpeedoMain);
		((Button)smartSpeedoMain.findViewById(R.id.connect)).setOnClickListener(smartSpeedoMain);
	}


	public void setAirTemp(float f) {
		airTemp.setValue((int)f);
	}


	public void setSpeed(int i) {
		rpm_speed_gear.setSecondValue(i,2);		
	}


	public void setGear(int i) {
		rpm_speed_gear.setSecondValue(i,3);		
	}


	public void setRPM(int i) {
		rpm_speed_gear.setValue(i);		
	}


	public void setCTRL_left(boolean b) {
		ctrl.set_left(b);		
	}


	public void setOILtemp(int i) {
		engine_temp.setValue(i);		
	}


	public void setCTRL_GPS(boolean b) {
		ctrl.set_GPS(b);		
	}


	public void setCTRL_right(boolean b) {
		ctrl.set_right(b);		
	}

	public void setMap(double lati,double longitude){
		if(map!=null){
			LatLng pos = new LatLng(lati,longitude);
			float zoom;
			boolean map_Zoom_changed=false;
			if(map_Zoom_changed){
				zoom=map.getCameraPosition().zoom;
			} else {
				zoom=15;
			}
			//				CameraPosition CamPos = new CameraPosition.Builder().target(pos).zoom(zoom).tilt(85).build();
			//				map.animateCamera(CameraUpdateFactory.newCameraPosition(CamPos));
		}
	}
}
