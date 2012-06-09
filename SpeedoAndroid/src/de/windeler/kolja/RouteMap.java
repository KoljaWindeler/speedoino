package de.windeler.kolja;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.View.OnTouchListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.OverlayItem;


public class RouteMap extends MapActivity implements OnTouchListener
{    
	String DEBUG_TAG = "inside_readTextFile";
	public static final String INPUT_FILE_NAME = "leeer";
	MapView mapView; 
	MapController mc;
	GeoPoint p,p2,mapViewCenter;
	String filename;
	int Zoomlevel;
	boolean is_touched=false;
	private OverlayItem speedo_gps_points;
	private Drawable userPic,userPOIPic;
	private MyItemizedOverlay userPicOverlay;
	private MyItemizedOverlay nearPicOverlay,nearPicPOIOverlay;
	private ArrayList<GeoPoint> gps_points = new ArrayList<GeoPoint>();
	private ArrayList<GeoPoint> POI_points = new ArrayList<GeoPoint>();



	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.map);

		mapView = (MapView) findViewById(R.id.mapView);
		LinearLayout zoomLayout = (LinearLayout)findViewById(R.id.zoom);  
		@SuppressWarnings("deprecation")
		View zoomView = mapView.getZoomControls(); 

		zoomLayout.addView(zoomView, 
				new LinearLayout.LayoutParams(
						LayoutParams.WRAP_CONTENT, 
						LayoutParams.WRAP_CONTENT)); 
		mapView.displayZoomControls(true);

		mc = mapView.getController();
		mapView.setOnTouchListener(this);

		mc.setZoom(12);  // 3=Weltkarte, 21=super dicht
		Zoomlevel=mapView.getZoomLevel();
		mapViewCenter=mapView.getMapCenter();

		filename = getIntent().getStringExtra(INPUT_FILE_NAME);
		Log.d(DEBUG_TAG, "Start LoadRoute");

		loadRoutefromFile(filename);
		loadVisiblePoints(mapView);


		mapView.invalidate();
	}


	@Override
	public boolean onTouch(View v, MotionEvent event) {
		if (event.getAction() == 1) {
			is_touched = false;
		} else {
			is_touched = true;
		}

		return super.onTouchEvent(event);
	};


	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	protected void onResume()
	{
		super.onResume();
		handler.postDelayed(zoomChecker, zoomCheckingDelay);
	}

	protected void onPause()
	{
		super.onPause();
		handler.removeCallbacks(zoomChecker); // stop the map from updating
	}


	protected boolean loadRoutefromFile(String filename){
		gps_points.clear();
		POI_points.clear();
		String line = "";
		try {
			// File zum Lesen oeffenen
			FileInputStream in = new FileInputStream(filename);

			// Wenn das File existiert zum einlesen vorbereiten
			InputStreamReader input = new InputStreamReader(in);
			BufferedReader buffreader = new BufferedReader(input);


			try {
				while ((line = buffreader.readLine()) != null) {
					//Log.d(DEBUG_TAG, "LogLine: " + line + " length "+ String.valueOf(line.length()));
					if(line.length()==55){
						double latitude=Double.parseDouble(line.substring(14, 23));
						double longitude=Double.parseDouble(line.substring(24, 33));
						//Log.d(DEBUG_TAG, "Koordinaten: " + String.valueOf(longitude) + " / "+ String.valueOf(latitude));
						latitude=Math.floor(latitude/1000000.0)*1000000+Math.round((latitude%1000000.0)*10/6);
						longitude=Math.floor(longitude/1000000.0)*1000000+Math.round((longitude%1000000.0)*10/6);
						p = new GeoPoint((int) (latitude),(int) (longitude));
						gps_points.add(p);
						if(Integer.parseInt(line.substring(54,55))==1){
							//Log.d(DEBUG_TAG, "POI: " + line);
							POI_points.add(p);
						}
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
				Log.d(DEBUG_TAG, "Error reading Line from File: " + e.getMessage());
			}

			try {
				in.close();
			} catch (Exception e) {
				e.printStackTrace();
				Log.d(DEBUG_TAG, "Error closing File: " + e.getMessage());

			}


		} catch (java.io.FileNotFoundException e) {
			e.printStackTrace();
			Log.d(DEBUG_TAG, "File not found: " + e.getMessage());

		}
		return false;
	}

	protected boolean is_point_visible(GeoPoint p, GeoPoint topLeft, GeoPoint bottomRight){
		if(p.getLatitudeE6()>topLeft.getLatitudeE6())
			return false;
		else if(p.getLatitudeE6()<bottomRight.getLatitudeE6())
			return false;
		else if(p.getLongitudeE6()<topLeft.getLongitudeE6())
			return false;
		else if(p.getLongitudeE6()>bottomRight.getLongitudeE6())
			return false;
		else
			return true;
	}

	protected boolean loadVisiblePoints(MapView mapView){

		Log.d(DEBUG_TAG, "Start loadVisiblePoints");
		userPic = this.getResources().getDrawable(R.drawable.hg_kreis);
		userPOIPic = this.getResources().getDrawable(R.drawable.hg_kreis);
		userPicOverlay = new MyItemizedOverlay(userPic);
		//OverlayItem overlayItem = new OverlayItem(geoPoint, "I'm Here!!!", null);
		//userPicOverlay.addOverlay(overlayItem); // crash
		mapView.getOverlays().add(userPicOverlay);


		userPic = this.getResources().getDrawable(R.drawable.hg_kreis);
		nearPicOverlay = new MyItemizedOverlay(userPic);

		userPOIPic = this.getResources().getDrawable(R.drawable.hg_kreispoi);
		nearPicPOIOverlay = new MyItemizedOverlay(userPOIPic);


		int min_abstand = 0;
		int zoomlevel = mapView.getZoomLevel(); // 3=Weltkarte, 21=Details 
		Log.d(DEBUG_TAG,"Zoomlevel="+String.valueOf(zoomlevel));

		if(zoomlevel>=20) min_abstand=0;			// maxdetails
		else if(zoomlevel>=19) min_abstand=10;			// maxdetails
		else if(zoomlevel>=18) min_abstand=15;			// maxdetails
		else if(zoomlevel>=17) min_abstand=25;			// maxdetails
		else if(zoomlevel>=16) min_abstand=50;			// maxdetails
		else if(zoomlevel>=15) min_abstand=100;
		else if(zoomlevel>=14) min_abstand=200;
		else if(zoomlevel>=13) min_abstand=400;
		else if(zoomlevel>=12) min_abstand=800;
		else if(zoomlevel>=8) min_abstand=1600;
		else min_abstand=1000;


		Location von = new Location("dummyprovider");
		Location bis = new Location("dummyprovider");

		// screen coordinates
		GeoPoint topLeft = mapView.getProjection().fromPixels(mapView.getLeft(), mapView.getTop());
		GeoPoint bottomRight = mapView.getProjection().fromPixels(mapView.getRight(), mapView.getBottom());
		GeoPoint center = mapView.getMapCenter();

		int breite=Math.abs(bottomRight.getLatitudeE6()-topLeft.getLatitudeE6());
		int hoehe=Math.abs(topLeft.getLongitudeE6()-bottomRight.getLongitudeE6()); 
		topLeft = new GeoPoint((int) (center.getLatitudeE6()+hoehe*4/5),(int) (center.getLongitudeE6()-breite*4/5));
		bottomRight = new GeoPoint((int) (center.getLatitudeE6()-hoehe*4/5),(int) (center.getLongitudeE6()+breite*4/5));
		// screen coordinates


		for(int i=0;i<gps_points.size();i++){
			p = gps_points.get(i);

			//Log.d(DEBUG_TAG, "von wird gesetzt");
			von.setLongitude(p.getLongitudeE6()/1E6);
			von.setLatitude(p.getLatitudeE6()/1E6);

			//Log.d(DEBUG_TAG, "von gesetzt");


			double abstand=999999;
			if(i>0){
				abstand=von.distanceTo(bis);
				//Log.d(DEBUG_TAG, "Von:"+String.valueOf(von.getLatitude())+"/"+String.valueOf(von.getLongitude())+" -> "+String.valueOf(bis.getLatitude())+"/"+String.valueOf(bis.getLongitude())+" abstand "+String.valueOf(abstand));
			} else {
				bis.setLatitude(von.getLatitude());
				bis.setLongitude(von.getLongitude());
			}

			/* jetzt wirds tricky,
			 * wir wissen den abstand in µGrad
			 * Ein Längengrad ist zwischen 0 und 111km/Grad groß
			 * Ein Breitengrad immer 111km/Grad
			 * Abhängig vom Zoomlevel müssen wir jetzt gucken
			 */

			//Log.d(DEBUG_TAG,"Abstand in yGrad:"+String.valueOf(abstand));
			if(abstand>min_abstand && is_point_visible(p,topLeft,bottomRight)){
				//Log.d(DEBUG_TAG,"New overlay");
				speedo_gps_points = new OverlayItem(p,"Name", null);//just check the brackets i just made change here so....
				//Log.d(DEBUG_TAG,"add overlay");
				nearPicOverlay.addOverlay(speedo_gps_points);
				bis.setLatitude(von.getLatitude());
				bis.setLongitude(von.getLongitude());
			}
		}

		// add ALL POI, since there are always just a few
		for(int i=0;i<POI_points.size();i++){
			p2 = POI_points.get(i);
			speedo_gps_points = new OverlayItem(p2,"Name", null);//just check the brackets i just made change here so....
			//Log.d(DEBUG_TAG,"add overlay");
			nearPicPOIOverlay.addOverlay(speedo_gps_points);
		};

		mapView.getOverlays().clear();
		mapView.getOverlays().add(nearPicOverlay);
		mapView.getOverlays().add(nearPicPOIOverlay);
		mc.animateTo(p);
		return true;
	}


	private Handler handler = new Handler();
	public static final int zoomCheckingDelay = 500; // in ms
	private Runnable zoomChecker = new Runnable()
	{
		public void run()
		{    
			if(!is_touched){
				if(Zoomlevel!=mapView.getZoomLevel() || (mapViewCenter.getLatitudeE6()!=mapView.getMapCenter().getLatitudeE6() || mapViewCenter.getLongitudeE6()!=mapView.getMapCenter().getLongitudeE6())){
					GeoPoint center=mapView.getMapCenter();
					loadVisiblePoints(mapView);
					mc.animateTo(center);
					Zoomlevel=mapView.getZoomLevel();
					mapViewCenter=mapView.getMapCenter();
				};
			}
			handler.removeCallbacks(zoomChecker); // remove the old callback
			handler.postDelayed(zoomChecker, zoomCheckingDelay); // register a new one

		}
	};


	public class MyItemizedOverlay extends ItemizedOverlay<OverlayItem> {

		private ArrayList<OverlayItem> myOverlays ;

		public MyItemizedOverlay(Drawable defaultMarker) {
			super(boundCenterBottom(defaultMarker));
			myOverlays = new ArrayList<OverlayItem>();
			populate();
		}

		public void addOverlay(OverlayItem overlay){
			myOverlays.add(overlay);
			populate();
		}

		@Override
		protected OverlayItem createItem(int i) {
			return myOverlays.get(i);
		}

		// Removes overlay item i
		public void removeItem(int i){
			myOverlays.remove(i);
			populate();
		}

		// Returns present number of items in list
		@Override
		public int size() {
			return myOverlays.size();
		}


		public void addOverlayItem(OverlayItem overlayItem) {
			myOverlays.add(overlayItem);
			populate();
		}


		public void addOverlayItem(int lat, int lon, String title) {
			try {
				GeoPoint point = new GeoPoint(lat, lon);
				OverlayItem overlayItem = new OverlayItem(point, title, null);
				addOverlayItem(overlayItem);    
			} catch (Exception e) {
				// TODO: handle exception
				e.printStackTrace();
			}
		}

		//	    @Override
		//	    protected boolean onTap(int index) {
		//	        // TODO Auto-generated method stub
		//	        String title = myOverlays.get(index).getTitle();
		//	        Toast.makeText(ShowMapActivity.context, title, Toast.LENGTH_LONG).show();
		//	        return super.onTap(index);
		//	    }
	}



}



