package com.jkw.smartspeedo;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.LinearGradient;
import android.graphics.Paint;
import android.graphics.RadialGradient;
import android.graphics.Paint.Style;
import android.graphics.Shader;
import android.graphics.Shader.TileMode;
import android.graphics.Path;
import android.util.AttributeSet;
import android.view.View;

public class ControllCustomView extends View  {
	private int mCanvasWidth = 320;
	private int mCanvasHeight = 480;
	private int total_slots = 7;//+2+2;
	private int edges=12;

	private Bitmap sym_gps;
	private Bitmap sym_bt;
	private Bitmap sym_fl_left;
	private Bitmap sym_fl_right;
	private Bitmap sym_oil;
	private Bitmap sym_highbeam;
	private Bitmap sym_ngear;

	private int hl_gps=0;
	private int hl_bt=0;
	private int hl_left=0;
	private int hl_right=0;
	private int hl_oil=0;
	private int hl_hb=0;
	private int hl_ng=0;

	Paint paint = new Paint(Paint.ANTI_ALIAS_FLAG);
	Path wallpath = new Path();

	public ControllCustomView(Context context, AttributeSet attrs) {
		super(context, attrs);
	}

	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);

		paint.setShader(null);
		paint.setStrokeWidth(5);
		paint.setColor(0xff000000);


		int maxHeight=(int) (mCanvasHeight-paint.getStrokeWidth());
		wallpath.reset(); // only needed when reusing this path for a new build
		wallpath.moveTo(0,0); // used for first point
		wallpath.lineTo(mCanvasWidth/edges, maxHeight);
		wallpath.lineTo(mCanvasWidth*(edges-1)/edges, maxHeight);
		wallpath.lineTo(mCanvasWidth,0);
		wallpath.lineTo(0,0); // there is a setLastPoint action but i found it not to work as expected



		paint.setStyle(Paint.Style.FILL);
		paint.setShader(new LinearGradient(0,0,0,(int)(maxHeight*2),0xff000000,0xff999999,Shader.TileMode.CLAMP));
		paint.setColor(0xBB000000);
		canvas.drawPath(wallpath, paint);

		//		paint.setStyle(Paint.Style.STROKE);
		//		canvas.drawPath(wallpath, paint);
		//		
		paint.setStyle(Paint.Style.STROKE);
		paint.setShader(null);
		paint.setStrokeWidth(3);
		paint.setColor(0xff000000);
		canvas.drawPath(wallpath, paint);

		int high=70;	
		int low=30;		
		paint.setAlpha(low);
		Paint rel_obj=null;


		///////////////////// left ///////////
		if(hl_left==1){
			rel_obj=null;
		} else {
			rel_obj=paint;
		}
		canvas.drawBitmap(sym_fl_left, getSlot(0), (float) (0.15 * maxHeight), rel_obj);
		///////////////////// left ///////////
		///////////////////// bt ///////////
		if(hl_bt==1){
			rel_obj=null;
		} else {
			rel_obj=paint;
		}
		canvas.drawBitmap(sym_bt, getSlot(1), (float) (0.15 * maxHeight), rel_obj);
		///////////////////// bt ///////////
		///////////////////// gps ///////////
		if(hl_gps==1){ 
			rel_obj=null;
		} else {
			rel_obj=paint;
		}
		canvas.drawBitmap(sym_gps, getSlot(2), (float) (0.15 * maxHeight), rel_obj);
		///////////////////// gps ///////////
		paint.setAlpha(low+high*hl_ng);
		canvas.drawBitmap(sym_ngear, getSlot(3), (float) (0.15 * maxHeight), paint);
		paint.setAlpha(low+high*hl_oil);
		canvas.drawBitmap(sym_oil, getSlot(4), (float) (0.15 * maxHeight), paint);
		paint.setAlpha(low+high*hl_hb);
		canvas.drawBitmap(sym_highbeam, getSlot(5), (float) (0.15 * maxHeight), paint);	
		paint.setAlpha(low+high*hl_right);
		canvas.drawBitmap(sym_fl_right, getSlot(6), (float) (0.15 * maxHeight), paint);

	}

	private int getSlot(final int slot){
		return (slot*mCanvasWidth*(edges-2))/(edges*total_slots)+mCanvasWidth/edges;
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


	private void init() {
		Bitmap bMap = BitmapFactory.decodeResource(getResources(), R.drawable.gps_icon);
		sym_gps = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.bluetooth_icon);
		sym_bt = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.pfeil_l);
		sym_fl_left = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.pfeil);
		sym_fl_right = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.oil);
		sym_oil = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.hb);
		sym_highbeam = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );

		bMap = BitmapFactory.decodeResource(getResources(), R.drawable.ng);
		sym_ngear = Bitmap.createScaledBitmap(bMap, (int)(0.7*mCanvasHeight/bMap.getHeight()*bMap.getWidth()),(int)(0.7*mCanvasHeight ),true );
	};

	public void set_BT(boolean active){
		hl_bt=0;
		if(active){
			hl_bt=1;
		}
		invalidate();
	}

	public void set_GPS(boolean active){
		hl_gps=0;
		if(active){
			hl_gps=1;
		}
		invalidate();
	}

	public void set_left(boolean active){
		hl_left=0;
		if(active){
			hl_left=1;
		}
		invalidate();
	}

	public void set_right(boolean active){
		hl_right=0;
		if(active){
			hl_right=1;
		}
		invalidate();
	}

	public boolean get_left(){
		if(hl_left>0) return true;
		return false;
	}
}
