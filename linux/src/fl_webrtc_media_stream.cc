#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:media_stream"

#include "fl_webrtc_media_stream.h"

struct _FlWebrtcMediaStream {
    GObject parent_instance;
};

static void fl_webrtc_media_stream_method_iface_init(FlWebrtcMediaStreamMethodInterface *iface);
G_DEFINE_TYPE_WITH_CODE(FlWebrtcMediaStream, fl_webrtc_media_stream, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(FL_TYPE_WEBRTC_MEDIA_STREAM_METHOD,
                        fl_webrtc_media_stream_method_iface_init))

static void get_user_media_impl(FlWebrtcMediaStream* self, FlMethodCall* method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void dispose_impl(FlWebrtcMediaStream* self, FlMethodCall* method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void get_sources_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void get_tracks_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void track_dispose_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void track_set_enable_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void track_switch_camera_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void fl_webrtc_media_stream_method_iface_init(FlWebrtcMediaStreamMethodInterface *iface) {
    g_message("%s:%d", __func__, __LINE__);
    iface->get_user_media = get_user_media_impl;
    iface->dispose = dispose_impl;
    iface->get_sources = get_sources_impl;
    iface->get_tracks = get_tracks_impl;
    iface->track_dispose = track_dispose_impl;
    iface->track_set_enable = track_set_enable_impl;
    iface->track_switch_camera = track_switch_camera_impl;
}

static void fl_webrtc_media_stream_dispose(GObject *object) {
    g_message("%s:%d", __func__, __LINE__);
    G_OBJECT_CLASS(fl_webrtc_media_stream_parent_class)->dispose(object);
}

static void fl_webrtc_media_stream_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_media_stream_parent_class)->finalize(object);
}

static void fl_webrtc_media_stream_class_init(FlWebrtcMediaStreamClass *klass) {
    g_message("%s:%d", __func__, __LINE__);
    G_OBJECT_CLASS(klass)->dispose = fl_webrtc_media_stream_dispose;
    G_OBJECT_CLASS(klass)->finalize = fl_webrtc_media_stream_finalize;
}

static void fl_webrtc_media_stream_init(FlWebrtcMediaStream *self) {
    g_message("%s:%d", __func__, __LINE__);
}

G_DEFINE_INTERFACE(FlWebrtcMediaStreamMethod, fl_webrtc_media_stream_method, G_TYPE_OBJECT)
static void fl_webrtc_media_stream_method_default_init(FlWebrtcMediaStreamMethodInterface *iface) {
    g_signal_new("getUserMedia", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, get_user_media),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("getSources", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, get_sources),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("streamDispose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, dispose),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("mediaStreamTrackSetEnable", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, track_set_enable),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("mediaStreamTrackSwitchCamera", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, track_switch_camera),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("trackDispose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcMediaStreamMethodInterface, track_dispose),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);
}
