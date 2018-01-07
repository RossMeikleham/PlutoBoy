package org.plutoboy.plutoboy;

import java.io.File;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

import org.libsdl.app.SDLActivity;
import org.plutoboy.plutoboy.SimpleFileChooser.FileSelectedListener;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;

public class PlutoboyActivity extends SDLActivity {
	private Vibrator v;
	private boolean vibratorActive;
	private Handler fileDialogHandler;
	private final Lock lock = new ReentrantLock();
	private final Condition fileSelected = lock.newCondition();
	private boolean fileSelectedB;
	private String fileSelectedPath;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) { 
	        super.onCreate(savedInstanceState); 
	        v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
	        vibratorActive = v.hasVibrator();
	        
	        // Setting up File Selection handlers and callbacks
	        final PlutoboyActivity pba = this;
	    	final FileSelectedListener fl = new FileSelectedListener() {
				public void onFileSelected(File file) {
					lock.lock();
					try {
						fileSelectedPath = file.getAbsolutePath();
						fileSelectedB = true;
						fileSelected.signal();
					} finally {
						lock.unlock();
					}
				}
			};
			
			fileDialogHandler = new Handler() {
				public void handleMessage(Message msg) {
					SimpleFileChooser fc = new SimpleFileChooser(pba, 
							Environment.getExternalStorageDirectory(), fl);
					fc.showDialog();
				}
			};
	} 
	    
	@Override
	protected void onDestroy() { 
	        super.onDestroy(); 
	} 
	
	
	public void vibrate() {
		
		//Vibrate for 25 milliseconds
		if (vibratorActive) {
			v.vibrate(25);
		}		
	}


	/* Opens a file dialog and waits until the user selects a file. Returns
	 * the absolute path to the selected file.
	 * MUST be called from a background thread (e.g. called directly from C JNI code) */
	public String getFile() {
		fileSelectedB = false;
		fileDialogHandler.sendMessage(new Message());
		
		lock.lock();
		try {
			while (!fileSelectedB) 
				fileSelected.await();
			
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} finally {
			lock.unlock();
		}
		
		return fileSelectedPath;
	}
}

