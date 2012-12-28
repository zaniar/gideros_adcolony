#include "gadcolony.h"
#include <jni.h>
#include <stdlib.h>
#include <glog.h>

extern "C" {
JavaVM *g_getJavaVM();
JNIEnv *g_getJNIEnv();
}

class GAdColony
{
public:
	GAdColony()
	{
		gid_ = g_nextgid();

		JNIEnv *env = g_getJNIEnv();

		jclass localClass = env->FindClass("com/nightspade/gideros/android/plugins/adcolony/GAdColony");
		cls_ = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "init", "(J)V"), (jlong)this);
	}

	~GAdColony()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "cleanup", "()V"));

		env->DeleteGlobalRef(cls_);

		gevent_removeEventsWithGid(gid_);
	}

	void onVideoInitialized()
	{
		gevent_enqueueEvent(gid_, callback_s, GADCOLONY_VIDEO_INITIALIZED_EVENT, NULL, 1, this);
	}

	void onVideoReady()
	{
		gevent_enqueueEvent(gid_, callback_s, GADCOLONY_VIDEO_READY_EVENT, NULL, 1, this);
	}

	void onVideoNotReady()
	{
		gevent_enqueueEvent(gid_, callback_s, GADCOLONY_VIDEO_NOT_READY_EVENT, NULL, 1, this);
	}

	void onVideoStarted()
	{
		gevent_enqueueEvent(gid_, callback_s, GADCOLONY_VIDEO_STARTED_EVENT, NULL, 1, this);
	}

	void onVideoFinished()
	{
		gevent_enqueueEvent(gid_, callback_s, GADCOLONY_VIDEO_FINISHED_EVENT, NULL, 1, this);
	}

	void configure(const char* appId, const char* zoneId)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "configure", "(Ljava/lang/String;Ljava/lang/String;)V"), env->NewStringUTF(appId), env->NewStringUTF(zoneId));
	}

	bool isConfigured()
	{
		JNIEnv *env = g_getJNIEnv();

		jboolean jstate = (jboolean) env->CallStaticBooleanMethod(cls_, env->GetStaticMethodID(cls_, "isConfigured", "()Z"));

		return jstate;
	}

	void enable(bool enabled)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "enable", "(Z)V"), enabled);
	}
	
	void showVideo(const char* zoneId)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "showVideo", "(Ljava/lang/String;)V"), env->NewStringUTF(zoneId));
	}
	
	void offerV4VC(const char* zoneId, bool postPopup)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "offerV4VC", "(Ljava/lang/String;Z)V"), env->NewStringUTF(zoneId), postPopup);
	}
	
	void showV4VC(const char* zoneId, bool postPopup)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "showV4VC", "(Ljava/lang/String;Z)V"), env->NewStringUTF(zoneId), postPopup);
	}

	g_id addCallback(gevent_Callback callback, void *udata)
		{
			return callbackList_.addCallback(callback, udata);
		}

		void removeCallback(gevent_Callback callback, void *udata)
		{
			callbackList_.removeCallback(callback, udata);
		}

		void removeCallbackWithGid(g_id gid)
		{
			callbackList_.removeCallbackWithGid(gid);
		}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		((GAdColony*)udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		callbackList_.dispatchEvent(type, event);
	}

private:
	gevent_CallbackList callbackList_;

private:
	jclass cls_;
	g_id gid_;
};


extern "C" {
	void Java_com_nightspade_gideros_android_plugins_adcolony_GAdColony_onVideoInitialized(JNIEnv *env, jclass clz, jlong data)
	{
		((GAdColony*)data)->onVideoInitialized();
	}

	void Java_com_nightspade_gideros_android_plugins_adcolony_GAdColony_onVideoReady(JNIEnv *env, jclass clz, jlong data)
	{
		((GAdColony*)data)->onVideoReady();
	}

	void Java_com_nightspade_gideros_android_plugins_adcolony_GAdColony_onVideoNotReady(JNIEnv *env, jclass clz, jlong data)
	{
		((GAdColony*)data)->onVideoNotReady();
	}

	void Java_com_nightspade_gideros_android_plugins_adcolony_GAdColony_onVideoStarted(JNIEnv *env, jclass clz, jlong data)
	{
		((GAdColony*)data)->onVideoStarted();
	}

	void Java_com_nightspade_gideros_android_plugins_adcolony_GAdColony_onVideoFinished(JNIEnv *env, jclass clz, jlong data)
	{
		((GAdColony*)data)->onVideoFinished();
	}
}

static GAdColony *s_adcolony = NULL;

extern "C" {

int gadcolony_isAvailable()
{
	return 1;
}

void gadcolony_init()
{
	s_adcolony = new GAdColony();
}

void gadcolony_cleanup()
{
	delete s_adcolony;
	s_adcolony = NULL;
}

void gadcolony_configure(const char* appId, const char* zoneId)
{
	s_adcolony->configure(appId, zoneId);
}

bool gadcolony_isConfigured()
{
	return s_adcolony->isConfigured();
}

void gadcolony_enable(bool enabled)
{
	s_adcolony->enable(enabled);
}

void gadcolony_showVideo(const char* zoneId)
{
	s_adcolony->showVideo(zoneId);
}

void gadcolony_offerV4VC(const char* zoneId, bool postPopup)
{
	s_adcolony->offerV4VC(zoneId, postPopup);
}

void gadcolony_showV4VC(const char* zoneId, bool postPopup)
{
	s_adcolony->showV4VC(zoneId, postPopup);
}

g_id gadcolony_addCallback(gevent_Callback callback, void *udata)
{
	return s_adcolony->addCallback(callback, udata);
}

void gadcolony_removeCallback(gevent_Callback callback, void *udata)
{
	s_adcolony->removeCallback(callback, udata);
}

void gadcolony_removeCallbackWithGid(g_id gid)
{
	s_adcolony->removeCallbackWithGid(gid);
}

}
