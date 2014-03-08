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
import android.view.MotionEvent;
import android.view.View;

public class GaugeCustomView extends View {

	public static final int TYPE_RPM =  1;
	public static final int TYPE_KMH =  2;
	public static final int TYPE_TEMP = 3;
	public static final int TYPE_GEAR = 4;


	private int type=TYPE_KMH;

	private int max_value=200;
	private int min_value=0;
	private int current_value=0;
	private int set_value=0;
	private float start_angle=0;

	private int progression_start=100;
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
	private RadialGradient gradient;

	private int mCanvasWidth = 320;
	private int mCanvasHeight = 480;

	//pass back messages to UI thread
	private Handler mHandler;

	public GaugeCustomView(Context context, AttributeSet attrs) {
		super(context, attrs);

		//find out if this view is specificed as a stopwatch or countdown view
		//        TypedArray a = context.getTheme().obtainStyledAttributes(
		//                attrs,
		//                R.styleable.StopwatchCustomView,
		//                0, 0);

		//        try {
		//            mIsStopwatch = a.getBoolean(R.styleable.StopwatchCustomView_watchType, true);
		//        } finally {
		//            a.recycle();
		//        }

		init();
	}

	private void init() {
		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
		// the gradient
		int [] colors = new int[] { 0xff000000,0xff000000, 0xffff0000 };
		float [] positions = new float[] {0.1f,0.8f, 1f };
		gradient = new RadialGradient(mCanvasWidth/2, mCanvasWidth/2, minDim/2-15, colors, positions, TileMode.CLAMP);
		
		thickness_blackring=30;
		steps_big_strokes=(Math.min(progression_start,max_value)-min_value)/step_size_big_strokes+(max_value-Math.min(progression_start,max_value))/(2*step_size_big_strokes);
		angle_big_strokes=end_angle/steps_big_strokes;
		steps_small_strokes=step_size_big_strokes/Math.max(1, step_size_small_strokes);
		angle_small_strokes=angle_big_strokes/(float)step_size_big_strokes*step_size_small_strokes;
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


		paint.setStyle(Paint.Style.STROKE);
		radius-=thickness_blackring;


		for(int i=0;i<=steps_big_strokes;i++){
			float x_start=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius-length_big_stroke));
			float x_end=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius));
			float y_start=(float)((mCanvasHeight/2)-Math.cos(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius-length_big_stroke));
			float y_end=(float) ((mCanvasWidth/2)-Math.cos(twoPI/360*(angle_big_strokes*i)+start_angle)*(radius));

		
			int marker_value=i*step_size_big_strokes+min_value;
			if(marker_value>progression_start){
				marker_value=(int) (progression_start+(i-Math.floor((progression_start-min_value)/step_size_big_strokes))*step_size_big_strokes*2);
			}			
			if(marker_value<=set_value){
				paint.setColor(0xffffffff);
			} else {
				paint.setColor(0xff333333);
			}

			canvas.drawLine(x_start, y_start, x_end, y_end, paint);

			if(i<steps_big_strokes && step_size_small_strokes>0){
				for(int ii=1;ii<steps_small_strokes;ii++){
					x_start=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius-length_small_stroke));
					x_end=(float) ((mCanvasWidth/2)+Math.sin(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius));
					y_start=(float)((mCanvasHeight/2)-Math.cos(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius-length_small_stroke));
					y_end=(float) ((mCanvasWidth/2)-Math.cos(twoPI/360*(angle_big_strokes*i+ii*angle_small_strokes)+start_angle)*(radius));

					int inner_marker_value=marker_value;
					if(i*step_size_big_strokes+ii*step_size_small_strokes+min_value>progression_start){
						inner_marker_value+=ii*step_size_small_strokes*2; 
					} else {
						inner_marker_value+=ii*step_size_small_strokes;
					}
					if(inner_marker_value<=set_value){
						paint.setColor(0xffffffff);
					} else {
						paint.setColor(0xff333333);
					}

					canvas.drawLine(x_start, y_start, x_end, y_end, paint);
				}
			}
		}




		paint.setColor(Color.WHITE);
		paint.setStyle(Style.FILL);

		if(type==TYPE_RPM){
			Rect rectText = new Rect();
			Rect rectText2 = new Rect();
			String shownText="";
			String shownText2="";
			shownText=String.valueOf((int)Math.floor(set_value/1000));
			int remaining=(int)(set_value-Math.floor(set_value/1000)*1000);
			shownText2=String.valueOf(remaining);
			if(remaining<10){
				shownText2="00"+shownText2;
			} else if(remaining<100){
				shownText2="0"+shownText2;
			}
			
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			paint.setTextSize((float) (Math.max(Math.round(minDim*2/3)-200,40)*0.3));
			paint.getTextBounds(shownText2, 0, shownText2.length(), rectText2);
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width()*2)/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
			paint.setTextSize((float) (Math.max(Math.round(minDim*2/3)-200,40)*0.3));
			canvas.drawText(shownText2,  (float) (mCanvasWidth/2 + rectText2.width()/6), (mCanvasHeight-rectText.height())/2+rectText.height()*3/4, paint);
		} 
		
		else if(type==TYPE_TEMP){
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value)+"°C";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
		} 
		
		else if(type==TYPE_KMH){
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
			
			int save_height=rectText.height();
			paint.setTextSize(Math.max((Math.round(minDim*2/3)-200)/3,14));
			shownText="km/h";
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);
			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (float) ((mCanvasHeight-rectText.height())/2+rectText.height()*2+save_height/2), paint);	
		} 
		
		else {
			paint.setTextSize(Math.max(Math.round(minDim*2/3)-200,40));
			Rect rectText = new Rect();
			String shownText="";
			shownText=String.valueOf(set_value);
			paint.getTextBounds(shownText, 0, shownText.length(), rectText);

			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paint);
		}
	}

	public void setLimits(final int min, final int max){
		min_value=min;
		max_value=max;
	}

	
	public void setValue(final int value){
		if(value>=min_value && value<=max_value){
			set_value=value;
			//			mSecsAngle=(set_value*twoPI)/max_value+start_angle;
			invalidate();
		}
	}

	public int getValue(){
		return set_value;
	}

	public void setType(int gauge_type){
		type=gauge_type;
	}
	
	public void setLayout(final int abs_start_angle,final int rel_end_angle,final int step_size_big,final int step_size_small,final int progression_start_value){
		start_angle=twoPI*abs_start_angle/360;;
		progression_start=progression_start_value;
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
		//        if (event.getAction() == MotionEvent.ACTION_DOWN) {
		//            SoundManager sm = SoundManager.getInstance(getContext());
		//            if (sm.isEndlessAlarmSounding()) {
		//                sm.stopEndlessAlarm();
		//            } else {
		//                mTouching = System.currentTimeMillis();
		//            }
		//        } else if (event.getAction() == MotionEvent.ACTION_MOVE) {
		//            if (mTouching > 0 && System.currentTimeMillis() - mTouching > 1000)
		//                mTouching = 0L;   //reset touch if user is swiping
		//        } else if (event.getAction() == MotionEvent.ACTION_UP) {
		//            if (mTouching > 0) startStop();
		//            mTouching = 0L;
		//        }
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
		notifyStateChanged();
		//        AlarmUpdater.cancelCountdownAlarm(getContext()); //just to be sure
	}

	//for optimization purposes
	@Override
	public boolean isOpaque() {
		return true;
	}

	//Message Handling between Activity/Fragment and View
	public void setHandler(Handler handler) {
		this.mHandler = handler;
	}

	private void notifyStateChanged() {
		Bundle b = new Bundle();
		//        b.putBoolean(UltimateStopwatchActivity.MSG_STATE_CHANGE, true);
		sendMessageToHandler(b);
	}

	private void notifyIconHint() {
		Bundle b = new Bundle();
		//        b.putBoolean(CountdownFragment.MSG_REQUEST_ICON_FLASH, true);
		sendMessageToHandler(b);
	}

	private void notifyCountdownComplete(boolean appResuming) {
		Bundle b = new Bundle();
		//        b.putBoolean(CountdownFragment.MSG_COUNTDOWN_COMPLETE, true);
		//        b.putBoolean(CountdownFragment.MSG_APP_RESUMING, appResuming);
		sendMessageToHandler(b);
	}

	//send the latest time to the parent fragment to populate the digits
	private void broadcastClockTime(double mTime) {
		Bundle b = new Bundle();
		//        b.putBoolean(UltimateStopwatchActivity.MSG_UPDATE_COUNTER_TIME, true);
		//        b.putDouble(UltimateStopwatchActivity.MSG_NEW_TIME_DOUBLE, mTime);
		sendMessageToHandler(b);
	}

	private void sendMessageToHandler(Bundle b) {
		if (mHandler != null) {
			Message msg = mHandler.obtainMessage();
			msg.setData(b);
			mHandler.sendMessage(msg);
		}
	}


}
