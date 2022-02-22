#ifndef __FL_WEBRTC_METHOD_CALL_INTERFACE_H__
#define __FL_WEBRTC_METHOD_CALL_INTERFACE_H__

#include <flutter_linux/flutter_linux.h>

#include <glib-2.0/glib.h>
#include <glib-2.0/glib-object.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_METHOD_CALL fl_webrtc_method_call_get_type()
G_DECLARE_INTERFACE(FlWebrtcMethodCall,
                    fl_webrtc_method_call,
                    FL,
                    WEBRTC_METHOD_CALL,
                    GObject);

struct _FlWebrtcMethodCallInterface {
    GTypeInterface g_iface;

    void (*create_peer_connection)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*get_user_media)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*get_display_media)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*get_sources)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*media_stream_get_tracks)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*create_offer)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*create_answer)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*add_stream)(FlutterWebrtcPlugin* self, FlMethodCall* method_call);
    void (*remove_stream)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*set_local_description)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*set_remote_description)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*add_candidate)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*get_stats)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*create_data_channel)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*data_channel_send)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*data_channel_close)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*stream_dispose)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*media_stream_track_set_enable)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*track_dispose)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*restart_ice)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*peer_connection_close)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*peer_connection_dispose)(flutterwebrtcplugin* self, flmethodcall* method_call);

    void (*create_video_renderer)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*video_renderer_dispose)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*videoRendererSetSrcObject)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*mediaStreamTrackSwitchCamera)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*setVolume)(flutterwebrtcplugin* self, flmethodcall* method_call);
    void (*getLocalDescription)(flutterwebrtcplugin* self, flmethodcall* method_call);
};

G_END_DECLS

#endif
