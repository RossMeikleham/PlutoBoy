package org.plutoboy.plutoboy;

import org.libsdl.app.SDLActivity;

import android.content.Context;
import android.os.Bundle;
import android.os.Vibrator;

public class PlutoboyActivity extends SDLActivity {
	private Vibrator v;
	private boolean vibratorActive;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) { 
	        super.onCreate(savedInstanceState); 
	        v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
	        vibratorActive = v.hasVibrator();
	} 
	    
	@Override
	protected void onDestroy() { 
	        super.onDestroy(); 
	} 
	
	
	
	void vibrate() {
		//Vibrate for 25 milliseconds
		if (vibratorActive) {
			v.vibrate(25);
		}
	}
}
