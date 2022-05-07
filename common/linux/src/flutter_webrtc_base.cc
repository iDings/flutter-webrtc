#include "flutter_webrtc_base.h"

#include "flutter_data_channel.h"
#include "flutter_peerconnection.h"
#include "flutter_glog.h"

namespace flutter_webrtc_plugin {

FlutterWebRTCBase::FlutterWebRTCBase(FlBinaryMessenger *messenger,
                                     FlTextureRegistrar *textures)
    : messenger_(messenger), textures_(textures) {
  LibWebRTC::Initialize();
  factory_ = LibWebRTC::CreateRTCPeerConnectionFactory();
  audio_device_ = factory_->GetAudioDevice();
  video_device_ = factory_->GetVideoDevice();
}

FlutterWebRTCBase::~FlutterWebRTCBase() {
  LibWebRTC::Terminate();
}

std::string FlutterWebRTCBase::GenerateUUID() {
  return uuidxx::uuid::Generate().ToString(false);
}

RTCPeerConnection *FlutterWebRTCBase::PeerConnectionForId(
    const std::string &id) {
  auto it = peerconnections_.find(id);

  if (it != peerconnections_.end()) return (*it).second.get();

  return nullptr;
}

void FlutterWebRTCBase::RemovePeerConnectionForId(const std::string &id) {
  auto it = peerconnections_.find(id);
  if (it != peerconnections_.end()) peerconnections_.erase(it);
}

RTCMediaTrack* FlutterWebRTCBase ::MediaTrackForId(const std::string& id) {
  auto it = local_tracks_.find(id);

  if (it != local_tracks_.end())
    return (*it).second.get();

  for (auto kv : peerconnection_observers_) {
      auto pco = kv.second.get();
      auto track = pco->MediaTrackForId(id);
      if (track != nullptr) return track;
  }

  return nullptr;
}

void FlutterWebRTCBase::RemoveMediaTrackForId(const std::string& id) {
  auto it = local_tracks_.find(id);
  if (it != local_tracks_.end())
    local_tracks_.erase(it);
}

FlutterPeerConnectionObserver* FlutterWebRTCBase::PeerConnectionObserversForId(
    const std::string& id) {
  auto it = peerconnection_observers_.find(id);

  if (it != peerconnection_observers_.end())
    return (*it).second.get();

  return nullptr;
}

void FlutterWebRTCBase::RemovePeerConnectionObserversForId(
    const std::string& id) {
  auto it = peerconnection_observers_.find(id);
  if (it != peerconnection_observers_.end())
    peerconnection_observers_.erase(it);
}

scoped_refptr<RTCMediaStream> FlutterWebRTCBase::MediaStreamForId(
    const std::string& id) {
  auto it = local_streams_.find(id);
  if (it != local_streams_.end()) {
    return (*it).second;
  }

  for (auto kv : peerconnection_observers_) {
    auto pco = kv.second.get();
    auto stream = pco->MediaStreamForId(id);
    if (stream != nullptr) return stream;
  }

  return nullptr;
}

void FlutterWebRTCBase::RemoveStreamForId(const std::string &id) {
  auto it = local_streams_.find(id);
  if (it != local_streams_.end()) local_streams_.erase(it);
}

bool FlutterWebRTCBase::ParseConstraints(const FlValue *constraints,
                                         RTCConfiguration *configuration) {
  memset(&configuration->ice_servers, 0, sizeof(configuration->ice_servers));
  return false;
}

void FlutterWebRTCBase::ParseConstraints(
    const FlValue *src,
    scoped_refptr<RTCMediaConstraints> mediaConstraints,
    ParseConstraintType type /*= kMandatory*/) {
  FlValue *srcV = const_cast<FlValue *>(src);
  size_t size = fl_value_get_length(srcV);
  for (size_t i = 0; i < size; i++) {
    FlValue *k = fl_value_get_map_key(srcV, i);
    FlValue *v = fl_value_get_map_value(srcV, i);
    FlValueType vtype = fl_value_get_type(v);

    std::string key = fl_value_get_string(k);
    std::string value;
    if (vtype == FL_VALUE_TYPE_STRING) {
      value = fl_value_get_string(v);
    } else if (vtype == FL_VALUE_TYPE_FLOAT) {
      value = std::to_string(fl_value_get_float(v));
    } else if (vtype == FL_VALUE_TYPE_INT) {
      value = std::to_string(fl_value_get_int(v));
    } else if (vtype == FL_VALUE_TYPE_BOOL) {
      value = fl_value_get_bool(v) ? RTCMediaConstraints::kValueTrue
                                : RTCMediaConstraints::kValueFalse;
    } else {
      FL_LOGI("unhandled value type:%d", vtype);
      continue;
    }

    if (type == kMandatory) {
      mediaConstraints->AddMandatoryConstraint(key.c_str(), value.c_str());
    } else {
      mediaConstraints->AddOptionalConstraint(key.c_str(), value.c_str());
      if (key == "DtlsSrtpKeyAgreement") {
        configuration_.srtp_type = fl_value_get_bool(v) ?
                    MediaSecurityType::kDTLS_SRTP : MediaSecurityType::kSDES_SRTP;
      }
    }
  }
}

scoped_refptr<RTCMediaConstraints> FlutterWebRTCBase::ParseMediaConstraints(
    const FlValue *constraints) {
  scoped_refptr<RTCMediaConstraints> media_constraints = RTCMediaConstraints::Create();
  FlValue *constraintsV = const_cast<FlValue *>(constraints);
  FlValue *mandatoryV = fl_value_lookup_string(constraintsV, "mandatory");
  if (fl_value_get_type(mandatoryV) == FL_VALUE_TYPE_MAP) {
    ParseConstraints(mandatoryV, media_constraints, kMandatory);
  }

  FlValue *optionalV = fl_value_lookup_string(constraintsV, "optional");
  if (optionalV) {
    FlValueType vtype = fl_value_get_type(optionalV);
    if (vtype == FL_VALUE_TYPE_LIST) {
      size_t size = fl_value_get_length(optionalV);
      for (size_t i = 0; i < size; i++) {
        FlValue *item = fl_value_get_list_value(optionalV, i);
        ParseConstraints(item, media_constraints, kOptional);
      }
    } else if (vtype == FL_VALUE_TYPE_MAP) {
      ParseConstraints(optionalV, media_constraints, kOptional);
    } else {
      FL_LOGW("unhandled type:%d", vtype);
    }
  }

  return media_constraints;
}

bool FlutterWebRTCBase::CreateIceServers(const FlValue *iceServersArray,
                                         IceServer *ice_servers) {
  FlValue *iceServersArrayV = const_cast<FlValue *>(iceServersArray);
  size_t size = fl_value_get_length(iceServersArrayV);
  for (size_t i = 0; i < size; i++) {
    IceServer &ice_server = ice_servers[i];
    FlValue *iceServerMap = fl_value_get_list_value(iceServersArrayV, i);
    FlValue *usernameV = fl_value_lookup_string(iceServerMap, "username");
    FlValue *credentialV =  fl_value_lookup_string(iceServerMap, "credential");
    bool hasUsernameAndCredential = usernameV && credentialV;

    FlValue *urlV = fl_value_lookup_string(iceServerMap, "url");
    if (urlV && fl_value_get_type(urlV) == FL_VALUE_TYPE_STRING) {
      if (hasUsernameAndCredential) {
        ice_server.username = std::string(fl_value_get_string(usernameV));
        ice_server.password = std::string(fl_value_get_string(credentialV));
      }

      std::string uri = fl_value_get_string(urlV);
      ice_server.uri = uri;
    }

    FlValue *urlsV = fl_value_lookup_string(iceServerMap, "urls");
    if (urlsV && fl_value_get_type(urlsV) == FL_VALUE_TYPE_STRING) {
      if (hasUsernameAndCredential) {
        ice_server.username = std::string(fl_value_get_string(usernameV));
        ice_server.password = std::string(fl_value_get_string(credentialV));
      }

      std::string uri = fl_value_get_string(urlsV);
      ice_server.uri = uri;
    }

    if (fl_value_get_type(urlsV) == FL_VALUE_TYPE_LIST) {
      size_t size = fl_value_get_length(urlsV);
      for (size_t i = 0; i < size; i++) {
        FlValue *item = fl_value_get_list_value(urlsV, i);
        FlValueType vtype = fl_value_get_type(item);
        if (vtype == FL_VALUE_TYPE_MAP) {
          FlValue* urlV = fl_value_lookup_string(item, "url");
          if (urlV && fl_value_get_type(urlV) == FL_VALUE_TYPE_STRING) {
            if (hasUsernameAndCredential) {
              ice_server.username = std::string(fl_value_get_string(usernameV));
              ice_server.password = std::string(fl_value_get_string(credentialV));
            }
            std::string uri = fl_value_get_string(urlV);
            ice_server.uri = uri;
          }
        } else if (vtype == FL_VALUE_TYPE_STRING) {
          std::string urlString = fl_value_get_string(item);
          ice_server.uri = urlString;
        }
      }
    }
  }

  return size > 0;
}

bool FlutterWebRTCBase::ParseRTCConfiguration(const FlValue *map,
                                              RTCConfiguration &conf) {
  FlValue *mapV = const_cast<FlValue *>(map);
  FlValue *iceServersV = fl_value_lookup_string(mapV, "iceServers");
  if (iceServersV && fl_value_get_type(iceServersV) == FL_VALUE_TYPE_LIST) {
    CreateIceServers(iceServersV, conf.ice_servers);
  }

  // iceTransportPolicy (public API)
  FlValue *iceTransportPolicyV = fl_value_lookup_string(mapV, "iceTransportPolicy");
  if (iceTransportPolicyV && fl_value_get_type(iceTransportPolicyV) == FL_VALUE_TYPE_STRING) {
    std::string v = fl_value_get_string(iceTransportPolicyV);
    if (v == "all")  // public
      conf.type = IceTransportsType::kAll;
    else if (v == "relay")
      conf.type = IceTransportsType::kRelay;
    else if (v == "nohost")
      conf.type = IceTransportsType::kNoHost;
    else if (v == "none")
      conf.type = IceTransportsType::kNone;
  }

  // bundlePolicy (public api)
  FlValue *bundlePolicyV = fl_value_lookup_string(mapV, "bundlePolicy");
  if (bundlePolicyV && fl_value_get_type(bundlePolicyV) == FL_VALUE_TYPE_STRING) {
    std::string v = fl_value_get_string(bundlePolicyV);
    if (v == "balanced")  // public
      conf.bundle_policy = kBundlePolicyBalanced;
    else if (v == "max-compat")  // public
      conf.bundle_policy = kBundlePolicyMaxCompat;
    else if (v == "max-bundle")  // public
      conf.bundle_policy = kBundlePolicyMaxBundle;
  }

  // rtcpMuxPolicy (public api)
  FlValue *rtcpMuxPolicyV = fl_value_lookup_string(mapV, "rtcpMuxPolicy");
  if (rtcpMuxPolicyV && fl_value_get_type(rtcpMuxPolicyV) == FL_VALUE_TYPE_STRING) {
    std::string v = fl_value_get_string(rtcpMuxPolicyV);
    if (v == "negotiate")  // public
      conf.rtcp_mux_policy = RtcpMuxPolicy::kRtcpMuxPolicyNegotiate;
    else if (v == "require")  // public
      conf.rtcp_mux_policy = RtcpMuxPolicy::kRtcpMuxPolicyRequire;
  }

  // FIXME: peerIdentity of type DOMString (public API)
  // FIXME: certificates of type sequence<RTCCertificate> (public API)
  // iceCandidatePoolSize of type unsigned short, defaulting to 0
  FlValue *iceCandidatePoolSizeV = fl_value_lookup_string(mapV, "iceCandidatePoolSize");
  if (iceCandidatePoolSizeV) {
    conf.ice_candidate_pool_size = fl_value_get_int(iceCandidatePoolSizeV);
  }

  // sdpSemantics (public api)
  FlValue *sdpSemanticsV = fl_value_lookup_string(mapV, "sdpSemantics");
  if (sdpSemanticsV && fl_value_get_type(sdpSemanticsV) == FL_VALUE_TYPE_STRING) {
    std::string v = fl_value_get_string(sdpSemanticsV);
    if (v == "plan-b")  // public
      conf.sdp_semantics = SdpSemantics::kPlanB;
    else if (v == "unified-plan")  // public
      conf.sdp_semantics = SdpSemantics::kUnifiedPlan;
  }

  return true;
}

scoped_refptr<RTCMediaTrack> FlutterWebRTCBase::MediaTracksForId(
    const std::string& id) {
  auto it = local_tracks_.find(id);
  if (it != local_tracks_.end()) {
    return (*it).second;
  }

  return nullptr;
}

void FlutterWebRTCBase::RemoveTracksForId(const std::string& id) {
  auto it = local_tracks_.find(id);
  if (it != local_tracks_.end())
    local_tracks_.erase(it);
}


}  // namespace flutter_webrtc_plugin
