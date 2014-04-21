package com.jkw.smartspeedo;


import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.RadialGradient;
import android.graphics.Rect;
import android.graphics.Shader.TileMode;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

public class GaugeCustomView extends View {

	public static final int TYPE_RPM =  1;
	public static final int TYPE_KMH =  2;
	public static final int TYPE_TEMP = 3;
	public static final int TYPE_GEAR = 4;
	public static final int TYPE_KMH_RPM = 5;


	private int type=TYPE_KMH;

	private int max_value=200;
	private int min_value=0;
	private int value_count=1;
	private int [] set_value = new int[] { 0,0,0};
	private float start_angle=0;

	private int progression_high=100;
	private int progression_low=0;
	private int step_size_big_strokes=10;
	private int step_size_small_strokes=2;
	private int end_angle=270;

	private int thickness_blackring;
	private float steps_big_strokes;
	private float angle_big_strokes;
	private float steps_small_strokes;
	private float angle_small_strokes;


	private final float twoPI = (float) (Math.PI * 2.0);
	Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
	Rect rectText = new Rect();
	private RadialGradient gradient;

	private int mCanvasWidth = 320;
	private int mCanvasHeight = 480;
	private boolean lockSize = false;

	//pass back messages to UI thread
	private Handler mHandler;

	public GaugeCustomView(Context context, AttributeSet attrs) {
		super(context, attrs);
		init();
	}

	public GaugeCustomView(Context context, int width, int height) {
		super(context);
		mCanvasHeight = height;
		mCanvasWidth = width;
		lockSize=true;
		init();
	}


	private void init() {
		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
		// the gradient
		int [] colors = new int[] { 0xff000000,0xff000000, 0xff999999 };
		//				int [] colors = new int[] { 0xFFffffff,0xFFffffff, 0xFFff0000};
		float [] positions = new float[] {0.1f,0.8f, 1f };
		gradient = new RadialGradient(mCanvasWidth/2, mCanvasWidth/2, minDim/2-15, colors, positions, TileMode.CLAMP);

		thickness_blackring=30;

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

		steps_big_strokes=(Math.max(min_value,progression_low)-min_value)/(2*step_size_big_strokes);
		steps_big_strokes+=(Math.min(max_value,progression_high)-Math.max(min_value,progression_low))/(step_size_big_strokes);
		steps_big_strokes+=(max_value-Math.min(max_value,progression_high))/(2*step_size_big_strokes);
		angle_big_strokes=end_angle/steps_big_strokes;
		steps_small_strokes=step_size_big_strokes/Math.max(1, step_size_small_strokes);
		angle_small_strokes=angle_big_strokes/(float)step_size_big_strokes*step_size_small_strokes;
	}


