#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:media_stream"

#include "fl_webrtc_media_stream.h"
#include "fl_webrtc_log.h"
#include "fl_webrtc_context.h"

struct _FlWebrtcMediaStream {
    GObject parent_instance;
    FlWebrtcContext *context;
};

static void fl_webrtc_media_stream_method_iface_init(FlWebrtcMediaStreamMethodInterface *iface);
G_DEFINE_TYPE_WITH_CODE(FlWebrtcMediaStream, fl_webrtc_media_stream, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(FL_TYPE_WEBRTC_MEDIA_STREAM_METHOD,
                        fl_webrtc_media_stream_method_iface_init))

static void get_user_media_impl(FlWebrtcMediaStream* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
  FlValue *args = fl_method_call_get_args(method_call);
  if (args == NULL) {
    fl_method_call_respond_error(method_call,
        "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
    return;
  }

  FlValue *constraints = fl_value_lookup_string(args, "constraints");
  if (fl_value_get_type(constraints) != FL_VALUE_TYPE_MAP) {
    fl_method_call_respond_error(method_call,
        "Bad Arguments", "constraints isn't MAP", nullptr, nullptr);
    return;
  }

  FL_WEBRTC_LOGI("constraints:%s", fl_value_to_string(constraints));
  g_autofree gchar *uuid = g_uuid_string_random();
  fl_method_call_respond_success(method_call, nullptr, nullptr);
  return;
}

static void dispose_impl(FlWebrtcMediaStream* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_sources_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_tracks_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void track_dispose_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void track_set_enable_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void track_switch_camera_impl(FlWebrtcMediaStream *self, FlMethodCall *method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void fl_webrtc_media_stream_method_iface_init(FlWebrtcMediaStreamMethodInterface *iface) {
  FL_WEBRTC_LOGI("Enter");
  iface->get_user_media = get_user_media_impl;
  iface->dispose = dispose_impl;
  iface->get_sources = get_sources_impl;
  iface->get_tracks = get_tracks_impl;
  iface->track_dispose = track_dispose_impl;
  iface->track_set_enable = track_set_enable_impl;
  iface->track_switch_camera = track_switch_camera_impl;
}

static void fl_webrtc_media_stream_dispose(GObject *object) {
  FL_WEBRTC_LOGI("Enter");
  G_OBJECT_CLASS(fl_webrtc_media_stream_parent_class)->dispose(object);
}

static void fl_webrtc_media_stream_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_media_stream_parent_class)->finalize(object);
}

static void fl_webrtc_media_stream_class_init(FlWebrtcMediaStreamClass *klass) {
  FL_WEBRTC_LOGI("Enter");
  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_media_stream_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_media_stream_finalize;
}

static void fl_webrtc_media_stream_init(FlWebrtcMediaStream *self) {
  FL_WEBRTC_LOGI("Enter");
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

FlWebrtcMediaStream *fl_webrtc_media_stream_new(FlWebrtcContext *context) {
  FlWebrtcMediaStream *self = FL_WEBRTC_MEDIA_STREAM(g_object_new(FL_TYPE_WEBRTC_MEDIA_STREAM, NULL));
  self->context = FL_WEBRTC_CONTEXT(context);
  return self;
}
