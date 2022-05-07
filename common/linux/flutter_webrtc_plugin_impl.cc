#include "flutter_webrtc/flutter_web_r_t_c_plugin.h"

#include <flutter_linux/flutter_linux.h>

#include "flutter_webrtc.h"

const char *kChannelName = "FlutterWebRTC.Method";

namespace flutter_webrtc_plugin {

// A webrtc plugin for windows/linux.
class FlutterWebRTCPluginImpl : public FlutterWebRTCPlugin {
 public:
   static void FlutterWebRTCMethodCallCB(FlMethodChannel* channel, FlMethodCall* method_call, gpointer user_data) {
     FlutterWebRTCPluginImpl* impl = static_cast<FlutterWebRTCPluginImpl *>(user_data);
     impl->HandleMethodCall(method_call);
   }

  static void FlutterWebRTCPluginImplDestroyNotify(gpointer data) {
    FlutterWebRTCPluginImpl* impl = static_cast<FlutterWebRTCPluginImpl *>(data);
    delete impl;
  }

  static void RegisterWithRegistrar(FlPluginRegistrar *registrar) {
    g_autoptr(FlStandardMethodCodec) codec1 = fl_standard_method_codec_new();
    FlMethodChannel *channel1 = fl_method_channel_new(fl_plugin_registrar_get_messenger(registrar),
                                                      kChannelName,
                                                      FL_METHOD_CODEC(codec1));

    FlutterWebRTCPluginImpl* impl = new FlutterWebRTCPluginImpl(registrar, channel1);
    fl_method_channel_set_method_call_handler(channel1,
                                              FlutterWebRTCMethodCallCB,
                                              impl, FlutterWebRTCPluginImplDestroyNotify);
  }

  virtual ~FlutterWebRTCPluginImpl() { g_object_unref(channel_); }

  void HandleMethodCall(FlMethodCall *method_call) {
    // handle method call and forward to webrtc native sdk.
    webrtc_->HandleMethodCall(method_call);
  }

  FlBinaryMessenger *messenger() { return messenger_; }
  FlTextureRegistrar *textures() { return textures_; }

 protected:
  // Creates a plugin that communicates on the given channel.
  FlutterWebRTCPluginImpl(FlPluginRegistrar *registrar, FlMethodChannel *channel)
      : channel_(channel),
        messenger_(fl_plugin_registrar_get_messenger(registrar)),
        textures_(fl_plugin_registrar_get_texture_registrar(registrar)) {
    webrtc_ = std::make_unique<FlutterWebRTC>(this);
  }

 private:
  FlMethodChannel *channel_;
  std::unique_ptr<FlutterWebRTC> webrtc_;
  FlBinaryMessenger *messenger_;
  FlTextureRegistrar *textures_;
};

}  // namespace flutter_webrtc_plugin

void FlutterWebRTCPluginRegisterWithRegistrar(FlPluginRegistrar *registrar) {
  flutter_webrtc_plugin::FlutterWebRTCPluginImpl::RegisterWithRegistrar(registrar);
}
