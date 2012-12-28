package com.nightspade.gideros.android.plugins.adcolony;

import java.lang.ref.WeakReference;

import com.jirbo.adcolony.AdColony;
import com.jirbo.adcolony.AdColonyVideoAd;
import com.jirbo.adcolony.AdColonyVideoListener;

import android.app.Activity;
import android.location.Address;
import android.widget.FrameLayout;

public class GAdColony {
	private static WeakReference<Activity> sActivity;
	private static GAdColony sInstance;
	private static long sData;
	
	private static String sAppId;
	private static String sZoneId;
	
	public GAdColony(Activity activity)
	{
	}
	
	public static void onCreate(Activity activity)
	{
		sActivity = new WeakReference<Activity>(activity);
	}
	
	public static void onPause()
	{
		AdColony.pause();
	}
	
	public static void onResume()
	{
		AdColony.resume(sActivity.get());
	}
	
	static public void onDestroy()
	{
		cleanup();
	}
	
	static public void init(long data)
	{
		sData = data;
		sInstance = new GAdColony(sActivity.get());
	}
	
	static public void cleanup()
	{
		if (sInstance != null)
		{
			sData = 0;
			sInstance = null;
		}
	}
	
	public static void configure(String appId, String zoneId)
	{
		sAppId = appId;
		sZoneId = zoneId;
		
		try
		{
			sActivity.get().runOnUiThread(new Runnable() {
				
				@Override
				public void run() {
					AdColony.configure(sActivity.get(), "1.0", sAppId, sZoneId);
				}
			});
		}
		catch(Exception ex){}
	}
	
	public static boolean isConfigured()
	{
		return AdColony.isConfigured();
	}
	
	public static void enable(boolean enabled)
	{
		AdColony.enable(enabled);
	}
	
	public static void showVideo(String zoneId)
	{
		AdColonyVideoAd ad;
		if(zoneId == null) {
			ad = new AdColonyVideoAd();
		} else {
			ad = new AdColonyVideoAd(zoneId);
		}
		onVideoInitialized(sData);
		if (ad.isReady()) {
			onVideoReady(sData);
			
	        ad.show( new AdColonyVideoListener() {
	
				@Override
				public void onAdColonyVideoFinished() {
					onVideoFinished(sData);
				}
	
				@Override
				public void onAdColonyVideoStarted() {
					onVideoStarted(sData);
				}
	        	
	        });
		} else {
			onVideoNotReady(sData);
		}
	}
	
	public static void offerV4VC(String zoneId, boolean postPopup)
	{
		AdColonyVideoAd ad;
		if(zoneId == null) {
			ad = new AdColonyVideoAd();
		} else {
			ad = new AdColonyVideoAd(zoneId);
		}
		onVideoInitialized(sData);
		if (ad.isReady()) {
			onVideoReady(sData);
			
	        ad.offerV4VC(new AdColonyVideoListener() {
	
				@Override
				public void onAdColonyVideoFinished() {
					onVideoFinished(sData);
				}
	
				@Override
				public void onAdColonyVideoStarted() {
					onVideoStarted(sData);
				}
	        	
	        }, postPopup);
		} else {
			onVideoNotReady(sData);
		}
	}
	
	public static void showV4VC(String zoneId, boolean postPopup)
	{
		AdColonyVideoAd ad;
		if(zoneId == null) {
			ad = new AdColonyVideoAd();
		} else {
			ad = new AdColonyVideoAd(zoneId);
		}
		onVideoInitialized(sData);
		if (ad.isReady()) {
			onVideoReady(sData);
			
	        ad.showV4VC(new AdColonyVideoListener() {
	
				@Override
				public void onAdColonyVideoFinished() {
					onVideoFinished(sData);
				}
	
				@Override
				public void onAdColonyVideoStarted() {
					onVideoStarted(sData);
				}
	        	
	        }, postPopup);
		} else {
			onVideoNotReady(sData);
		}
	}
	
	public static native void onVideoInitialized(long data);
	public static native void onVideoReady(long data);
	public static native void onVideoNotReady(long data);
	public static native void onVideoStarted(long data);
	public static native void onVideoFinished(long data);
}
