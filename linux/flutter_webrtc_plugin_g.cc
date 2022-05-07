#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc"

#include "flutter_webrtc/flutter_web_r_t_c_plugin.h"

#include "fl_webrtc_context.h"
#include "fl_webrtc_log.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

const char *kChannelName = "FlutterWebRTC.Method";

// Called when a method call is received from Flutter.
static void flutter_webrtc_plugin_handle_method_call(FlWebrtcContext* self,
                                                     FlMethodCall* method_call) {
  const gchar* method = fl_method_call_get_name(method_call);
  FL_WEBRTC_LOGI("invoking method: %s", method);

  gboolean handled = FALSE;
  handled = fl_webrtc_context_invoke_method(self, method_call);
  if (handled) return;

  g_autoptr(FlMethodResponse) response = nullptr;
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

static void webrtc_method_call_cb(FlMethodChannel* channel, FlMethodCall* method_call,
                           gpointer user_data) {
  FlWebrtcContext* context = FL_WEBRTC_CONTEXT(user_data);
  flutter_webrtc_plugin_handle_method_call(context, method_call);
}

void flutter_web_r_t_c_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  FlWebrtcContext* context = fl_webrtc_context_new(
      fl_plugin_registrar_get_messenger(registrar), fl_plugin_registrar_get_texture_registrar(registrar));

  g_autoptr(FlStandardMethodCodec) codec1 = fl_standard_method_codec_new();
  g_autoptr(FlMethodChannel) channel1 =
      fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                            kChannelName,
                            FL_METHOD_CODEC(codec1));
  fl_method_channel_set_method_call_handler(channel1, webrtc_method_call_cb,
                                            g_object_ref(context),
                                            g_object_unref);

  g_object_unref(context);
}
