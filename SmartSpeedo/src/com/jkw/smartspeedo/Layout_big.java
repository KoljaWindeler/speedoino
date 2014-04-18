package com.jkw.smartspeedo;

import android.content.Context;
import android.util.AttributeSet;
import android.widget.Button;

public class Layout_big implements Layout {

	Context mContext; 

	// surface
	GaugeCustomView rpm;
	GaugeCustomView speed;
	GaugeCustomView temp;
	GaugeCustomView airTemp;
	LinearCustomView engine_temp;

	// surface 2
	ControllCustomView ctrl;

	public Layout_big() {	}


	public int get_layout(){ 
		return R.layout.layout_big;
	}
	public void unregister_elements(SmartSpeedoMain smartSpeedoMain){};

	public void find_elements(SmartSpeedoMain smartSpeedoMain){
		//views
		rpm=(GaugeCustomView)smartSpeedoMain.findViewById(R.id.rpm);
		speed=(GaugeCustomView)smartSpeedoMain.findViewById(R.id.speed);
		temp=(GaugeCustomView)smartSpeedoMain.findViewById(R.id.coolantTemp);
		ctrl=((ControllCustomView)smartSpeedoMain.findViewById(R.id.ctrl));
		engine_temp=((LinearCustomView)smartSpeedoMain.findViewById(R.id.engineTemp));
		airTemp=((GaugeCustomView)smartSpeedoMain.findViewById(R.id.airTemp));
		
		
		airTemp.setLimits(-10, 50);
		airTemp.setLayout(270, 270, 10, 1);
		airTemp.setValue(0);
		airTemp.setType(GaugeCustomView.TYPE_TEMP);

		engine_temp.setLimits(40, 120);
		engine_temp.setLayout(0, 0, 15, 5, 70,90);
		engine_temp.setValue(40);

		rpm.setLimits(0, 5500);
		rpm.setLayout(180, 270, 1000, 50);
		//		rpm.setLimits(0, 18000);
		//		rpm.setLayout(180, 270, 1000, 200);
		rpm.setValue(3255); // rpm
		rpm.setType(GaugeCustomView.TYPE_RPM);
		
		speed.setLimits(0, 250);
		speed.setLayout(180, 270, 20, 10);
		//		rpm.setLimits(0, 18000);
		//		rpm.setLayout(180, 270, 1000, 200);
		speed.setValue(32); // rpm
		speed.setType(GaugeCustomView.TYPE_KMH);


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
		speed.setValue(i);		
	}


	public void setGear(int i) {
//		rpm.setSecondValue(i,3);		
	}


	public void setRPM(int i) {
		rpm.setValue(i);		
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
	
	public void setMap(double lati,double longitude){};
}
