#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc"

#include "flutter_webrtc/flutter_web_r_t_c_plugin.h"

#include <flutter_linux/flutter_linux.h>
#include <gtk/gtk.h>
#include <sys/utsname.h>

#include <cstring>

extern void FlutterWebRTCPluginRegisterWithRegistrar(FlPluginRegistrar *registrar);
void flutter_web_r_t_c_plugin_register_with_registrar(FlPluginRegistrar* registrar) {
  FlutterWebRTCPluginRegisterWithRegistrar(registrar);
}
