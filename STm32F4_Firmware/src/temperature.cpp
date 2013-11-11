/* Speedoino - This file is part of the firmware.
 * Copyright (C) 2011 Kolja Windeler
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "global.h"

temperature::temperature(){
	oil_temp_value_counter=0;
	oil_temp_value=0;
	oil_temp_fail_status=0; // 0 = no failed read, 1-5 = Number of shorts read, 6-9 = Number of open read

	water_temp_value_counter=0;
	water_temp_value=0;
	water_temp_fail_status=0; // 0 = no failed read, 1-5 = Number of shorts read, 6-9 = Number of open read

	air_temp_value=0;

	water_warning_temp=0; // 95 C
	oil_warning_temp=0; // 120 C
	// values for
	for(unsigned int j=0; j<sizeof(oil_r_werte)/sizeof(oil_r_werte[0]); j++){
		oil_r_werte[j]=0;
		oil_t_werte[j]=0;

		water_r_werte[j]=0;
		water_t_werte[0]=0;
	};
};

temperature::~temperature(){
};

int temperature::check_vars(){
	if(water_r_werte[0]==0 || water_t_werte[0]==0 || oil_t_werte[0]==0 || oil_r_werte[0]==0){
		// Temperatur und Widerstands LookUp
		// OIL
		int r_werte[19]={1000,700,550,400,330,250,230,210,195,150,140,135,110,100, 90, 80, 20, 15, 10}; // widerstandswerte
		int t_werte[19]={  27, 35, 45, 50, 55, 60, 65, 70, 75, 80, 85, 90, 95,100,105,110,115,120,125}; // passender Temperaturwert

		for(unsigned int j=0; j<sizeof(oil_r_werte)/sizeof(oil_r_werte[0]); j++){
			oil_r_werte[j]=r_werte[j];
			oil_t_werte[j]=t_werte[j];
		};

		// Water
		int r_werte2[19]={354,323,241,198,157,135,111,93,80,64,52,43,37,30, 23, 18, 15, 10,  9}; // widerstandswerte
		int t_werte2[19]={ 30, 35, 40, 45, 50, 55, 60,65,70,75,80,85,90,95,100,105,110,115,116}; // passender Temperaturwert
		for(unsigned int j=0; j<sizeof(water_r_werte)/sizeof(water_r_werte[0]); j++){
			water_r_werte[j]=r_werte2[j];
			water_t_werte[j]=t_werte2[j];
		};

		water_warning_temp=950; // 95 C
		oil_warning_temp=1200; // 120 C
		Serial.puts_ln(USART1,("temp failed"));
		return 1;
	};

	return 0;
}

void temperature::init(){
	// configure analog input B0 for water and C1 oil
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure; // create new datatype
	/* Configure PB0 (ADC Channel ADC12_IN8) as analog input */
	/* ADC12_IN8 means channel 8 for ADC1 OR ADC2 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	// enable Clock for ADC1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	//	// ADC-Config TODO nötig?
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
	ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStructure);

	/* ADC1 Configuration */
	ADC_InitTypeDef ADC_InitStructure;
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
	ADC_Init(ADC1, &ADC_InitStructure);

	// ADC1 regular channel 8 configuration: witch adc, witch channel, groupe sequencer, sample time
	//(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
	//	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_480Cycles);
	ADC_Cmd(ADC1, ENABLE);   // Enable ADC1
	//	ADC_SoftwareStartConv(ADC1); //Start ADC1 Conversion


	//	I2c.begin();
	//	I2c.setSpeed(0); /// fast
	//	I2c.timeOut(100); // 100 ms to get Temperature
	Serial.puts_ln(USART1,("Temp init done."));
}

