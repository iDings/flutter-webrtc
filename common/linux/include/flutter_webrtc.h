#ifndef PLUGINS_FLUTTER_WEBRTC_HXX
#define PLUGINS_FLUTTER_WEBRTC_HXX

#include <flutter_linux/flutter_linux.h>

#include "flutter_data_channel.h"
#include "flutter_media_stream.h"
#include "flutter_peerconnection.h"
#include "flutter_video_renderer.h"
#include "libwebrtc.h"

namespace flutter_webrtc_plugin {
using namespace libwebrtc;

class FlutterWebRTCPlugin {
 public:
  virtual FlBinaryMessenger *messenger() = 0;
  virtual FlTextureRegistrar *textures() = 0;
};

class FlutterWebRTC : public FlutterWebRTCBase,
                      public FlutterVideoRendererManager,
                      public FlutterMediaStream,
                      public FlutterPeerConnection,
                      public FlutterDataChannel {
 public:
  FlutterWebRTC(FlutterWebRTCPlugin *plugin);
  virtual ~FlutterWebRTC();

  void HandleMethodCall(FlMethodCall *method_call);
};

}  // namespace flutter_webrtc_plugin

#endif  // PLUGINS_FLUTTER_WEBRTC_HXX
