package de.windeler.kolja;

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.widget.ImageView;
import de.windeler.kolja.GifDecoder;

public class GifDecoderView extends ImageView {

	public GifDecoder mGifDecoder;
	private Bitmap mTmpBitmap;
	private int status;

	public GifDecoderView(Context context, InputStream stream, String filename_without_ext, String tempfolder, Handler mHandlerUpdate) throws FileNotFoundException {
		super(context);
		mGifDecoder = new GifDecoder();
		status=mGifDecoder.read(stream);
		if(status==0){
			for (int i = 0; i < mGifDecoder.getFrameCount(); i++) {
				Message msg = mHandlerUpdate.obtainMessage();
				Bundle bundle = new Bundle();
				bundle.putInt("current", i);
				bundle.putInt("total", mGifDecoder.getFrameCount());
				msg.setData(bundle);
				mHandlerUpdate.sendMessage(msg);
								
				mTmpBitmap = mGifDecoder.getFrame(i);
				String filename=tempfolder+filename_without_ext.substring(filename_without_ext.lastIndexOf("/")+1)+"_"+String.valueOf(i)+".png";
				FileOutputStream out = new FileOutputStream(filename);
				mTmpBitmap.compress(Bitmap.CompressFormat.PNG, 96, out);
			}
		}
	}

};

