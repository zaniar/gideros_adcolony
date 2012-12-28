#include "gadcolony.h"
#include "gideros.h"
#include <glog.h>

// some Lua helper functions
#ifndef abs_index
#define abs_index(L, i) ((i) > 0 || (i) <= LUA_REGISTRYINDEX ? (i) : lua_gettop(L) + (i) + 1)
#endif

static void luaL_newweaktable(lua_State *L, const char *mode)
{
	lua_newtable(L);			// create table for instance list
	lua_pushstring(L, mode);
	lua_setfield(L, -2, "__mode");	  // set as weak-value table
	lua_pushvalue(L, -1);             // duplicate table
	lua_setmetatable(L, -2);          // set itself as metatable
}

static void luaL_rawgetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_rawget(L, idx);
}

static void luaL_rawsetptr(lua_State *L, int idx, void *ptr)
{
	idx = abs_index(L, idx);
	lua_pushlightuserdata(L, ptr);
	lua_insert(L, -2);
	lua_rawset(L, idx);
}

static const char *VIDEO_INITIALIZED = "videoInitialized";
static const char *VIDEO_READY = "videoReady";
static const char *VIDEO_NOT_READY = "videoNotReady";
static const char *VIDEO_STARTED = "videoStarted";
static const char *VIDEO_FINISHED = "videoFinished";

static char keyWeak = ' ';

class AdColony : public GEventDispatcherProxy
{
public:
	AdColony(lua_State *L) : L(L)
	{
		gadcolony_init();
		gadcolony_addCallback(callback_s, this);
	}
	
	~AdColony()
	{
		gadcolony_removeCallback(callback_s, this);
		gadcolony_cleanup();
	}
	
	void configure(const char* appId, const char* zoneId)
	{
		gadcolony_configure(appId, zoneId);
	}
	
	bool isConfigured()
	{
		return gadcolony_isConfigured();
	}
	
	void enable(bool enabled)
	{
		gadcolony_enable(enabled);
	}
	
	void showVideo(const char* zoneId)
	{
		gadcolony_showVideo(zoneId);
	}
	
	void offerV4VC(const char* zoneId, bool postPopup)
	{
		gadcolony_offerV4VC(postPopup);
	}
	
	void showV4VC(const char* zoneId, bool postPopup)
	{
		gadcolony_showV4VC(postPopup);
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		static_cast<AdColony*>(udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		dispatchEvent(type, event);
	}

	void dispatchEvent(int type, void *event)
	{
		luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
		luaL_rawgetptr(L, -1, this);

		if (lua_isnil(L, -1))
		{
			lua_pop(L, 2);
			return;
		}

		lua_getfield(L, -1, "dispatchEvent");

		lua_pushvalue(L, -2);

		lua_getglobal(L, "Event");
		lua_getfield(L, -1, "new");
		lua_remove(L, -2);

		switch (type)
		{
		case GADCOLONY_VIDEO_INITIALIZED_EVENT:
			lua_pushstring(L, VIDEO_INITIALIZED);
			break;
		case GADCOLONY_VIDEO_READY_EVENT:
			lua_pushstring(L, VIDEO_READY);
			break;
		case GADCOLONY_VIDEO_NOT_READY_EVENT:
			lua_pushstring(L, VIDEO_NOT_READY);
			break;
		case GADCOLONY_VIDEO_STARTED_EVENT:
			lua_pushstring(L, VIDEO_STARTED);
			break;
		case GADCOLONY_VIDEO_FINISHED_EVENT:
			lua_pushstring(L, VIDEO_FINISHED);
			break;
		}

		lua_call(L, 1, 1);

		lua_call(L, 2, 0);

		lua_pop(L, 2);
	}

private:
	lua_State *L;
	bool initialized_;
};

static int destruct(lua_State* L)
{
	void *ptr =*(void**)lua_touserdata(L, 1);
	GReferenced* object = static_cast<GReferenced*>(ptr);
	AdColony *adcolony = static_cast<AdColony*>(object->proxy());

	adcolony->unref();

	return 0;
}

static AdColony *getInstance(lua_State *L, int index)
{
	GReferenced *object = static_cast<GReferenced*>(g_getInstance(L, "AdColony", index));
	AdColony *adcolony = static_cast<AdColony*>(object->proxy());

	return adcolony;
}

static int configure(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	const char *appId = lua_tostring(L, 2);
	const char *zoneId = lua_tostring(L, 3);
	
	adcolony->configure(appId, zoneId);
	
	return 0;
}

static int isConfigured(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	lua_pushboolean(L, adcolony->isConfigured());
	
	return 0;
}

static int enable(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	bool enabled = lua_toboolean(L, 2);
	
	adcolony->enable(enabled);
	
	return 0;
}

static int showVideo(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	const char *zoneId = lua_tostring(L, 2);
	
	adcolony->showVideo(zoneId);
	
	return 0;
}

static int offerV4VC(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	const char *zoneId = lua_tostring(L, 2);
	bool postPopup = lua_toboolean(L, 3);
	
	adcolony->offerV4VC(zoneId, postPopup);
	
	return 0;
}

static int showV4VC(lua_State *L)
{
	AdColony *adcolony = getInstance(L, 1);
	
	const char *zoneId = lua_tostring(L, 2);
	bool postPopup = lua_toboolean(L, 3);
	
	adcolony->showV4VC(zoneId, postPopup);
	
	return 0;
}

static int loader(lua_State *L)
{
	const luaL_Reg functionList[] = {
		{"configure", configure},
		{"isConfigured", isConfigured},
		{"enable", enable},
		{"showVideo", showVideo},
		{"offerV4VC", offerV4VC},
		{"showV4VC", showV4VC},
		{NULL, NULL}
	};

    g_createClass(L, "AdColony", "EventDispatcher", NULL, destruct, functionList);

    // create a weak table in LUA_REGISTRYINDEX that can be accessed with the address of keyWeak
	luaL_newweaktable(L, "v");
	luaL_rawsetptr(L, LUA_REGISTRYINDEX, &keyWeak);

    lua_getglobal(L, "Event");
	lua_pushstring(L, VIDEO_INITIALIZED);
	lua_setfield(L, -2, "VIDEO_INITIALIZED");
	lua_pushstring(L, VIDEO_READY);
	lua_setfield(L, -2, "VIDEO_READY");
	lua_pushstring(L, VIDEO_NOT_READY);
	lua_setfield(L, -2, "VIDEO_NOT_READY");
	lua_pushstring(L, VIDEO_STARTED);
	lua_setfield(L, -2, "VIDEO_STARTED");
	lua_pushstring(L, VIDEO_FINISHED);
	lua_setfield(L, -2, "VIDEO_FINISHED");
	lua_pop(L, 1);

	AdColony *adcolony = new AdColony(L);
	g_pushInstance(L, "AdColony", adcolony->object());

	luaL_rawgetptr(L, LUA_REGISTRYINDEX, &keyWeak);
	lua_pushvalue(L, -2);
	luaL_rawsetptr(L, -2, adcolony);
	lua_pop(L, 1);

	lua_pushvalue(L, -1);
	lua_setglobal(L, "adcolony");

    return 1;
}

static void g_initializePlugin(lua_State *L)
{
    lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	lua_pushcfunction(L, loader);
	lua_setfield(L, -2, "adcolony");

	lua_pop(L, 2);
}

static void g_deinitializePlugin(lua_State *L)
{

}

REGISTER_PLUGIN("AdColony", "2012.12")
