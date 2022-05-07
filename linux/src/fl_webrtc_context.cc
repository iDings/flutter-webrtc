#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:context"

#include "fl_webrtc_context.h"
#include "fl_webrtc_log.h"

#include "fl_webrtc_peer_connection.h"
#include "fl_webrtc_media_stream.h"
#include "fl_webrtc_data_channel.h"
#include "fl_webrtc_video_renderer.h"

#include "libwebrtc.h"
#include "rtc_audio_device.h"
#include "rtc_media_stream.h"
#include "rtc_media_track.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_device.h"

using namespace libwebrtc;

// TODO: libwebrtc objects lifecycle
// use c++ class
struct _FlWebrtcContext {
  GObject parent_instance;
  FlBinaryMessenger* messenger;
  FlTextureRegistrar* texture_registrar;

  FlWebrtcPeerConnection *peer_connection;
  FlWebrtcDataChannel *data_channel;
  FlWebrtcMediaStream *media_stream;
  FlWebrtcVideoRenderer *video_renderer;

  // c contextu, use raw pointer
  RTCPeerConnectionFactory *factory;
  // !in factory
  RTCAudioDevice *audio_device;
  // !in factory
  RTCVideoDevice *video_device;
};

G_DEFINE_TYPE(FlWebrtcContext, fl_webrtc_context, g_object_get_type())

static void fl_webrtc_context_dispose(GObject* object) {
  FL_WEBRTC_TRACE("Enter");
  FlWebrtcContext *self = FL_WEBRTC_CONTEXT(object);

  g_clear_object(&self->peer_connection);
  g_clear_object(&self->data_channel);
  g_clear_object(&self->media_stream);
  g_clear_object(&self->video_renderer);

  G_OBJECT_CLASS(fl_webrtc_context_parent_class)->dispose(object);
}

static void fl_webrtc_context_finalize(GObject *object) {
  FL_WEBRTC_TRACE("Enter");
  FlWebrtcContext *self = FL_WEBRTC_CONTEXT(object);
  self->factory->Release();
  LibWebRTC::Terminate();

  G_OBJECT_CLASS(fl_webrtc_context_parent_class)->finalize(object);
}

static void fl_webrtc_context_class_init(FlWebrtcContextClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_context_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_context_finalize;
}

static void fl_webrtc_context_init(FlWebrtcContext* self) {
  FL_WEBRTC_LOGI("Enter");
  return;
}

FlWebrtcContext *fl_webrtc_context_new(FlBinaryMessenger *message, FlTextureRegistrar *texture) {
  FlWebrtcContext *self = FL_WEBRTC_CONTEXT(g_object_new(fl_webrtc_context_get_type(), NULL));
  self->messenger = message;
  self->texture_registrar = texture;

  self->peer_connection = fl_webrtc_peer_connection_new(self);
  self->data_channel = fl_webrtc_data_channel_new(self);
  self->media_stream = fl_webrtc_media_stream_new(self);
  self->video_renderer = fl_webrtc_video_renderer_new(self);

  LibWebRTC::Initialize();
  self->factory = LibWebRTC::CreateRTCPeerConnectionFactory().release();
  self->audio_device = self->factory->GetAudioDevice().get();
  self->video_device = self->factory->GetVideoDevice().get();
  return self;
}

gboolean fl_webrtc_context_invoke_method(FlWebrtcContext *self, FlMethodCall *method_call) {
  const gchar* method = fl_method_call_get_name(method_call);
  guint signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_PEER_CONNECTION);
  if (signal_id) {
    g_signal_emit_by_name(self->peer_connection, method, method_call);
    return TRUE;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_DATA_CHANNEL);
  if (signal_id) {
    g_signal_emit_by_name(self->data_channel, method, method_call);
    return TRUE;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_MEDIA_STREAM);
  if (signal_id) {
    g_signal_emit_by_name(self->media_stream, method, method_call);
    return TRUE;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_VIDEO_RENDERER);
  if (signal_id) {
    g_signal_emit_by_name(self->video_renderer, method, method_call);
    return TRUE;
  }

  return FALSE;
}

FlBinaryMessenger *fl_webrtc_context_messenger(FlWebrtcContext *self) {
  return self->messenger;
}

FlTextureRegistrar *fl_webrtc_context_texture_registrar(FlWebrtcContext *self) {
  return self->texture_registrar;
}

RTCPeerConnectionFactory *fl_webrtc_context_peerconnection_factory(FlWebrtcContext *self) {
  return self->factory;
}