void temperature::read_oil_temp() {
	// werte in array speichern in °C*10 für Nachkommastelle
#ifdef TEMP_DEBUG
	Serial.puts_ln(USART1,"\n\rTemp: Beginne Öl zu lesen");
#endif

	//// Widerstandsberechnung ////
	/*	Spannungsabgriff zwischen 2 Widerständen, R1 ggn 5V da drunter in Reihe "R".
	 *  R=U/I, U=R*I, I=U/R
	 *  Spannung über dem zu messenden Widerstand:  Uabgriff = R*I
	 *  I=5V/(R+R1)
	 *  Uabgriff = 5V*analogValue/1024 = R*5V/(R+R1)
	 *  5*analogValue/1024*R + 5*analogValue/1024*R1 = R*5
	 *  5*analogValue/1024*R1 = R*(5-5*analogValue/1024)
	 *  R= (5*analogValue/1024*R1)/(5-5*analogValue/1024) ; "5" Kürzen
	 *  R= (analogValue/1024*R1)/(1-1*analogValue/1024) ; "1024" nach unten
	 *  R= (analogValue*R1)/(1024-analogValue) ; "1024" nach unten
	 *   Widerstandsberechnung *////

	// werte auslesen TODO neuer wertebereich?
	ADC_RegularChannelConfig(ADC1,ADC_Channel_8,1,ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1); //Start ADC1 Conversion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	uint16_t oil_value=ADC_GetConversionValue(ADC1);//analogRead(OIL_TEMP_PIN);
	uint16_t temp=(4096-oil_value)/10; // max 409
	if(temp>0 && temp<409){
		int r_temp=round((oil_value*5.5)/temp); // 22*1024 < 32000
		//		Serial.puts(USART1,"Oel Wert eingelesen: ");
		//		Serial.puts(USART1,oil_value);
		//		Serial.puts(USART1," zwischenschritt ");
		//		Serial.puts_ln(USART1,r_temp);
		// LUT  wert ist z.B. 94°C somit 102 ohm => dann wird hier in der for schleife bei i=13 ausgelößt, also guck ich mir den her + den davor an
		for(int i=0;i<19;i++){ // 0 .. 18 müssen durchsucht werden das sind die LUT positionen
			if(r_temp>=oil_r_werte[i]){ // den einen richtigen raussuchen
				int j=i-1;  if(j<0) j=0; // j=12
				int offset=r_temp-oil_r_werte[i]; // wieviel höher ist mein messwert als den, den ich nicht wollte => 102R-100R => 2R
				int differ_r=oil_r_werte[j]-oil_r_werte[i]; // wie weit sind die realen widerstands werte auseinander 10R
				int differ_t=oil_t_werte[i]-oil_t_werte[j]; // wie weit sind die realen temp werte auseinander 5°C
				int aktueller_wert=round(10*(oil_t_werte[i]-offset*differ_t/differ_r));
				oil_temp_value=Sensors.flatIt(aktueller_wert,&oil_temp_value_counter,20,oil_temp_value);

#ifdef TEMP_DEBUG
				Serial.puts(USART1,"Oel Wert eingelesen: ");
				Serial.puts(USART1,oil_value);
				Serial.puts(USART1," und interpretiert ");
				Serial.puts(USART1,aktueller_wert);
				Serial.puts(USART1," und geplaettet: ");
				Serial.puts_ln(USART1,int(round(oil_temp_value)));
#endif
				oil_temp_fail_status=0;
				break; // break the for loop
			};
		};
	} else if(temp==0) { // kein Sensor  0=(1024-x)/10		x>=1015
#ifdef TEMP_DEBUG
		Serial.puts(USART1,"@");
		Serial.puts(USART1,Millis.get());
		Serial.puts(USART1,": oil Wert -> OPEN");
		Serial.puts(USART1," | Oil fail status:");
		Serial.puts_ln(USART1,(int)oil_temp_fail_status);
#endif

		if(oil_temp_fail_status<6){
			oil_temp_fail_status=6;
		} else if(oil_temp_fail_status<9){
			oil_temp_fail_status++;
		}
	} else { // Kurzschluss nach masse: 102=(1024-x)/10  	x<=4
		if(oil_temp_fail_status<5){ // nach sechs maligem fehler => ausgabe!
			oil_temp_fail_status++;
		};

#ifdef TEMP_DEBUG
		Serial.puts(USART1,"@");
		Serial.puts(USART1,Millis.get());
		Serial.puts(USART1,": oil Wert -> Short to GND");
		Serial.puts(USART1," | Oil fail status:");
		Serial.puts_ln(USART1,(int)oil_temp_fail_status);
#endif
	}
};

