package com.jkw.smartspeedo;


import android.content.Context;
import android.content.SharedPreferences;
import android.content.res.Resources;
import android.content.res.TypedArray;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Paint;
import android.graphics.Paint.Style;
import android.graphics.Rect;
import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.support.v4.view.ViewCompat;
import android.util.AttributeSet;
import android.util.TypedValue;
import android.view.MotionEvent;
import android.view.View;
import android.view.animation.AccelerateDecelerateInterpolator;

public class GaugeCustomView extends View {

	private int max_value=200;
	private int min_value=0;
	private int current_value=0;
	private int set_value=0;
	private float start_angle=0;

	private double handsScaleFactor;


	boolean mIsStopwatch=true;
	boolean mIsRunning=false;
	private static final String KEY_STATE = "state_bool";
	private static final String KEY_LASTTIME = "lasttime";
	private static final String KEY_NOWTIME = "currenttime_int";
	private static final String KEY_COUNTDOWN_SUFFIX = "_cd";

	private Bitmap mBackgroundImage;
	private int mBackgroundStartY;
	private int mAppOffsetX = 0;
	private int mAppOffsetY = 0;
	private float mMinsAngle = 0;
	private float mSecsAngle = 0;
	private int mDisplayTimeMillis = 0;  //max value is 100hours, 360000000ms
	private final float twoPI = (float) (Math.PI * 2.0);
	private boolean mStopwatchMode = true;
	private long mTouching = 0;

	private int mCanvasWidth = 320;
	private int mCanvasHeight = 480;
	private int mSecsCenterX = 156;
	private int mSecsCenterY = 230;
	private int mMinsCenterX = 156;
	private int mMinsCenterY = 185;

	private int mSecsHalfWidth = 0;
	private int mSecsHalfHeight = 0;
	private int mMinsHalfWidth = 0;
	private int mMinsHalfHeight = 0;
	//private Paint mBackgroundPaint;

	/**
	 * Used to figure out elapsed time between frames
	 */
	private long mLastTime = 0;
	private Drawable mSecHand;
	private Drawable mMinHand;
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
		Resources res = getResources();
		//the stopwatch graphics are square, so find the smallest dimension they must fit in and load appropriately
		int minDim = Math.min(mCanvasHeight, mCanvasWidth);
		final BitmapFactory.Options options = new BitmapFactory.Options();
		options.inScaled = false;

		mBackgroundImage = BitmapFactory.decodeResource(res, R.drawable.clock, options);
		//		Resources r = getResources();
		//		int px = Math.round(TypedValue.applyDimension(TypedValue.COMPLEX_UNIT_DIP, 200, r.getDisplayMetrics()));
		mBackgroundImage = Bitmap.createScaledBitmap(mBackgroundImage, mCanvasHeight, mCanvasWidth, false);

		//        if (minDim >= 1000) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background1000 : R.drawable.background1000_cd, options);
		handsScaleFactor = 0.35;
		//        } else if (minDim >= 720) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background720 : R.drawable.background720_cd, options);
		//            handsScaleFactor = 1;
		//        } else if (minDim >= 590) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background590 : R.drawable.background590_cd, options);
		//            handsScaleFactor = 0.82;
		//        } else if (minDim >= 460) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background460 : R.drawable.background460_cd, options);
		//            handsScaleFactor = 0.64;
		//        } else if (minDim >= 320) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background320 : R.drawable.background320_cd, options);
		//            handsScaleFactor = 0.444; 
		//        } else if (minDim >= 240) {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background240 : R.drawable.background240_cd, options);
		//            handsScaleFactor = 0.333;
		//        } else {
		//            mBackgroundImage = BitmapFactory.decodeResource(res, mIsStopwatch ? R.drawable.background150 : R.drawable.background150_cd, options);
		//            handsScaleFactor = 0.208;
		//        }

		mSecHand = res.getDrawable(R.drawable.jkwhand);
		mMinHand = res.getDrawable(R.drawable.minhand);

		mSecsHalfWidth = mSecHand.getIntrinsicWidth() / 2;   //  42
		mSecsHalfHeight = mSecHand.getIntrinsicHeight() / 2; // 849/600/x=0.35

		handsScaleFactor = (minDim*0.49525)/mSecsHalfHeight;

		mMinsHalfWidth = mMinHand.getIntrinsicWidth() / 2; 
		mMinsHalfHeight = mMinHand.getIntrinsicHeight() / 2;

		mMinsHalfHeight = (int) ((double) mMinsHalfHeight * (1+handsScaleFactor));
		mMinsHalfWidth = (int) ((double) mMinsHalfWidth * (1+handsScaleFactor));
		mSecsHalfHeight = (int) ((double) mSecsHalfHeight * handsScaleFactor);
		mSecsHalfWidth = (int) ((double) mSecsHalfWidth * handsScaleFactor);

		//		mBackgroundStartY = (mCanvasHeight - mBackgroundImage.getHeight()) / 2;
		mBackgroundStartY=0;
		mAppOffsetX = (mCanvasWidth - mBackgroundImage.getWidth()) / 2;

