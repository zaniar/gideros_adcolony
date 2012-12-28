#ifndef GADCOLONY_H
#define GADCOLONY_H

#include <gglobal.h>
#include <gevent.h>

enum
{
	GADCOLONY_VIDEO_INITIALIZED_EVENT,
	GADCOLONY_VIDEO_READY_EVENT,
	GADCOLONY_VIDEO_NOT_READY_EVENT,
	GADCOLONY_VIDEO_STARTED_EVENT,
	GADCOLONY_VIDEO_FINISHED_EVENT,
};

#ifdef __cplusplus
extern "C" {
#endif

G_API int gadcolony_isAvailable();

G_API void gadcolony_init();
G_API void gadcolony_cleanup();

G_API void gadcolony_configure(const char* appId, const char* zoneId);
G_API bool gadcolony_isConfigured();
G_API void gadcolony_enable(bool enabled);

G_API void gadcolony_showVideo(const char* zoneId);
G_API void gadcolony_offerV4VC(const char* zoneId, bool postPopup);
G_API void gadcolony_showV4VC(const char* zoneId, bool postPopup);

G_API g_id gadcolony_addCallback(gevent_Callback callback, void *udata);
G_API void gadcolony_removeCallback(gevent_Callback callback, void *udata);
G_API void gadcolony_removeCallbackWithGid(g_id gid);

#ifdef __cplusplus
}
#endif

#endif
