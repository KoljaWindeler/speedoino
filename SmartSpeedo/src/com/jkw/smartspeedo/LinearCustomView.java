package com.jkw.smartspeedo;


import android.content.Context;
import android.content.SharedPreferences;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.os.Handler;
import android.util.AttributeSet;
import android.view.MotionEvent;
import android.view.View;

public class LinearCustomView extends View {

	public static final int TYPE_FUEL=  1;
	public static final int TYPE_TEMP = 2;


	private int type=TYPE_FUEL;

	private int max_value=200;
	private int min_value=0;
	private int value_count=1;
	private int [] set_value = new int[] { 0,0,0};

	private int offset_left=10;
	private int offset_right=10;
	private int number_of_bars=6;
	private int partial_lengh_of_separators=10;
	private int min=40;
	private int max=120;
	private int progression_start=60;
	private int progression_end=100;
	private float progression_hardness=2;
	private float x_endof_progression_start;
	private float x_startof_progression_end;
	private float scale_low;
	private float scale_high;


	private int progression_high=100;
	private int progression_low=0;
	private int step_size_big_strokes=10;
	private int step_size_small_strokes=2;
	private int start_offset=40;
	private int end_offset=40;

	private float steps_big_strokes;
	private float dist_big_strokes;
	private float steps_small_strokes;
	private float dist_small_strokes;


	Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
	Rect rectText = new Rect();

	private int mCanvasWidth = 320;
	private int mCanvasHeight = 480;

	//pass back messages to UI thread
	private Handler mHandler;

	public LinearCustomView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	private void init() {
		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
		// progression low: 60
		// progression high: 100
		// min: 40
		// max: 120
		// big step:10
		// soll werte: 40,60,70,80,90,100,120
		// jetzt müssen wir erstmal wissen wie groß der untere progression bereich ist
		// (Math.max(min_value,progression_low)-min_value)/(2*step_size_big_strokes) = (max(40,60)-40)/(2*10)=(60-40)/20=1
		// dann den bereich zwischen den progressions
		// (Math.min(max_value,progression_high)-Math.max(min_value,progression_low))/(step_size_big_strokes) = (min(120,100)-max(40,60))/10=(100-60)/10=4
		// und dann noch den ab progression high
		// (max_value-Math.min(max_value,progression_high))/(2*step_size_big_strokes)=(120-min(120,100))/(2*10)=(120-100)/20=1

		//		steps_big_strokes=(Math.max(min_value,progression_low)-min_value)/(2*step_size_big_strokes);
		//		steps_big_strokes+=(Math.min(max_value,progression_high)-Math.max(min_value,progression_low))/(step_size_big_strokes);
		//		steps_big_strokes+=(max_value-Math.min(max_value,progression_high))/(2*step_size_big_strokes);
		//		dist_big_strokes=(mCanvasWidth-start_offset-end_offset)/steps_big_strokes;
		//		steps_small_strokes=step_size_big_strokes/Math.max(1, step_size_small_strokes);
		//		dist_small_strokes=dist_big_strokes/(float)step_size_big_strokes*step_size_small_strokes;
	}


	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		// Account for padding
		int xpad = (getPaddingLeft() + getPaddingRight());
		int ypad = (getPaddingTop() + getPaddingBottom());

