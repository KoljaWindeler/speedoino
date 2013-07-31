package de.windeler.kolja;

//Route editor => log zu kml

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.Intent;
import android.graphics.drawable.Drawable;
import android.location.Location;
import android.net.Uri;
import android.os.Bundle;
import android.os.Handler;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
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
	String date;
	private MenuItem mMenuItemConnect;
	int Zoomlevel;
	boolean is_touched=false;
	boolean lap_marker_in_file=false;
	private OverlayItem speedo_gps_points;
	private Drawable userPic,userPOIPic;
	private MyItemizedOverlay userPicOverlay;
	private MyItemizedOverlay nearPicOverlay,nearPicPOIOverlay;
	private ArrayList<GeoPoint> gps_points = new ArrayList<GeoPoint>();
	private ArrayList<Integer> add_info_speed = new ArrayList<Integer>();
	private ArrayList<String> add_info_time = new ArrayList<String>();
	private ArrayList<Integer> add_info_special = new ArrayList<Integer>();



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
		if(!filename.substring(filename.lastIndexOf('.')+1).toUpperCase().equals("GPS")){
			// show dialog	
			AlertDialog alertDialog = new AlertDialog.Builder(this).create();
			alertDialog.setTitle("Warning");
			alertDialog.setMessage("This is not a GPS file from the Speedoino");
			alertDialog.setButton("OK",new DialogInterface.OnClickListener() {
				@Override
				public void onClick(DialogInterface arg0, int arg1) {	
					//upload_animation=true;	
					finish();
				}});
			alertDialog.show();

		} else {

			loadRoutefromFile(filename);
			loadVisiblePoints(mapView);
			mapView.invalidate();
		};
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
		add_info_speed.clear();
		add_info_special.clear();
		add_info_time.clear();
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
					//z.b. 111543,160613,052230803,00333,004,00227,4294964509,06,2,0
					/* 	0 gps_time[a]
						1 gps_date[a],
						2 gps_lati[a],
						3 gps_long[a],
						4 gps_speed_arr[a],
						5 gps_course[a],
						6 gps_alt[a],
						7 gps_sats[a],
						8 gps_fix[a],
						9 gps_special[a]);
					 */
					try {
						if(line.replaceAll("[^,]","").length()==9){ // exakt 9 ","
							String[] line_splitted = line.split(",");
							// Koordinaten
							double latitude=Double.parseDouble(line_splitted[2]);
							double longitude=Double.parseDouble(line_splitted[3]);
							//Log.d(DEBUG_TAG, "Koordinaten: " + String.valueOf(longitude) + " / "+ String.valueOf(latitude));
							latitude=Math.floor(latitude/1000000.0)*1000000+Math.round((latitude%1000000.0)*10/6); // nmea to dec!!
							longitude=Math.floor(longitude/1000000.0)*1000000+Math.round((longitude%1000000.0)*10/6); // nmea to dec!!
							p = new GeoPoint((int) (latitude),(int) (longitude));
							gps_points.add(p);
							// speed
							int temp_int=Integer.parseInt(line_splitted[4]);
							add_info_speed.add(temp_int);
							// time & date
							add_info_time.add(line_splitted[0]);
							date=line_splitted[1];
							add_info_special.add(Integer.parseInt(line_splitted[9]));
							if(Integer.parseInt(line_splitted[9])==2 || Integer.parseInt(line_splitted[9])==3){
								lap_marker_in_file=true;
							}

						}
					} catch (Exception e) {
						e.printStackTrace();
						Log.d(DEBUG_TAG, "Parsing one point failed, but dont worry, we'll go on: " + e.getMessage());
					}
				}
			} catch (Exception e) {
				e.printStackTrace();
				Log.d(DEBUG_TAG, "Error reading Line from File: " + e.getMessage());
			}
			/// CLOSE
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

		mapView.getOverlays().clear();
		mapView.getOverlays().add(nearPicOverlay);
		mapView.getOverlays().add(nearPicPOIOverlay);

		if(p!=null){
			mc.animateTo(p);
		}
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


	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		MenuInflater inflater = getMenuInflater();
		inflater.inflate(R.menu.menu_show_maps, menu);
		mMenuItemConnect = menu.getItem(0);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		switch (item.getItemId()) {
		case R.id.menu_exit:
			finish();
			return true;
		case R.id.menu_save_kml:
			try {
				save_as_kml();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			return true;
		}
		return false;
	}


	private void save_as_kml() throws IOException {
		// TODO Auto-generated method stub
		Integer lap_counter=1;
		String filename_out = filename.substring(0,filename.lastIndexOf('.'))+".kml";
		FileOutputStream out = null;
		out = new FileOutputStream(filename_out,false);
		String output="<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><kml xmlns=\"http://earth.google.com/kml/2.1\"><Document>\r\n";
		out.write(output.getBytes());
		output="<name>Speedoino Route</name><open>true</open><description>Captured by the Speedoino on "+date.substring(0,2)+"."+date.substring(2,4)+".20"+date.substring(4,6)+"</description>";
		out.write(output.getBytes());
		output="<Style id=\"routeStyle\"><LineStyle><color>7FFF0055</color><width>3.0</width></LineStyle></Style>";
		out.write(output.getBytes());
		output="<Style id=\"trackStyle\"><LineStyle><color>FFFF00FF</color><width>3.0</width></LineStyle></Style>";
		out.write(output.getBytes());
		p = gps_points.get(0);
		if(p!=null){
			output="<LookAt id=\"ID\"><longitude>"+String.valueOf(p.getLongitudeE6()/1E6)+"</longitude><latitude>"+String.valueOf(p.getLatitudeE6()/1E6)+"</latitude><altitude>1000</altitude><heading>0</heading><tilt>0</tilt><range></range><altitudeMode>absolute</altitudeMode></LookAt>";
			out.write(output.getBytes());
		};

		if(lap_marker_in_file){
			output="<Folder><name>Lap "+String.valueOf(lap_counter)+"</name>";
		} else {
			output="<Folder><name>GPS Waypoints</name>";
		}
		out.write(output.getBytes());


		for(int i=0;i<gps_points.size();i++){
			p = gps_points.get(i);

			if(add_info_time.get(i).length()>=9){ // neue files mit ms timestamp
				output="\n\r<Placemark><name></name><description>Speedoino autosaved point\nSpeed: "+add_info_speed.get(i)+" km/h\nTime: "+add_info_time.get(i).substring(0, 2)+":"+add_info_time.get(i).substring(2, 4)+":"+add_info_time.get(i).substring(4, 6)+"."+add_info_time.get(i).substring(6, 9)+"</description>";
			} else { // alte files ohne ms
				output="\n\r<Placemark><name></name><description>Speedoino autosaved point\nSpeed: "+add_info_speed.get(i)+" km/h\nTime: "+add_info_time.get(i).substring(0, 2)+":"+add_info_time.get(i).substring(2, 4)+":"+add_info_time.get(i).substring(4, 6)+"</description>";
			}
			out.write(output.getBytes());

			// 0-100 is fade from full red to yellow (red+green)
			// 100-200 is fade from full yellow (red+green) to green

			// red -> 000..100 -> 255
			// red -> 100..200 -> 255..0
			// red -> 200..000 -> 0
			int red=0;
			if(add_info_speed.get(i)<=100){
				red=255;
			} else if(add_info_speed.get(i)>100 && add_info_speed.get(i)<=200){
				red=-((add_info_speed.get(i)-100)*255)/100+255;
			} else {
				red=0;
			}
			String red_str=Integer.toHexString(red);
			while(red_str.length()<2){
				red_str="0"+red_str;
			}

			// green -> 000..100 -> 0
			// green -> 100..200 -> 0..255
			// green -> 200..000 -> 255
			int green=0;
			if(add_info_speed.get(i)<=100){
				green=0;
			} else if(add_info_speed.get(i)>100 && add_info_speed.get(i)<=200){
				green=((add_info_speed.get(i)-100)*255)/100;
			} else {
				green=255;
			}
			String green_str=Integer.toHexString(green);
			while(green_str.length()<2){
				green_str="0"+green_str;
			}

			if(add_info_special.get(i)==2 || add_info_special.get(i)==3) {
				String flag_color="FF00FFFF";	// red flag = Finish lap flag
				String scale="1.5";
				if(add_info_special.get(i)==2){ 
					flag_color="FFFFFFFF"; // yellow flag = sector end flag
					scale="1";
				}
				output="<Style><IconStyle><color>"+flag_color+"</color><scale>"+scale+"</scale><Icon><href>http://www.tischlerei-windeler.de/finish_line.gif</href></Icon></IconStyle></Style>";
			} else {
				output="<Style><IconStyle><color>FF00"+green_str+red_str+"</color><scale>0.18</scale><Icon><href>http://www.tischlerei-windeler.de/punkt.png</href></Icon></IconStyle></Style>";
			}
			out.write(output.getBytes());
			output="<Point><coordinates>"+String.valueOf(p.getLongitudeE6()/1E6)+","+String.valueOf(p.getLatitudeE6()/1E6)+",0</coordinates></Point></Placemark>";
			out.write(output.getBytes());

			if(add_info_special.get(i)==3) { // lap end marker
				lap_counter++;
				output="</Folder><Folder><name>Lap "+String.valueOf(lap_counter)+"</name>";
				out.write(output.getBytes());
			}

		}
		output = "</Folder>";
		out.write(output.getBytes());

		/////////////////////////////// add path ///////////////////////////////
		String formating="<Style><LineStyle><color>#ffff5500</color><width>5</width></LineStyle></Style>";
		lap_counter=1;
		output="<Folder><name>Laps Paths</name>";
		out.write(output.getBytes());
		output = "<Placemark id=\"Track\"><name>Lap "+String.valueOf(lap_counter)+"</name><LineString><coordinates>";
		out.write(output.getBytes());
		for(int i=0;i<gps_points.size();i++){
			p = gps_points.get(i);
			output= String.valueOf(p.getLongitudeE6()/1E6)+","+String.valueOf(p.getLatitudeE6()/1E6)+",0 ";
			out.write(output.getBytes());
			if(add_info_special.get(i)==3) { // lap end marker 
				output="</coordinates></LineString>"+formating+"</Placemark>";
				out.write(output.getBytes());
				lap_counter++;
				output = "<Placemark id=\"Track\"><name>Lap "+String.valueOf(lap_counter)+"</name><LineString><coordinates>";
				out.write(output.getBytes());
			}
		}
		output="</coordinates></LineString>"+formating+"</Placemark></Folder>";
		out.write(output.getBytes());
		/////////////////////////////// add path ///////////////////////////////

		// end document
		output = "</Document></kml>";
		out.write(output.getBytes());
		out.close();

		// show file sharing dialog
		Intent share = new Intent(Intent.ACTION_SEND);
		share.setType("text/plain");
		share.putExtra(Intent.EXTRA_STREAM,Uri.parse("file://"+filename_out));
		startActivity(Intent.createChooser(share, "Send in addition"));
	}

}



