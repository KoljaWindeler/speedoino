package de.windeler.kolja;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import android.content.Context;
import android.graphics.Bitmap;
import android.widget.ImageView;
import de.windeler.kolja.GifDecoder;

public class GifDecoderView extends ImageView {

	public GifDecoder mGifDecoder;
	private Bitmap mTmpBitmap;
	private int status;

	public GifDecoderView(Context context, InputStream stream, String filename_without_ext, String tempfolder) throws FileNotFoundException {
		super(context);
		mGifDecoder = new GifDecoder();
		status=mGifDecoder.read(stream);
		if(status==0){
			for (int i = 0; i < mGifDecoder.getFrameCount(); i++) {
				mTmpBitmap = mGifDecoder.getFrame(i);
				String filename=tempfolder+filename_without_ext.substring(filename_without_ext.lastIndexOf("/")+1)+"_"+String.valueOf(i)+".PNG";
				FileOutputStream out = new FileOutputStream(filename);
				mTmpBitmap.compress(Bitmap.CompressFormat.PNG, 96, out);
			}
		}
	}

};