		mCanvasWidth = w - xpad;
		mCanvasHeight = h - ypad;
		init();
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);




		int number_of_separators=number_of_bars-1; // e.g. 7
		int total_bardisplay_length=mCanvasWidth-offset_left-offset_right;  // e.g. 2000-400-400 = 1200
		float length_of_bars=total_bardisplay_length/((float)number_of_separators/partial_lengh_of_separators+number_of_bars); // 580/(6,5)=89.23
		float length_of_separtors=length_of_bars/partial_lengh_of_separators;	// 8.923

		scale_low=((Math.max(progression_start,min)-min)/progression_hardness+	Math.min(progression_end,max)-Math.max(progression_start,min) + (Math.max(max,progression_end)-progression_end)/progression_hardness)/total_bardisplay_length; 	// e.g. ((60-40)/2 + 100-60 + (120-100)/2)/100 = (10+40+10)/100 = 0.06
		scale_high=scale_low*progression_hardness; // e.g. 0.12

		x_endof_progression_start=(Math.max(progression_start,min)-min)/scale_high; // e.g.  (60-40)/0.12=166.6
		x_startof_progression_end=total_bardisplay_length-(Math.max(progression_end,max)-progression_end)/scale_high; // e.g. 1000-(120-100)/0.12=833.33

		for(int i=0;i<number_of_bars;i++){ // 0..7
			float start_x=i*length_of_bars+(i)*length_of_separtors; // 8,92 | ... 
			float start_value=pos_to_value(start_x); //  41,84 | ...

			// check if we have to draw anything at all
			if(start_value < set_value[0]) {
				// now lets see if we are able to draw the complete bar 
				float end_x=(i+1)*length_of_bars+(i)*length_of_separtors; //98.15
				float end_value=pos_to_value(end_x); // 60.15				
				if(set_value[0]<end_value){
					// do NOT draw the complete box
					// lets see if we crossed a border of a progression or we have at least a "static" increase
					int start_section=0;
					if(start_x>x_startof_progression_end) {
						start_section+=2;
					} else if(start_x>x_endof_progression_start){
						start_section+=1;
					}

					int end_section=0;
					if(end_x>x_startof_progression_end){
						end_section+=2;
					} else if(end_x>x_endof_progression_start){
						end_section+=1;
					}

					if(start_section==end_section){
						// the complete bar would be in the same section, so we just have to correct the length
						end_x=((set_value[0]-start_value)/(end_value-start_value)+i)*length_of_bars+(i+1)*length_of_separtors;
					} else {
						// we have different sections *yeahi*, so first: check if the left or the right part is interrupted
						float border=0;
						if(start_section==0){
							border=x_endof_progression_start;
						} else if(start_section==1){
							border=x_startof_progression_end;
						}
						float border_value=pos_to_value(border);
						if(border_value>set_value[0]){ 
							// we just need the partitial left part
							end_x=i*length_of_bars+(i+1)*length_of_separtors + (set_value[0]-start_value)/(border_value-start_value)*(border-start_x);
						} else {
							// we need the complete left part + a partitial box on the right
							end_x=i*length_of_bars+(i+1)*length_of_separtors;
							end_x+=(border-end_x)+((set_value[0]-border_value)/(end_value-border_value))*(length_of_bars-(border-start_x));
						}
					}
				}
				Paint mPaint = new Paint();
				mPaint.setStyle(Paint.Style.FILL);
			    mPaint.setColor(Color.WHITE); 
				canvas.drawRect(start_x+offset_left,0,end_x+offset_left,mCanvasHeight,mPaint);
			}
		} // for
	}


//		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
//		int length_big_stroke=(int)Math.round(mCanvasHeight*4/5);
//		int length_small_stroke=length_big_stroke/2;
//
//
//		//		paint.setStyle(Paint.Style.STROKE);
//		paint.setStyle(Paint.Style.FILL);
//		paint.setStrokeWidth(2);
//
//		for(int i=0;i<=steps_big_strokes;i++){
//			// generate start / endpoint for big strokes
//			float x_start=(float) ((dist_big_strokes*i)+start_offset);
//			float y_start=(float)(0);
//			float y_end=(float) (mCanvasHeight/2);
//
//			// to check if the storke should be white or gray we have to calculate the current value
//			int marker_value=i*step_size_big_strokes+min_value;
//			if(i>0){
//				if((2*step_size_big_strokes)*i+min_value>progression_low){ // wir sind schon drüber hinaus
//					int steps_in_low_progression=(Math.max(progression_low,min_value)-min_value)/(2*step_size_big_strokes);
//					marker_value+=steps_in_low_progression*step_size_big_strokes; //einfach aufaddieren weil das ja doppelt war
//				} else { // wir sind noch drin
//					marker_value+=i*step_size_big_strokes;
//				}
//
//				if(marker_value>progression_high){ // eventuell noch die high addieren
//					marker_value+=(marker_value-progression_high);
//				}
//			}
//
//			paint.setColor(0xff333333);
//			if(value_count>1){
//				if(marker_value<=Math.min(set_value[0],set_value[1])){
//					paint.setColor(0xffffffff);
//				} else if (marker_value<=set_value[0] && marker_value>=set_value[1]){ // 0:water, 1:oil
//					paint.setColor(0xff3333ff); // blue if water is hotter than oil
//				} else if (marker_value>=set_value[0] && marker_value<=set_value[1]){
//					paint.setColor(0xffff3333); // red if oil is hotter than water
//				}
//			} else {
//				if(marker_value<=set_value[0]){
//					paint.setColor(0xffffffff);
//				}
//			}
//
//			// draw the big stroke
//			canvas.drawLine(x_start, y_start, x_start, y_end, paint);
//
//			// write a number next to it
//			paint.setTextSize(Math.max(Math.round(minDim*1/5)-150,25));
//
//			String shownText=String.valueOf(marker_value);
//			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
//			canvas.drawText(shownText,  x_start-rectText.width()/2, mCanvasHeight*4/5, paint);
//
//
//			if(i<steps_big_strokes && step_size_small_strokes>0){
//				for(int ii=1;ii<steps_small_strokes;ii++){
//					x_start=(float) ((dist_big_strokes*i+ii*dist_small_strokes)+start_offset);
//					y_start=(float)(0);
//					y_end=(float) (mCanvasHeight*3/8);
//			
//					int inner_marker_value=marker_value;
//
//					for(int iii=0; iii<ii; iii++){
//						inner_marker_value+=step_size_small_strokes;
//						if(inner_marker_value<progression_low || inner_marker_value>progression_high){ // den unten bereich haben wir noch nicht verlassen
//							inner_marker_value+=step_size_small_strokes;
//						}
//					}
//
//					paint.setColor(0xff333333);
//					if(value_count>1){
//						if(inner_marker_value<=Math.min(set_value[0],set_value[1])){
//							paint.setColor(0xffffffff);
//						} else if (inner_marker_value<=set_value[0] && inner_marker_value>=set_value[1]){
//							paint.setColor(0xff3333ff); // blue if water is hotter than oil
//						} else if (inner_marker_value>=set_value[0] && inner_marker_value<=set_value[1]){
//							paint.setColor(0xffff3333); // red if oil is hotter than water
//						}
//					} else {
//						if(inner_marker_value<=set_value[0]){
//							paint.setColor(0xffffffff);
//						}
//					}
//
//					canvas.drawLine(x_start, y_start, x_start, y_end, paint);
//
//				}
//			}
//		}
//
//
//		paint.setColor(Color.WHITE);
//		paint.setStyle(Style.FILL);

