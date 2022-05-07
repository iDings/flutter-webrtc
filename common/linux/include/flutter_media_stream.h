#ifndef FLUTTER_WEBRTC_RTC_GET_USERMEDIA_HXX
#define FLUTTER_WEBRTC_RTC_GET_USERMEDIA_HXX

#include "flutter_webrtc_base.h"

namespace flutter_webrtc_plugin {

class FlutterMediaStream {
 public:
  FlutterMediaStream(FlutterWebRTCBase *base) : base_(base) {}

  void GetUserMedia(const FlValue *constraints, FlMethodCall *result);

  void GetUserAudio(const FlValue *constraints,
                    scoped_refptr<RTCMediaStream> stream, FlValue *params);

  void GetUserVideo(const FlValue *constraints,
                    scoped_refptr<RTCMediaStream> stream, FlValue *params);

  void GetSources(FlMethodCall *result);

  void MediaStreamGetTracks(const std::string &stream_id, FlMethodCall *result);

  void MediaStreamDispose(const std::string &stream_id, FlMethodCall *result);

  void MediaStreamTrackSetEnable(const std::string &track_id, FlMethodCall *result);

  void MediaStreamTrackSwitchCamera(const std::string &track_id, FlMethodCall *result);

  void MediaStreamTrackDispose(const std::string &track_id, FlMethodCall *result);

 private:
  FlutterWebRTCBase *base_;
};

}  // namespace flutter_webrtc_plugin

#endif  // !FLUTTER_WEBRTC_RTC_GET_USERMEDIA_HXX
