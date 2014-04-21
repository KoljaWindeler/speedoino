package com.jkw.smartspeedo;

public class Converter {
	private double meter_per_pulse;

	public Converter(){
		// load conversion factor
		meter_per_pulse=2.5;	
		
	}
	
	public float water_r_to_t(int r){
		return r;
	}
	
	public float oil_r_to_t(int r){
		return r;
	}
	
	public float air(int r){
		return r/10;
	}

	public int engine_freq_to_rpm(int rpm_in) {
		// der smartspeedo gibt schon U/min allerdings mit faktor 1/2 aus (hz*30)
		return rpm_in*2;
	}
	
	public int speed_freq_to_kmh(int freq){
		///8.192*2.5m*3.6
		return (int)(freq*meter_per_pulse*3.6/8.192);
	}
}
