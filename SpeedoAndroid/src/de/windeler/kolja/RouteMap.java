package de.windeler.kolja;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.graphics.drawable.Drawable;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.ViewGroup.LayoutParams;
import android.widget.LinearLayout;

import com.google.android.maps.GeoPoint;
import com.google.android.maps.ItemizedOverlay;
import com.google.android.maps.MapActivity;
import com.google.android.maps.MapController;
import com.google.android.maps.MapView;
import com.google.android.maps.OverlayItem;


public class RouteMap extends MapActivity 
{    
	String DEBUG_TAG = "inside_readTextFile";
	public static final String INPUT_FILE_NAME = "leeer";
	MapView mapView; 
	MapController mc;
	GeoPoint p,p_old;
	private OverlayItem speedo_gps_points;
	private Drawable userPic,atmPic;
	private MyItemizedOverlay userPicOverlay;
	private MyItemizedOverlay nearPicOverlay;


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

		String filename = getIntent().getStringExtra(INPUT_FILE_NAME);
		Log.d(DEBUG_TAG, "Start LoadRoute");
		loadRoute(filename,mapView);

		mc.setZoom(13); 
		mapView.invalidate();



	}

	@Override
	protected boolean isRouteDisplayed() {
		// TODO Auto-generated method stub
		return false;
	}

	protected boolean loadRoute(String filename, MapView mapView){
		String line = "";

		Log.d(DEBUG_TAG, "Start LoadRoute");
		userPic = this.getResources().getDrawable(R.drawable.hg_kreis);
		userPicOverlay = new MyItemizedOverlay(userPic);
		//OverlayItem overlayItem = new OverlayItem(geoPoint, "I'm Here!!!", null);
		//userPicOverlay.addOverlay(overlayItem); // crash
		mapView.getOverlays().add(userPicOverlay);


		atmPic = this.getResources().getDrawable(R.drawable.hg_kreis);
		nearPicOverlay = new MyItemizedOverlay(atmPic);



		try {
			// File zum Lesen oeffenen
			FileInputStream in = new FileInputStream(filename);

			// Wenn das File existiert zum einlesen vorbereiten
			InputStreamReader input = new InputStreamReader(in);
			BufferedReader buffreader = new BufferedReader(input);


			// Die Datei zeilenweise einlesen und in sb appenden
			// Der appender wird hier benutzt um das ganze zu beschleunigen
			// Operation mit + wäre hier viel zu langsam.
			int i=0;
			int zoomlevel = mapView.getZoomLevel();
			int min_u_grad = 300;
			double cos=1;


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

						double abstand=999999;
						if(i>0){
							double abstand_x=Math.pow(Math.abs(p.getLatitudeE6()-p_old.getLatitudeE6()),2);
							double abstand_y=Math.pow(Math.abs(p.getLongitudeE6()-p_old.getLongitudeE6()),2)*cos;
							abstand=Math.sqrt(abstand_x+abstand_y);
						} else {
							p_old= new GeoPoint(p.getLatitudeE6(),p.getLongitudeE6());
							cos=Math.abs(Math.PI*2*111*Math.cos(p.getLongitudeE6())/360);
						}
						
						/* jetzt wirds tricky,
						 * wir wissen den abstand in µGrad
						 * Ein Längengrad ist zwischen 0 und 111km/Grad groß
						 * Ein Breitengrad immer 111km/Grad
						 * Abhängig vom Zoomlevel müssen wir jetzt gucken
						 */
						
						//Log.d(DEBUG_TAG,"Abstand in yGrad:"+String.valueOf(abstand));
						if(abstand>min_u_grad){
							//Log.d(DEBUG_TAG,"New overlay");
							speedo_gps_points = new OverlayItem(p,"Name", null);//just check the brackets i just made change here so....
							//Log.d(DEBUG_TAG,"add overlay");
							nearPicOverlay.addOverlay(speedo_gps_points);
							p_old= new GeoPoint(p.getLatitudeE6(),p.getLongitudeE6());
						}

						i++;

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


		mapView.getOverlays().add(nearPicOverlay);
		mc.animateTo(p);
		return true;
	}

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