/*if(type==TYPE_RPM){
			Rect rectText2 = new Rect();
			String shownText="";
			String shownText2="";
			shownText=String.valueOf((int)Math.floor(set_value[0]/1000));
			int remaining=(int)(set_value[0]-Math.floor(set_value[0]/1000)*1000);
			shownText2=String.valueOf(remaining);
			if(remaining<10){
				shownText2="00"+shownText2;
			} else if(remaining<100){
				shownText2="0"+shownText2;
			}

			paint.setTextSize(Math.max(Math.round(minDim*3/6)-200,40));
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  ((float)mCanvasWidth/2-paint.measureText(shownText))*6/6, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);

			paint.setTextSize((float) (Math.max(Math.round(minDim*2/3)-200,40)*0.3));
			paint.getTextBounds(shownText2, 0, shownText2.length(), rectText2);
			int save_height=(mCanvasHeight-rectText.height())/2+rectText.height()*3/4;
			canvas.drawText(shownText2,  (float) (mCanvasWidth/2 + rectText2.width()/6), save_height, paint);


			paint.setTextSize(Math.max((Math.round(minDim*3/5)-200)/8,14));
			shownText="rpm";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (float) (mCanvasWidth/2 + rectText2.width()/6), (float) (save_height+rectText.height()*1.2), paint);	
		} 

		else if(type==TYPE_TEMP){
			paint.setTextSize(Math.max(Math.round(minDim*3/5)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value[0])+"°C";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
		} 

		else if(type==TYPE_KMH){
			paint.setTextSize(Math.max(Math.round(minDim*3/5)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value[0]);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);

			int save_height=(mCanvasHeight-rectText.height())/2+rectText.height();
			paint.setTextSize(Math.max((Math.round(minDim*3/5)-200)/3,14));
			shownText="km/h";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (float) (save_height+rectText.height()*1.2), paint);	
		} 

		else {
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value[0]);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);

			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
		}*/
//	}

private float pos_to_value(float pos){
	float value=pos*scale_high+min;
	if(pos>x_endof_progression_start){
		value-=scale_low*(pos-x_endof_progression_start);
	}

	if(pos>x_startof_progression_end){
		value+=scale_low*(pos-x_startof_progression_end);
	}
	return value;
}

public void setLimits(final int min, final int max){
	min_value=min;
	max_value=max;
}


public void setValue(int value){
	if(value<=min_value){
		value=min_value;
	} else if(value>=max_value){
		value=max_value;
	}

	set_value[0]=value;
	invalidate();
}

public void setSecondValue(final int value){
	if(value>=min_value && value<=max_value){
		set_value[1]=value;
		//			mSecsAngle=(set_value*twoPI)/max_value+start_offset;
		invalidate();
	}
}

public void setValueCount(final int valuecount){
	value_count=valuecount;
}

public int getValue(){
	return set_value[0];
}

public void setType(int gauge_type){
	type=gauge_type;
}

public void setLayout(final int abs_start_offset,final int rel_end_angle,final int step_size_big,final int step_size_small){
	setLayout(abs_start_offset, rel_end_angle, step_size_big, step_size_small, min_value, max_value);
}

public void setLayout(final int abs_start_offset,final int rel_end_angle,final int step_size_big,final int step_size_small,final int progression_low_value,final int progression_high_value){
	//		start_offset=abs_start_offset;
	progression_high=progression_high_value;
	progression_low=progression_low_value;
	step_size_big_strokes=step_size_big;
	step_size_small_strokes=step_size_small;
	//		end_angle=rel_end_angle;
}

// Deal with touch events, either start/stop or swipe
@Override
public boolean onTouchEvent(MotionEvent event) {
	return true;
}

/**
 * Dump state to the provided Bundle. Typically called when the
 * Activity is being suspended.
 */
public void saveState(SharedPreferences.Editor map) {

}

/**
 * Restores state from the indicated Bundle. Called when
 * the Activity is being restored after having been previously
 * destroyed.
 */
public synchronized void restoreState(SharedPreferences savedState) {

}

//for optimization purposes
@Override
public boolean isOpaque() {
	return true;
}

}