#ifndef FLUTTER_WEBRTC_BASE_HXX
#define FLUTTER_WEBRTC_BASE_HXX

#include <flutter_linux/flutter_linux.h>

#include <string.h>
#include <list>
#include <map>
#include <memory>
#include <mutex>

#include "libwebrtc.h"
#include "rtc_audio_device.h"
#include "rtc_media_stream.h"
#include "rtc_media_track.h"
#include "rtc_mediaconstraints.h"
#include "rtc_peerconnection.h"
#include "rtc_peerconnection_factory.h"
#include "rtc_video_device.h"
#include "uuidxx.h"

namespace flutter_webrtc_plugin {

using namespace libwebrtc;

class FlutterVideoRenderer;
class FlutterRTCDataChannelObserver;
class FlutterPeerConnectionObserver;

// nullptr if none
inline FlValue *findEncodableValue(const FlValue *map, const std::string& key) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  return keyV;
}

inline FlValue *findMap(const FlValue *map, const std::string& key) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  if (fl_value_get_type(keyV) == FL_VALUE_TYPE_MAP) return keyV;
  return nullptr;
}

inline FlValue *findList(const FlValue *map, const std::string& key) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  if (fl_value_get_type(keyV) == FL_VALUE_TYPE_LIST) return keyV;
  return nullptr;
}

inline std::string findString(const FlValue *map, const std::string& key) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  if (fl_value_get_type(keyV) == FL_VALUE_TYPE_STRING)
    return fl_value_get_string(keyV);
  return std::string();
}

// fl_value is int64_t
inline int findInt(const FlValue *map, const std::string& key) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  if (fl_value_get_type(keyV) == FL_VALUE_TYPE_INT)
    return fl_value_get_int(keyV);
  return -1;
}

inline int64_t findLongInt(const FlValue *map, const std::string& key) {
FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *keyV = fl_value_lookup_string(mapV, key.c_str());
  if (fl_value_get_type(keyV) == FL_VALUE_TYPE_INT)
    return fl_value_get_int(keyV);
  return -1;
}

inline int toInt(FlValue *inputVal, int defaultVal) {
  int intValue = defaultVal;
  FlValueType vtype = fl_value_get_type(inputVal);
  if (vtype == FL_VALUE_TYPE_INT) {
    intValue = fl_value_get_int(inputVal);
  } else if (vtype == FL_VALUE_TYPE_STRING) {
    intValue = atoi(fl_value_get_string(inputVal));
  }
  return intValue;
}

class FlutterWebRTCBase {
 public:
  friend class FlutterMediaStream;
  friend class FlutterPeerConnection;
  friend class FlutterVideoRendererManager;
  friend class FlutterDataChannel;
  friend class FlutterPeerConnectionObserver;
  enum ParseConstraintType { kMandatory, kOptional };

 public:
  FlutterWebRTCBase(FlBinaryMessenger* messenger, FlTextureRegistrar* textures);
  ~FlutterWebRTCBase();

  std::string GenerateUUID();

  RTCPeerConnection* PeerConnectionForId(const std::string& id);

  void RemovePeerConnectionForId(const std::string& id);

  RTCMediaTrack* MediaTrackForId(const std::string& id);

  void RemoveMediaTrackForId(const std::string& id);

  FlutterPeerConnectionObserver* PeerConnectionObserversForId(
      const std::string& id);

  void RemovePeerConnectionObserversForId(const std::string& id);

  scoped_refptr<RTCMediaStream> MediaStreamForId(const std::string& id);

  void RemoveStreamForId(const std::string& id);

  bool ParseConstraints(const FlValue *constraints,
                        RTCConfiguration* configuration);

  scoped_refptr<RTCMediaConstraints> ParseMediaConstraints(
      const FlValue *constraints);

  bool ParseRTCConfiguration(const FlValue *map,
                             RTCConfiguration& configuration);

  scoped_refptr<RTCMediaTrack> MediaTracksForId(const std::string& id);

  void RemoveTracksForId(const std::string& id);

 private:
  void ParseConstraints(const FlValue *src,
                        scoped_refptr<RTCMediaConstraints> mediaConstraints,
                        ParseConstraintType type = kMandatory);

  bool CreateIceServers(const FlValue *iceServersArray,
                        IceServer* ice_servers);

 protected:
  scoped_refptr<RTCPeerConnectionFactory> factory_;
  scoped_refptr<RTCAudioDevice> audio_device_;
  scoped_refptr<RTCVideoDevice> video_device_;
  RTCConfiguration configuration_;

  std::map<std::string, scoped_refptr<RTCPeerConnection>> peerconnections_;
  std::map<std::string, scoped_refptr<RTCMediaStream>> local_streams_;
  std::map<std::string, scoped_refptr<RTCMediaTrack>> local_tracks_;
  std::map<std::string, scoped_refptr<RTCDataChannel>> data_channels_;
  std::map<int64_t, std::shared_ptr<FlutterVideoRenderer>> renders_;
  std::map<int, std::shared_ptr<FlutterRTCDataChannelObserver>>
      data_channel_observers_;
  std::map<std::string, std::shared_ptr<FlutterPeerConnectionObserver>>
      peerconnection_observers_;
  mutable std::mutex mutex_;

  void lock() { mutex_.lock(); }
  void unlock() { mutex_.unlock(); }

 protected:
  FlBinaryMessenger* messenger_;
  FlTextureRegistrar* textures_;
};

}  // namespace flutter_webrtc_plugin

#endif  // !FLUTTER_WEBRTC_BASE_HXX
