#include "global.h"

Speedo_aktors::Speedo_aktors(){
	m_stepper=new speedo_stepper();
};

Speedo_aktors::~Speedo_aktors(){
};

void Speedo_aktors::init(){
	pinMode(RGB_IN_R,OUTPUT);
	pinMode(RGB_IN_G,OUTPUT);
	pinMode(RGB_IN_B,OUTPUT);
	pinMode(RGB_OUT_R,OUTPUT);
	pinMode(RGB_OUT_G,OUTPUT);
	pinMode(RGB_OUT_B,OUTPUT);
	set_rgb_in(255,255,255);
	set_rgb_out(0,0,0);
	dimm_step=0;
	dimm_steps=0;
	//dimm_rgb_to(0,0,255,256,0); // äußeren auf 0,0,255 dimmen in 256*10ms
};


void Speedo_aktors::set_rgb_in(int r,int g,int b){
	if(r>255) r=255; else if(r<0) r=0;
	if(g>255) g=255; else if(g<0) g=0;
	if(b>255) b=255; else if(b<0) b=0;

	analogWrite(RGB_IN_R,255-r);
	analogWrite(RGB_IN_G,255-g);
	analogWrite(RGB_IN_B,255-b);

	RGB.inner.r.actual=r;
	RGB.inner.g.actual=g;
	RGB.inner.b.actual=b;
};

void Speedo_aktors::set_rgb_out(int r,int g,int b){
	if(r>255) r=255; else if(r<0) r=0;
	if(g>255) g=255; else if(g<0) g=0;
	if(b>255) b=255; else if(b<0) b=0;

	analogWrite(RGB_OUT_R,r);
	analogWrite(RGB_OUT_G,g);
	analogWrite(RGB_OUT_B,b);

	RGB.outer.r.actual=r;
	RGB.outer.g.actual=g;
	RGB.outer.b.actual=b;
};
/* 10ms pro schritt -> 0 to 256 = 2,56 sec */
void Speedo_aktors::dimm_rgb_to(int r,int g,int b,int max_dimm_steps, int set_in_out){
	if(set_in_out==1){
		RGB.inner.r.to=r;
		RGB.inner.g.to=g;
		RGB.inner.b.to=b;
		RGB.inner.r.from=RGB.inner.r.actual;
		RGB.inner.g.from=RGB.inner.g.actual;
		RGB.inner.b.from=RGB.inner.b.actual;
	} else {
		RGB.outer.r.to=r;
		RGB.outer.g.to=g;
		RGB.outer.b.to=b;
		RGB.outer.r.from=RGB.outer.r.actual;
		RGB.outer.g.from=RGB.outer.g.actual;
		RGB.outer.b.from=RGB.outer.b.actual;
	}
	dimm_steps=max_dimm_steps;
	in_out=set_in_out;
	dimm_step=0; // wir beginnen bei 0 bis max dimm_steps


	TCCR3A = 0x00;
	/* ich denke mal alles umzuschalten => 256 schritte in 2,56 sec
	 * wäre doch okay, also 256 Timer Schritte in 0,01 sec
	 * ein schritt demnach in 0,01/256=0,000039062
	 * das sind in der welt von 16 mhz => 1/16000000*x=0,000039062
	 * x=625 schritte .. das ist doof vorteiler kann 256 oder 1024 sein
	 * 16000000/256/256 = 244 hz -> nur ne gute sekunde
	 * 16000000/1024/256 = 61 hz -> mehr als 4 sekunde, dazu dann mit
	 * 256/2-1 vorladen = 127 -> zack 2 sek
	 */
	// prescale 1024
	TCCR3B |= (1<<CS32) | (1<<CS30);
	TCCR3B &= ~(1<<CS31);
	// 98 vorladen, 157 schritte
	TCNT3H = 0xFF;
	TCNT3L = 0x62;
	// interrupts aktivieren
	TIMSK3 |= (1<<TOIE3);
	TIFR3  |= (1<<TOV3);
};

void Speedo_aktors::timer_overflow(){
	if(in_out==1){
		int r=int(round((float(int(RGB.inner.r.to)-int(RGB.inner.r.from))*dimm_step)/dimm_steps))+int(RGB.inner.r.from);
		int g=int(round((float(int(RGB.inner.g.to)-int(RGB.inner.g.from))*dimm_step)/dimm_steps))+int(RGB.inner.g.from);
		int b=int(round((float(int(RGB.inner.b.to)-int(RGB.inner.b.from))*dimm_step)/dimm_steps))+int(RGB.inner.b.from);
		set_rgb_in(r,g,b);
	} else {
		int r=int(round((float(int(RGB.outer.r.to)-int(RGB.outer.r.from))*dimm_step)/dimm_steps))+int(RGB.outer.r.from);
		int g=int(round((float(int(RGB.outer.g.to)-int(RGB.outer.g.from))*dimm_step)/dimm_steps))+int(RGB.outer.g.from);
		int b=int(round((float(int(RGB.outer.b.to)-int(RGB.outer.b.from))*dimm_step)/dimm_steps))+int(RGB.outer.b.from);
		set_rgb_out(r,g,b);
	}

	// sind am ende ? wenn ja, timer aus und to werte in from speichern,
	// wenn nicht dimm_steps hochzählen und timer wieder vorladen damit er mit 10 ms
	// läuft
	if(dimm_step<dimm_steps){
		dimm_step++;
		// 127 vorladen
		TCNT3H = 0xFF;
		TCNT3L = 0x62;
	} else {
		// timer aus
		TCCR3B &= ~((1<<CS32) | (1<<CS31) | (1<<CS30));
		// aktueller und from wert sind damit der to wert
		if(in_out==1){
			RGB.inner.r.from=RGB.inner.r.to;
			RGB.inner.g.from=RGB.inner.g.to;
			RGB.inner.b.from=RGB.inner.b.to;
		} else {
			RGB.outer.r.from=RGB.outer.r.to;
			RGB.outer.g.from=RGB.outer.g.to;
			RGB.outer.b.from=RGB.outer.b.to;
		};
	};
};

bool Speedo_aktors::dimm_available(){
	if(dimm_step==dimm_steps)
		return true;
	else
		return false;
};

ISR(TIMER3_OVF_vect){
	pAktors->timer_overflow();
}
