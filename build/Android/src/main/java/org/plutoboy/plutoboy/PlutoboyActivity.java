package org.plutoboy.plutoboy;

import java.io.File;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;
import java.io.IOException;
import java.io.InputStream;
import java.io.ByteArrayOutputStream;

import org.libsdl.app.SDLActivity;
import org.plutoboy.plutoboy.SimpleFileChooser.FileSelectedListener;

import android.content.Context;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.os.Vibrator;
import android.util.Log;
import android.content.pm.PackageManager;

//import android.support.v4.content.ContextCompat;
//import android.support.v4.app.ActivityCompat;
class BoolHolder {
	public boolean value;
}


public class PlutoboyActivity extends SDLActivity {
	private Vibrator v;
	private boolean vibratorActive;
	private Handler fileDialogHandler;
	private final Lock lock = new ReentrantLock();
	private final Condition fileSelected = lock.newCondition();
	private boolean fileSelectedB;
	private String fileSelectedPath;
	private SimpleFileChooser fc;
	private BoolHolder isReady;

	@Override
	protected void onCreate(Bundle savedInstanceState) { 
	        super.onCreate(savedInstanceState); 

			isReady = new BoolHolder();
			isReady.value = false;	

			 String[] perms = {android.Manifest.permission.WRITE_EXTERNAL_STORAGE, "android.permission.VIBRATE"};
	        
			int permsRequestCode = 200; 
			requestPermissions(perms, permsRequestCode);
		/*
     	// Here, thisActivity is the current activity
    	if (ContextCompat.checkSelfPermission(self,
            Manifest.permission.WRITE_EXTERNAL_STORAGE)
            != PackageManager.PERMISSION_GRANTED) {

        // Permission is not granted
        // Should we show an explanation?
        if (ActivityCompat.shouldShowRequestPermissionRationale(thisActivity,
                Manifest.permission.WRITE_EXTERNAL_STORAGE)) {
            // Show an explanation to the user *asynchronously* -- don't block
            // this thread waiting for the user's response! After the user
            // sees the explanation, try again to request the permission.
        } else {
            // No explanation needed; request the permission
            ActivityCompat.requestPermissions(thisActivity,
                    new String[]{Manifest.permission.WRITE_EXTERNAL_STORAGE},
                    MY_PERMISSIONS_WRITE_EXTERNAL_STORAGE);

            // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
            // app-defined int constant. The callback method gets the
            // result of the request.
        }
		}*/



			v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
	        vibratorActive = v.hasVibrator();
	} 
	    
	@Override
	protected void onDestroy() { 
	        super.onDestroy(); 
	} 
	

	public void waitUntilReady() {
		synchronized (isReady) {
			if (isReady.value == false) {
				try {
					Log.i("Plutoboy", "Waiting...\n");
					isReady.wait();
				}
				catch (InterruptedException e) {
					e.printStackTrace();	
				}
			}
		}	
		Log.i("Plutoboy", "Ready\n");
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


    public String[] getFilesForPath(String path) {
        Log.i("Plutoboy", "Getting files for path: " + path);
	
		Log.i("Plutoboy", "Waiting til ready");
		waitUntilReady();
		Log.i("Plutoboy", "Ready");
        
        try {
        String[] listAssets = getAssets().list("");
        Log.i("Plutoboy", String.valueOf(listAssets.length));
        for(String file : listAssets)
        {
            Log.i("Plutoboy", file);
        }
        } catch (IOException io)
        {
            Log.i("Plutoboy", "IOException");
        }

        String[] result = fc.fileListForPath(path);
        Log.i("Plutoboy", "Result: " + Boolean.toString(result == null));
        return result;
    }

    public byte[] assetData(String path) {
        ByteArrayOutputStream content = new ByteArrayOutputStream();
        
        try {
            InputStream ais = getAssets().open(path);
        
            int nRead;
            byte[] data = new byte[16384];

            while ((nRead = ais.read(data, 0, data.length)) != -1) {
                content.write(data, 0, nRead);
            }

            Log.e("Plutoboy", content.toByteArray().toString());
            return content.toByteArray();
        }
        catch (IOException e) {
            Log.e("Plutoboy", "Failed to load asset " + path + ": " + e.toString());
            return null;
        }
    }

    public boolean isDir(String path) {
       return fc.isDir(path); 
    }

    public String getPath() {
        return fc.getPath();
    }



	@Override
	public void onRequestPermissionsResult(int permsRequestCode, String[] permissions, int[] grantResults){

    	switch(permsRequestCode){

        	case 200:

            boolean writeExternalAccepted = grantResults[0]==PackageManager.PERMISSION_GRANTED;
            boolean vibrateAccepted = grantResults[1]==PackageManager.PERMISSION_GRANTED;
			
			Log.i("Plutoboy", "permissionResult:\n");
			Log.i("Plutoboy", Boolean.toString(writeExternalAccepted));
			Log.i("Plutoboy", Boolean.toString(vibrateAccepted));
	        
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
			/*
			fileDialogHandler = new Handler() {
				public void handleMessage(Message msg) {
					fc = new SimpleFileChooser(pba, 
							Environment.getExternalStorageDirectory(), fl);
					fc.showDialog();
				}
			};
    */		    
            fc = new SimpleFileChooser(pba,  Environment.getExternalStorageDirectory(), fl);
		
			synchronized (isReady) {
				isReady.value = true;
				isReady.notify();
			}

            break;
    	}
	}
}

