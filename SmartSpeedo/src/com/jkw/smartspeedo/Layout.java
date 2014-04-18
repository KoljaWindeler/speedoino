package com.jkw.smartspeedo;

public interface Layout {
	public int get_layout();
	public void find_elements(SmartSpeedoMain smartSpeedoMain);
	public void unregister_elements(SmartSpeedoMain smartSpeedoMain);
	public void setAirTemp(float f);
	public void setSpeed(int i);
	public void setGear(int i);
	public void setRPM(int i);
	public void setCTRL_left(boolean b);
	public void setOILtemp(int i);
	public void setCTRL_GPS(boolean b);
	public void setCTRL_right(boolean b);
	public void setMap(double lati,double longitude);
}
