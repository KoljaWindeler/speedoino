package com.jkw.smartspeedo;

public class converter {
	private double meter_per_pulse;

	public converter(){
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

	public int rpm(int rpm_in) {
		return rpm_in*2;
	}
	
	public int speed_freq_to_kmh(int freq){
		return (int)(freq*meter_per_pulse);
	}
}