void temperature::read_water_temp() {
	// werte in array speichern in °C*10 für Nachkommastelle
#ifdef TEMP_DEBUG
	Serial.puts_ln(USART1,"\n\rTemp: Beginne Water zu lesen");
#endif

	// werte auslesen // TODO change pin + range warning
	ADC_RegularChannelConfig(ADC1,ADC_Channel_9,1,ADC_SampleTime_480Cycles);
	ADC_SoftwareStartConv(ADC1); //Start ADC1 Conversion
	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	uint16_t water_value=ADC_GetConversionValue(ADC1);//analogRead(WATER_TEMP_PIN);
	/* kann bis zu 225.060 werden bei 40°C etwa 470 ohm: 470+220=690 Ohm, U/R=I => 5/690=0,007246377A, R*I=U, 1,594202899V, Wert=326
	 * wenn temp < 102 sein soll, dann ergibt 1020 schon ein error, bei 102=(1024-X)/10 => x <= 4
	 * Da haben wir also 4*5V/1024 über dem PT100 und den Rest über 390Ohm
	 * 0,01953125 V über dem PT100 und 4,98046875V über 390Ohm, -> PT100: 1,529411765 Ohm
	 */
	int temp=(1024-water_value)/10; // max 102
	if(temp>0 && temp<102){
		int r_temp=round((int)((unsigned long)(water_value*39)/temp)); // 39*1024 > 32000
		//Serial.puts(USART1,"Oel Wert eingelesen: "); Serial.puts(USART1,oil_value); Serial.puts(USART1," zwischenschritt "); Serial.puts_ln(USART1,r_temp);
		// LUT  wert ist z.B. 94°C somit 102 ohm => dann wird hier in der for schleife bei i=13 ausgelößt, also guck ich mir den her + den davor an
		// wenn unser ermittelter R KLEINER ist als der kleinste R, dann haben wir da mehr grad als maximum und brauchen nicht interpolieren
		if(r_temp<water_r_werte[18]){
			water_temp_value=Sensors.flatIt(10*water_t_werte[18],&water_temp_value_counter,60,water_temp_value);
		} else {
			for(int i=0;i<19;i++){ // 0 .. 18 müssen durchsucht werden das sind die LUT positionen
				if(r_temp>=water_r_werte[i]){ // den einen richtigen raussuchen
					int j=i-1;  if(j<0) j=0; // j=12
					int offset=r_temp-water_r_werte[i]; // wieviel höher ist mein messwert als den, den ich nicht wollte => 102R-100R => 2R
					int differ_r=water_r_werte[j]-water_r_werte[i]; // wie weit sind die realen widerstands werte auseinander 10R
					int differ_t=water_t_werte[i]-water_t_werte[j]; // wie weit sind die realen temp werte auseinander 5°C
					int aktueller_wert=round(10*(water_t_werte[i]-offset*differ_t/differ_r));
					water_temp_value=Sensors.flatIt(aktueller_wert,&water_temp_value_counter,60,water_temp_value);

#ifdef TEMP_DEBUG
					Serial.puts(USART1,"Water Wert eingelesen: ");
					Serial.puts(USART1,water_value);
					Serial.puts(USART1," und interpretiert als R ");
					Serial.puts(USART1,r_temp);
					Serial.puts(USART1," und geplaettet: ");
					Serial.puts_ln(USART1,int(round(water_temp_value)));
#endif
					water_temp_fail_status=0;
					break; // break the for loop
				};
			};
		};
	} else if(temp==0) { // kein Sensor  0=(1024-x)/10		x>=1015
		if(water_temp_fail_status<6){
			water_temp_fail_status=6;
		} else if(water_temp_fail_status<9){
			water_temp_fail_status++;
		}
	} else { // Kurzschluss nach masse: 102=(1024-x)/10  	x<=4
		if(water_temp_fail_status<5){ // nach sechs maligem fehler => ausgabe!
			water_temp_fail_status++;
		};

#ifdef TEMP_DEBUG
		Serial.puts(USART1,"Water Wert Kurzschluss ggn Masse");
#endif
	}
};

void temperature::read_air_temp() {
	return;

	/*
	// get i2c tmp102 //
#ifdef TEMP_DEBUG
	Serial.puts_ln(USART1,"beginne air read");
#endif
	int sensorAddress = 0b01001000;  // From datasheet sensor address is 0x91 shift the address 1 bit right
	byte msb;
	byte lsb;
	I2c.read(sensorAddress,0x00,2);
#ifdef TEMP_DEBUG
	Serial.puts_ln(USART1,"request abgeschickt");
#endif

	if (I2c.available() >= 2){  // if two bytes were received
#ifdef TEMP_DEBUG
		Serial.puts_ln(USART1,"2 Byte im Puffer");
#endif
		msb = I2c.receive();  // receive high byte (full degrees)
		lsb = I2c.receive();  // receive low byte (fraction degrees)
		air_temp_value = ((msb) << 3);  // MSB
		air_temp_value|= (lsb >> 5);    // LSB
		air_temp_value = round(air_temp_value*1.25); // round and save
#ifdef TEMP_DEBUG
		Serial.puts(USART1,"Air value "); Serial.puts_ln(USART1,air_temp_value);
#endif
	} else {
		air_temp_value = 999;
#ifdef TEMP_DEBUG
		Serial.puts_ln(USART1,"ALARM keine Antwort vom I2C sensor!!");
#endif
	};
	 */
};


int temperature::get_air_temp(){
	if(air_temp_value>999) // notfall
		return 0;
	else
		return air_temp_value;
}

int temperature::get_oil_temp(){
	if(Speedo.trip_dist[2]==0 && get_air_temp()!=999 && oil_temp_value!=8888 && oil_temp_value!=9999) // wir sind heute noch exakt gar nicht gefahren
		return get_air_temp()-1;
	else
		return int(round(oil_temp_value));
}

int temperature::get_water_temp(){
	if(Speedo.trip_dist[2]==0  && get_air_temp()!=999 && water_temp_value!=8888 && water_temp_value!=9999) // wir sind heute noch exakt gar nicht gefahren
		return get_air_temp()-1;
	else
		return int(round(water_temp_value));
}
