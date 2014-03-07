package com.jkw.smartspeedo;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.TreeMap;

import android.app.AlertDialog;
import android.app.ListActivity;
import android.content.DialogInterface;
import android.os.Bundle;
import android.view.KeyEvent;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.TextView;

public class FileDialog extends ListActivity {

	private static final String ITEM_KEY = "key";
	private static final String ITEM_KEY_LOW = "key_low";
	private static final String ITEM_IMAGE = "image";
	private static final String ROOT = "/";

	public static final String START_PATH = "START_PATH";
	public static final String EXT_FILTER = "";
	public static final String RESULT_PATH = "RESULT_PATH";
	public static final String RESULT = "ASD";
	public static final String SELECTION_MODE = "SELECTION_MODE";

	private List<String> path = null;
	private TextView myPath;
	private TextView myFilter;
	private ArrayList<HashMap<String, Object>> mList;

	private Button selectButton;

	private LinearLayout layoutSelect;
	private InputMethodManager inputManager;
	private String parentPath;
	private String extensionFilter;
	private String currentPath = ROOT;

	private File selectedFile;
	private HashMap<String, Integer> lastPositions = new HashMap<String, Integer>();

	/** Called when the activity is first created. */
	@Override
	public void onCreate(Bundle savedInstanceState) { 
		super.onCreate(savedInstanceState);
		setResult(RESULT_CANCELED, getIntent());

		setContentView(R.layout.file_dialog_main);
		myPath = (TextView) findViewById(R.id.path);
		myFilter = (TextView) findViewById(R.id.filter);

		inputManager = (InputMethodManager) getSystemService(INPUT_METHOD_SERVICE);

		selectButton = (Button) findViewById(R.id.fdButtonSelect);
		selectButton.setEnabled(false);
		selectButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				if (selectedFile != null) {
					getIntent().putExtra(RESULT_PATH, selectedFile.getPath());
					setResult(RESULT_OK, getIntent());
					finish();
				}
			}
		});


		layoutSelect = (LinearLayout) findViewById(R.id.fdLinearLayoutSelect);

		final Button cancelButton = (Button) findViewById(R.id.fdButtonCancel);
		cancelButton.setOnClickListener(new OnClickListener() {

			@Override
			public void onClick(View v) {
				setResult(RESULT_CANCELED, getIntent());
				finish();
			}
		}
				);
		
		String startPath = getIntent().getStringExtra(START_PATH);
		extensionFilter = getIntent().getStringExtra(EXT_FILTER);
		if (startPath != null) {
			getDir(startPath);
		} else {
			getDir(ROOT);
		}
	}

	private void getDir(String dirPath) {

		boolean useAutoSelection = dirPath.length() < currentPath.length();

		Integer position = lastPositions.get(parentPath);

		getDirImpl(dirPath);

		if (position != null && useAutoSelection) {
			getListView().setSelection(position);
		}

	}

	private void getDirImpl(final String dirPath) {

		currentPath = dirPath;

		final List<String> item = new ArrayList<String>();
		path = new ArrayList<String>();
		mList = new ArrayList<HashMap<String, Object>>();

		File f = new File(currentPath);
		File[] files = f.listFiles();
		if (files == null) {
			currentPath = ROOT;
			f = new File(currentPath);
			files = f.listFiles();
		}
		myPath.setText(getText(R.string.location) + ": " + currentPath);
		
		if(extensionFilter.length()>0){
			myFilter.setText(getText(R.string.filter) + ": showing only " + extensionFilter + "-files");
		} else {
			myFilter.setText(getText(R.string.filter) + ": showing all files");
		}

		if (!currentPath.equals(ROOT)) {

			item.add(ROOT);
			addItem(ROOT, R.drawable.folder);
			path.add(ROOT);

			item.add("../");
			addItem("../", R.drawable.folder);
			path.add(f.getParent());
			parentPath = f.getParent();

		}

		TreeMap<String, String> dirsMap = new TreeMap<String, String>();
		TreeMap<String, String> dirsPathMap = new TreeMap<String, String>();
		TreeMap<String, String> filesMap = new TreeMap<String, String>();
		TreeMap<String, String> filesPathMap = new TreeMap<String, String>();
		for (File file : files) {
			if(!file.getName().toString().substring(0,1).equals(".")){
				if (file.isDirectory()) {
					String dirName = file.getName();
					dirsMap.put(dirName, dirName);
					dirsPathMap.put(dirName, file.getPath());
				} else {
					if(extensionFilter.length()>0){
						String this_extension=file.toString().substring(file.toString().lastIndexOf('.')+1).toLowerCase();
						if(!this_extension.equals(extensionFilter.toLowerCase())){
							continue;
						}
					};
					
					filesMap.put(file.getName(), file.getName());
					filesPathMap.put(file.getName(), file.getPath());
				}
			}
		}
		item.addAll(dirsMap.tailMap("").values());
		item.addAll(filesMap.tailMap("").values());
		path.addAll(dirsPathMap.tailMap("").values());
		path.addAll(filesPathMap.tailMap("").values());

		SimpleAdapter fileList = new SimpleAdapter(this, mList,
				R.layout.file_dialog_row,
				new String[] { ITEM_KEY, ITEM_IMAGE, ITEM_KEY_LOW }, new int[] {
				R.id.fdrowtext, R.id.fdrowimage, R.id.fdrowtext_lower });

		for (String dir : dirsMap.tailMap("").values()) {
			if(dir.toString().length()>23){
				addItem(dir.toString().substring(0, 20)+"...", R.drawable.folder);
			} else {
				addItem(dir, R.drawable.folder);
			}
		}

		for (String file : filesMap.tailMap("").values()) {
			// hier if(filter) einbuane, TODO
			if(extensionFilter.length()>0){
				String this_extension=file.toString().substring(file.toString().lastIndexOf('.')+1).toLowerCase();
				if(!this_extension.equals(extensionFilter.toLowerCase())){
					continue;
				}
			};
			
			if(file.toString().length()>23){
				addItem(file.toString().substring(0, 20)+"...", R.drawable.file);
			} else {
				addItem(file, R.drawable.file);
			}
		}

		fileList.notifyDataSetChanged();

		setListAdapter(fileList);

	}

	private void addItem(String fileName, int imageId) {
		HashMap<String, Object> item = new HashMap<String, Object>();
		item.put(ITEM_KEY, fileName);
		item.put(ITEM_IMAGE, imageId);
		String extension = fileName.substring(fileName.lastIndexOf('.')+1).toLowerCase();
		if(imageId==R.drawable.folder){
			item.put(ITEM_KEY_LOW,"Folder");
		} else if(extension.equals("smf")){
			item.put(ITEM_KEY_LOW,"Speedoino MAP File");
		} else if(extension.toLowerCase().equals("gps")){
			item.put(ITEM_KEY_LOW,"Downloaded GPS Log");
		} else if(extension.toLowerCase().equals("hex")){
			item.put(ITEM_KEY_LOW,"Speedoino Firmware File");
		} else if(extension.equals("sgf")){
			item.put(ITEM_KEY_LOW,"Speedoino GFX File");
		} else if(extension.equals("scf")){
			item.put(ITEM_KEY_LOW,"Speedoino CONFIG File");
		} else if(extension.equals("kml")){
			item.put(ITEM_KEY_LOW,"Google map file");
		} else if(extension.equals("jpg") || extension.equals("jpeg") || extension.equals("gif") || extension.equals("png") || extension.equals("bmp")){
			item.put(ITEM_KEY_LOW,"Convertable image file");
		} else {
			item.put(ITEM_KEY_LOW,"");
		}
		mList.add(item);
	}

	@Override
	protected void onListItemClick(ListView l, View v, int position, long id) {

		File file = new File(path.get(position));
		setSelectVisible(v);

		if (file.isDirectory()) {
			selectButton.setEnabled(false);
			if (file.canRead()) {
				lastPositions.put(currentPath, position);
				getDir(path.get(position));
			} else {
				new AlertDialog.Builder(this)
				.setIcon(R.drawable.icon)
				.setTitle(
						"[" + file.getName() + "] "
								+ getText(R.string.cant_read_folder))
								.setPositiveButton("OK",
										new DialogInterface.OnClickListener() {

									@Override
									public void onClick(DialogInterface dialog,
											int which) {

									}
								}).show();
			}
		} else {
			selectedFile = file;
			v.setSelected(true);
			selectButton.setEnabled(true);
		}
	}

	@Override
	public boolean onKeyDown(int keyCode, KeyEvent event) {
		if ((keyCode == KeyEvent.KEYCODE_BACK)) {
			selectButton.setEnabled(false);
			if (!currentPath.equals(ROOT)) {
				getDir(parentPath);
			} else {
				return super.onKeyDown(keyCode, event);
			}
			return true;
		} else {
			return super.onKeyDown(keyCode, event);
		}
	}


	private void setSelectVisible(View v) {
		layoutSelect.setVisibility(View.VISIBLE);

		inputManager.hideSoftInputFromWindow(v.getWindowToken(), 0);
		selectButton.setEnabled(false);
	}
}
