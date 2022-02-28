#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc"

#include "flutter_webrtc/flutter_web_r_t_c_plugin.h"
#include "fl_webrtc_peer_connection.h"
#include "fl_webrtc_media_stream.h"
#include "fl_webrtc_data_channel.h"
#include "fl_webrtc_video_renderer.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

#define FLUTTER_WEBRTC_PLUGIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), flutter_webrtc_plugin_get_type(), \
                              FlutterWebrtcPlugin))

struct _FlutterWebrtcPlugin {
  GObject parent_instance;

  FlWebrtcPeerConnection *peer_connection;
  FlWebrtcDataChannel *data_channel;
  FlWebrtcMediaStream *media_stream;
  FlWebrtcVideoRenderer *video_renderer;

  FlBinaryMessenger* messenger;
  FlTextureRegistrar* texture_registrar;
};

G_DEFINE_TYPE(FlutterWebrtcPlugin, flutter_webrtc_plugin, g_object_get_type())

const char *kChannelName = "FlutterWebRTC.Method";

static void flutter_webrtc_plugin_dispose(GObject* object) {
  G_OBJECT_CLASS(flutter_webrtc_plugin_parent_class)->dispose(object);
}

static void flutter_webrtc_plugin_finalize(GObject *object) {
    FlutterWebrtcPlugin *plugin = FLUTTER_WEBRTC_PLUGIN(object);

    g_clear_object(&plugin->peer_connection);
    g_clear_object(&plugin->data_channel);
    g_clear_object(&plugin->media_stream);
    g_clear_object(&plugin->video_renderer);

    G_OBJECT_CLASS(flutter_webrtc_plugin_parent_class)->finalize(object);
}

static void flutter_webrtc_plugin_class_init(FlutterWebrtcPluginClass* klass) {
  G_OBJECT_CLASS(klass)->dispose = flutter_webrtc_plugin_dispose;
  G_OBJECT_CLASS(klass)->finalize = flutter_webrtc_plugin_finalize;
}

static void flutter_webrtc_plugin_init(FlutterWebrtcPlugin* self) {
    self->peer_connection = FL_WEBRTC_PEER_CONNECTION(g_object_new(FL_TYPE_WEBRTC_PEER_CONNECTION, NULL));
    self->data_channel = FL_WEBRTC_DATA_CHANNEL(g_object_new(FL_TYPE_WEBRTC_DATA_CHANNEL, NULL));
    self->media_stream = FL_WEBRTC_MEDIA_STREAM(g_object_new(FL_TYPE_WEBRTC_MEDIA_STREAM, NULL));
    self->video_renderer = FL_WEBRTC_VIDEO_RENDERER(g_object_new(FL_TYPE_WEBRTC_VIDEO_RENDERER, NULL));
}

static FlutterWebrtcPlugin *flutter_webrtc_plugin_new(FlBinaryMessenger *message, FlTextureRegistrar *texture) {
    FlutterWebrtcPlugin *plugin = FLUTTER_WEBRTC_PLUGIN(g_object_new(flutter_webrtc_plugin_get_type(), NULL));
    plugin->messenger = message;
    plugin->texture_registrar = texture;
    return plugin;
}

// Called when a method call is received from Flutter.
static void flutter_webrtc_plugin_handle_method_call(FlutterWebrtcPlugin* self,
                                                     FlMethodCall* method_call) {
  g_autoptr(FlMethodResponse) response = nullptr;

  const gchar* method = fl_method_call_get_name(method_call);
  g_message("invoking method: %s", method);

  guint signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_PEER_CONNECTION);
  if (signal_id) {
    g_signal_emit_by_name(self->peer_connection, method, method_call);
    return;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_DATA_CHANNEL);
  if (signal_id) {
    g_signal_emit_by_name(self->data_channel, method, method_call);
    return;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_MEDIA_STREAM);
  if (signal_id) {
    g_signal_emit_by_name(self->media_stream, method, method_call);
    return;
  }

  signal_id = g_signal_lookup(method, FL_TYPE_WEBRTC_VIDEO_RENDERER);
  if (signal_id) {
    g_signal_emit_by_name(self->video_renderer, method, method_call);
    return;
  }

  if (strcmp(method, "getPlatformVersion") == 0) {
    struct utsname uname_data = {};
    uname(&uname_data);
    g_autofree gchar *version = g_strdup_printf("Linux %s", uname_data.version);
    g_autoptr(FlValue) result = fl_value_new_string(version);
    response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
  } else {
    response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
  }

  fl_method_call_respond(method_call, response, nullptr);
}

static void method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  FlutterWebrtcPlugin* plugin = FLUTTER_WEBRTC_PLUGIN(user_data);
  flutter_webrtc_plugin_handle_method_call(plugin, method_call);
}

static void webrtc_method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  FlutterWebrtcPlugin* plugin = FLUTTER_WEBRTC_PLUGIN(user_data);
  flutter_webrtc_plugin_handle_method_call(plugin, method_call);
}

void flutter_web_r_t_c_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  FlutterWebrtcPlugin* plugin = flutter_webrtc_plugin_new(
                                fl_plugin_registrar_get_messenger(registrar),
                                fl_plugin_registrar_get_texture_registrar(registrar));

  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            "flutter_webrtc",
                            FL_METHOD_CODEC(codec));
  fl_method_channel_set_method_call_handler(channel, method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_autoptr(FlStandardMethodCodec) codec1 = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel1 =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            kChannelName,
                            FL_METHOD_CODEC(codec1));
  fl_method_channel_set_method_call_handler(channel1, webrtc_method_call_cb,
                                            g_object_ref(plugin),
                                            g_object_unref);

  g_object_unref(plugin);
}
