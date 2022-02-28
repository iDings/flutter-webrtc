#ifndef __FL_WEBRTC_MEDIA_STREAM_H__
#define __FL_WEBRTC_MEDIA_STREAM_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_MEDIA_STREAM fl_webrtc_media_stream_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcMediaStream, fl_webrtc_media_stream, FL, WEBRTC_MEDIA_STREAM, GObject)

#define FL_TYPE_WEBRTC_MEDIA_STREAM_METHOD fl_webrtc_media_stream_method_get_type()
G_DECLARE_INTERFACE(FlWebrtcMediaStreamMethod,
                    fl_webrtc_media_stream_method,
                    FL,
                    WEBRTC_MEDIA_STREAM_METHOD,
                    GObject)

struct _FlWebrtcMediaStreamMethodInterface {
    GTypeInterface g_iface;

    void (*get_user_media)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*get_sources)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*get_tracks)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*dispose)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*track_set_enable)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*track_switch_camera)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
    void (*track_dispose)(FlWebrtcMediaStream* self, FlMethodCall* method_call);
};

G_END_DECLS

#endif