	@Override
	protected void onSizeChanged(int w, int h, int oldw, int oldh) {
		// Account for padding
		int xpad = (getPaddingLeft() + getPaddingRight());
		int ypad = (getPaddingTop() + getPaddingBottom());

		if(!lockSize){
			mCanvasWidth = w - xpad;
			mCanvasHeight = h - ypad;
		}
		init();
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
		float radius = minDim/2;
		int length_big_stroke=(int)Math.round(radius/7);
		int length_small_stroke=length_big_stroke/2;


		paint.setStyle(Paint.Style.FILL);
		paint.setDither(true);
		paint.setShader(gradient);
		canvas.drawCircle(mCanvasWidth/2, mCanvasHeight/2, radius-thickness_blackring, paint);

		paint.setShader(null);
		paint.setStyle(Paint.Style.STROKE);
		paint.setStrokeWidth(thickness_blackring);
		paint.setColor(Color.BLACK);
		canvas.drawCircle(mCanvasWidth/2, mCanvasHeight/2, radius-thickness_blackring/2, paint);
		paint.setStrokeWidth(6);
		paint.setColor(Color.WHITE);
		canvas.drawCircle(mCanvasWidth/2, mCanvasHeight/2, (float) ((radius-thickness_blackring/2)*0.99), paint);


		//		paint.setStyle(Paint.Style.STROKE);
		paint.setStyle(Paint.Style.FILL);
		paint.setStrokeWidth(2);
		radius-=thickness_blackring;


		for(int i=0;i<=steps_big_strokes;i++){
			// generate start / endpoint for big strokes
			float x_start=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius-length_big_stroke));
			float x_end=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius));
			float y_start=(float)((mCanvasHeight/2)-Math.cos(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius-length_big_stroke));
			float y_end=(float) ((mCanvasWidth/2)-Math.cos(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius));

			// to check if the storke should be white or gray we have to calculate the current value
			int marker_value=i*step_size_big_strokes+min_value;
			if(i>0){
				if((2*step_size_big_strokes)*i+min_value>progression_low){ // wir sind schon drüber hinaus
					int steps_in_low_progression=(Math.max(progression_low,min_value)-min_value)/(2*step_size_big_strokes);
					marker_value+=steps_in_low_progression*step_size_big_strokes; //einfach aufaddieren weil das ja doppelt war
				} else { // wir sind noch drin
					marker_value+=i*step_size_big_strokes;
				}

				if(marker_value>progression_high){ // eventuell noch die high addieren
					marker_value+=(marker_value-progression_high);
				}
			}

			paint.setColor(0xff333333);
			if(value_count>1){
				if(marker_value<=Math.min(set_value[0],set_value[1])){
					paint.setColor(0xffffffff);
				} else if (marker_value<=set_value[0] && marker_value>=set_value[1]){ // 0:water, 1:oil
					paint.setColor(0xff3333ff); // blue if water is hotter than oil
				} else if (marker_value>=set_value[0] && marker_value<=set_value[1]){
					paint.setColor(0xffff3333); // red if oil is hotter than water
				}
			} else {
				if(marker_value<=set_value[0]){
					paint.setColor(0xffffffff);
				}
			}

			// draw the big stroke
			canvas.drawLine(x_start, y_start, x_end, y_end, paint);

			// write a number next to it
			float distance=(float) (radius-length_big_stroke*1.6);
			if(radius<200){
				distance=(float) (radius-length_big_stroke*2.2);
			}
			x_start=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i)+start_angle)*distance);
			y_start=(float)((mCanvasHeight/2)-Math.cos(twoPI/360*(angle_big_strokes*i)+start_angle)*distance);
			paint.setTextSize(Math.max(Math.round(minDim*1/15),25));

			String shownText=String.valueOf(marker_value);
			if(type==TYPE_RPM || type==TYPE_KMH_RPM){
				shownText=String.valueOf((int)marker_value/1000);
			}
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			//			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2+x_start, (mCanvasHeight-rectText.height())/2+rectText.height()+y_start, paint);
			canvas.drawText(shownText,  x_start-rectText.width()/2, y_start+rectText.height()/2, paint);



			if(i<steps_big_strokes && step_size_small_strokes>0){
				for(int ii=1;ii<steps_small_strokes;ii++){
					x_start=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius-length_small_stroke));
					x_end=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius));
					y_start=(float)((mCanvasHeight/2)-Math.cos(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius-length_small_stroke));
					y_end=(float) ((mCanvasWidth/2)-Math.cos(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius));

					int inner_marker_value=marker_value;

					for(int iii=0; iii<ii; iii++){
						inner_marker_value+=step_size_small_strokes;
						if(inner_marker_value<progression_low || inner_marker_value>progression_high){ // den unten bereich haben wir noch nicht verlassen
							inner_marker_value+=step_size_small_strokes;
						}
					}

					paint.setColor(0xff333333);
					if(value_count>1){
						if(inner_marker_value<=Math.min(set_value[0],set_value[1])){
							paint.setColor(0xffffffff);
						} else if (inner_marker_value<=set_value[0] && inner_marker_value>=set_value[1]){
							paint.setColor(0xff3333ff); // blue if water is hotter than oil
						} else if (inner_marker_value>=set_value[0] && inner_marker_value<=set_value[1]){
							paint.setColor(0xffff3333); // red if oil is hotter than water
						}
					} else {
						if(inner_marker_value<=set_value[0]){
							paint.setColor(0xffffffff);
						}
					}

					canvas.drawLine(x_start, y_start, x_end, y_end, paint);

				}
			}
		}


		paint.setColor(Color.WHITE);
		paint.setStyle(Style.FILL);

		if(type==TYPE_RPM){
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

		else if(type==TYPE_KMH_RPM){
			// 
			paint.setTextSize(Math.max(mCanvasHeight*3/16,40));
			Rect rectText = new Rect();
			String shownText=String.valueOf(set_value[1]);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, mCanvasHeight/4+rectText.height()*3/2, paint);
			int tmp=rectText.height()*3/2;

			paint.setTextSize(Math.max(mCanvasHeight*1/32,40));
			shownText="km/h";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (float) (mCanvasHeight/4+rectText.height()*3/2+tmp), paint);
			//

			paint.setTextSize(Math.max(mCanvasHeight*3/16,40));
			rectText = new Rect();
			shownText=String.valueOf(set_value[2]);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, mCanvasHeight*2/4+rectText.height()*3/2, paint);
			if(set_value[2]>1){
				int tmp2=rectText.height()*3/2;
				shownText=String.valueOf(set_value[2]-1);
				paint.setTextSize(Math.max(mCanvasHeight*3/32,40));
				paint.getTextBounds(shownText, 0, shownText.length(), rectText);
				paint.setColor(0xff333333);
				canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2-rectText.width()*2, mCanvasHeight*2/4+tmp2, paint);
				if(set_value[2]<6){
					shownText=String.valueOf(set_value[2]+1);
					paint.setTextSize(Math.max(mCanvasHeight*3/32,40));
					paint.getTextBounds(shownText, 0, shownText.length(), rectText);
					paint.setColor(0xff333333);
					canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2+rectText.width()*2, mCanvasHeight*2/4+tmp2, paint);	
				}
			}

		}

		else {
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value[0]);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);

			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
		}
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

		if(set_value[0]!=value){
			set_value[0]=value;
			invalidate();
		}
	}

	public void setSecondValue(final int value,final int place){
		if(place==2||place==3){
			if(value>=min_value && value<=max_value && value!=set_value[place-1]){
				set_value[place-1]=value;
				//			mSecsAngle=(set_value*twoPI)/max_value+start_angle;
				invalidate();
			}
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

	public void setLayout(final int abs_start_angle,final int rel_end_angle,final int step_size_big,final int step_size_small){
		setLayout(abs_start_angle, rel_end_angle, step_size_big, step_size_small, min_value, max_value);
	}

	public void setLayout(final int abs_start_angle,final int rel_end_angle,final int step_size_big,final int step_size_small,final int progression_low_value,final int progression_high_value){
		start_angle=twoPI*abs_start_angle/360;;
		progression_high=progression_high_value;
		progression_low=progression_low_value;
		step_size_big_strokes=step_size_big;
		step_size_small_strokes=step_size_small;
		end_angle=rel_end_angle;
	}


	//Stopwatch and countdown animation runnable
	//	private final Runnable animator = new Runnable() {
	//		@Override
	//		public void run() {
	//			if(mIsRunning)
	//			{
	//				invalidate();
	//				removeCallbacks(this);
	//				ViewCompat.postOnAnimation(GaugeCustomView.this, this);
	//			}
	//		}
	//	};

	/**
	 * Update the time
	 */
	//	private void updateWatchState(boolean appResuming) {
	//		long now = System.currentTimeMillis();
	//
	//
	//		if (mIsRunning) {
	//			if (mIsStopwatch)
	//				mDisplayTimeMillis += (now - mLastTime);
	//			else
	//				mDisplayTimeMillis -= (now - mLastTime);
	//		} else {
	//			mLastTime = now;
	//		}
	//
	//		// mins is 0 to 30
	//		mMinsAngle = twoPI * (mDisplayTimeMillis / 1800000.0f);
	//		mSecsAngle = twoPI * mDisplayTimeMillis / 60000.0f;
	//
	//		if (mDisplayTimeMillis < 0) mDisplayTimeMillis = 0;
	//
	//		// send the time back to the Activity to update the other views
	//		broadcastClockTime(mIsStopwatch ? mDisplayTimeMillis : -mDisplayTimeMillis);
	//		mLastTime = now;
	//
	//		// stop timer at end
	//		if (mIsRunning && !mIsStopwatch && mDisplayTimeMillis <= 0) {
	//			notifyCountdownComplete(appResuming);
	//		}
	//	}

	// Deal with touch events, either start/stop or swipe
	@Override
	public boolean onTouchEvent(MotionEvent event) {
		//		if (event.getAction() == MotionEvent.ACTION_UP) {
		//			setValue((int) (getValue()*1.1+3));
		//		} else if (event.getAction() == MotionEvent.ACTION_DOWN) {
		//			setValue((int) (getValue()*0.9+3));
		//		}
		//		invalidate();
		return true;
	}


	//	private void start() {
	//		mLastTime = System.currentTimeMillis();
	//		mIsRunning = true;
	//
	//		//vibrate
	//		//        if(SettingsActivity.isVibrate()){
	//		//            Vibrator v = (Vibrator) getContext().getSystemService(Context.VIBRATOR_SERVICE);
	//		//            v.vibrate(20);
	//		//        }
	//
	//		removeCallbacks(animator);
	//		post(animator);
	//	}

	//	protected void stop() {
	//		mIsRunning = false;
	//
	//		//vibrate
	//		//        if(SettingsActivity.isVibrate()){
	//		//            Vibrator v = (Vibrator) getContext().getSystemService(Context.VIBRATOR_SERVICE);
	//		//            v.vibrate(20);
	//		//        }
	//
	//		removeCallbacks(animator);
	//	}
	//
	//	public boolean isRunning() {
	//		return mIsRunning;
	//	}

	//	public double getWatchTime() {
	//		return mDisplayTimeMillis;
	//	}

	/**
	 * Dump state to the provided Bundle. Typically called when the
	 * Activity is being suspended.
	 */
	public void saveState(SharedPreferences.Editor map) {
		//        if (!mIsStopwatch || mDisplayTimeMillis > 0) {
		//            if (!mIsStopwatch && mDisplayTimeMillis > 0 && mIsRunning) {
		//                AlarmUpdater.setCountdownAlarm(getContext(), (long) mDisplayTimeMillis);
		//            } else {
		//                AlarmUpdater.cancelCountdownAlarm(getContext()); //just to be sure
		//            }
		//
		//            map.putBoolean(KEY_STATE + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), mIsRunning);
		//            map.putLong(KEY_LASTTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), mLastTime);
		//            map.putInt(KEY_NOWTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), mDisplayTimeMillis);
		//        } else {
		//            map.clear();
		//        }
	}

	/**
	 * Restores state from the indicated Bundle. Called when
	 * the Activity is being restored after having been previously
	 * destroyed.
	 */
	public synchronized void restoreState(SharedPreferences savedState) {
		//		if (savedState != null) {
		//			mIsRunning = (savedState.getBoolean(KEY_STATE + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), false));
		//			mLastTime = savedState.getLong(KEY_LASTTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), System.currentTimeMillis());
		//			mDisplayTimeMillis = savedState.getInt(KEY_NOWTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), 0);
		//			updateWatchState(true);
		//
		//			removeCallbacks(animator);
		//			if (mIsRunning) post(animator);
		//		}
		//        AlarmUpdater.cancelCountdownAlarm(getContext()); //just to be sure
	}

	//for optimization purposes
	@Override
	public boolean isOpaque() {
		return true;
	}

}