		if (mBackgroundStartY < 0)
			mAppOffsetY = -mBackgroundStartY;

		mSecsCenterY = mBackgroundStartY + (mBackgroundImage.getHeight() / 2); //new graphics have watch center in center
		mMinsCenterY = mBackgroundStartY + (mBackgroundImage.getHeight() * 314 / 1000);//mSecsCenterY - 44;
		mSecsCenterX = mCanvasWidth / 2;
		mMinsCenterX = mCanvasWidth / 2;
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
		// Draw the background image
		if (mBackgroundImage != null)
			canvas.drawBitmap(mBackgroundImage, mAppOffsetX, mBackgroundStartY + mAppOffsetY, null);

		// draw the mins hand with its current rotatiom
		if (mSecHand != null) {

			Paint paintText = new Paint(Paint.ANTI_ALIAS_FLAG);
			paintText.setColor(Color.BLACK);
			paintText.setTextSize(Math.round(500*handsScaleFactor));
			paintText.setStyle(Style.FILL);

			Rect rectText = new Rect();
			String shownText="";
			if(true){
				shownText=String.valueOf(set_value);
			}
			paintText.getTextBounds(shownText, 0, shownText.length(), rectText);

			canvas.drawText(shownText,  (mCanvasWidth-rectText.width())/2, (mCanvasHeight-rectText.height())/2+rectText.height(), paintText);

			// Draw the secs hand with its current rotation
			canvas.save();

			canvas.rotate((float) Math.toDegrees(mSecsAngle), mSecsCenterX, mSecsCenterY + mAppOffsetY);
			mSecHand.setBounds(mSecsCenterX - mSecsHalfWidth, mSecsCenterY - mSecsHalfHeight + mAppOffsetY,
					mSecsCenterX + mSecsHalfWidth, mSecsCenterY + mAppOffsetY + mSecsHalfHeight);
			mSecHand.draw(canvas);
			canvas.restore();
		}
	}

	public void setLimits(final int min, final int max){
		min_value=min;
		max_value=max;
	}

	public void setStartAngle(final int deg){
		start_angle=twoPI*deg/360;
	}

	public void setValue(final int value){
		if(value>=min_value && value<=max_value){
			set_value=value;
			mSecsAngle=(set_value*twoPI)/max_value+start_angle;
			invalidate();
		}
	}

	public int getValue(){
		return set_value;
	}


	//Stopwatch and countdown animation runnable
	private final Runnable animator = new Runnable() {
		@Override
		public void run() {
			if(mIsRunning)
			{
				invalidate();
				removeCallbacks(this);
				ViewCompat.postOnAnimation(GaugeCustomView.this, this);
			}
		}
	};

	/**
	 * Update the time
	 */
	private void updateWatchState(boolean appResuming) {
		long now = System.currentTimeMillis();


		if (mIsRunning) {
			if (mIsStopwatch)
				mDisplayTimeMillis += (now - mLastTime);
			else
				mDisplayTimeMillis -= (now - mLastTime);
		} else {
			mLastTime = now;
		}

		// mins is 0 to 30
		mMinsAngle = twoPI * (mDisplayTimeMillis / 1800000.0f);
		mSecsAngle = twoPI * mDisplayTimeMillis / 60000.0f;

		if (mDisplayTimeMillis < 0) mDisplayTimeMillis = 0;

		// send the time back to the Activity to update the other views
		broadcastClockTime(mIsStopwatch ? mDisplayTimeMillis : -mDisplayTimeMillis);
		mLastTime = now;

		// stop timer at end
		if (mIsRunning && !mIsStopwatch && mDisplayTimeMillis <= 0) {
			notifyCountdownComplete(appResuming);
		}
	}

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


	private void start() {
		mLastTime = System.currentTimeMillis();
		mIsRunning = true;

		//vibrate
		//        if(SettingsActivity.isVibrate()){
		//            Vibrator v = (Vibrator) getContext().getSystemService(Context.VIBRATOR_SERVICE);
		//            v.vibrate(20);
		//        }

		removeCallbacks(animator);
		post(animator);
	}

	protected void stop() {
		mIsRunning = false;

		//vibrate
		//        if(SettingsActivity.isVibrate()){
		//            Vibrator v = (Vibrator) getContext().getSystemService(Context.VIBRATOR_SERVICE);
		//            v.vibrate(20);
		//        }

		removeCallbacks(animator);
	}

	public boolean isRunning() {
		return mIsRunning;
	}

	public double getWatchTime() {
		return mDisplayTimeMillis;
	}

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
		if (savedState != null) {
			mIsRunning = (savedState.getBoolean(KEY_STATE + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), false));
			mLastTime = savedState.getLong(KEY_LASTTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), System.currentTimeMillis());
			mDisplayTimeMillis = savedState.getInt(KEY_NOWTIME + (mStopwatchMode ? "" : KEY_COUNTDOWN_SUFFIX), 0);
			updateWatchState(true);

			removeCallbacks(animator);
			if (mIsRunning) post(animator);
		}
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
