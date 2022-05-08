#include "flutter_peerconnection.h"
#include "flutter_glog.h"

#include "base/scoped_ref_ptr.h"
#include "flutter_data_channel.h"
#include "rtc_dtmf_sender.h"
#include "rtc_rtp_parameters.h"

namespace flutter_webrtc_plugin {

std::string RTCMediaTypeToString(RTCMediaType type) {
  switch (type) {
    case libwebrtc::RTCMediaType::ANY:
      return "any";
    case libwebrtc::RTCMediaType::AUDIO:
      return "audio";
    case libwebrtc::RTCMediaType::VIDEO:
      return "video";
    case libwebrtc::RTCMediaType::DATA:
      return "data";
    default:
      return "";
  }
}
std::string transceiverDirectionString(RTCRtpTransceiverDirection direction) {
  switch (direction) {
    case RTCRtpTransceiverDirection::kSendRecv:
      return "sendrecv";
    case RTCRtpTransceiverDirection::kSendOnly:
      return "sendonly";
    case RTCRtpTransceiverDirection::kRecvOnly:
      return "recvonly";
    case RTCRtpTransceiverDirection::kInactive:
      return "inactive";
    case RTCRtpTransceiverDirection::kStopped:
      return "stoped";
  }
  return "";
}

// caller's ownership
FlValue *rtpParametersToMap(
    libwebrtc::scoped_refptr<libwebrtc::RTCRtpParameters> rtpParameters) {
  FlValue *info = fl_value_new_map();
  fl_value_set_string_take(
      info, "transactionId",
      fl_value_new_string(
          rtpParameters->transaction_id().std_string().c_str()));

  FlValue *rtcp = fl_value_new_map();
  fl_value_set_string_take(
      rtcp, "cname",
      fl_value_new_string(
          rtpParameters->rtcp_parameters()->cname().std_string().c_str()));
  fl_value_set_string_take(
      rtcp, "reducedSize",
      fl_value_new_bool(
          rtpParameters->rtcp_parameters()->reduced_size()));
  fl_value_set_string_take(info, "rtcp", rtcp);

  FlValue *headerExtensions = fl_value_new_list();
  auto header_extensions = rtpParameters->header_extensions();
  for (scoped_refptr<libwebrtc::RTCRtpExtension> extension : header_extensions.std_vector()) {
    FlValue *map = fl_value_new_map();
    fl_value_set_string_take(
        map, "uri", fl_value_new_string(extension->uri().std_string().c_str()));
    fl_value_set_string_take(map, "id", fl_value_new_int(extension->id()));
    fl_value_set_string_take(map, "encrypted", fl_value_new_bool(extension->encrypt()));

    fl_value_append_take(headerExtensions, map);
  }
  fl_value_set_string_take(info, "headerExtensions", headerExtensions);

  FlValue *encodings_info = fl_value_new_list();
  auto encodings = rtpParameters->encodings();
  for (scoped_refptr<libwebrtc::RTCRtpEncodingParameters> encoding : encodings.std_vector()) {
    FlValue *map = fl_value_new_map();
    fl_value_set_string_take(map, "active",
                             fl_value_new_bool(encoding->active()));
    fl_value_set_string_take(map, "maxBitrate",
                             fl_value_new_int(encoding->max_bitrate_bps()));
    fl_value_set_string_take(map, "minBitrate",
                             fl_value_new_int(encoding->min_bitrate_bps()));
    fl_value_set_string_take(map, "maxFramerate",
                             fl_value_new_float(encoding->max_framerate()));
    fl_value_set_string_take(
        map, "scaleResolutionDownBy",
        fl_value_new_float(encoding->scale_resolution_down_by()));
    fl_value_set_string_take(map, "ssrc", fl_value_new_int(encoding->ssrc()));

    fl_value_append_take(encodings_info, map);
  }
  fl_value_set_string_take(info, "encodings", encodings_info);

  FlValue *codecs_info = fl_value_new_list();
  auto codecs = rtpParameters->codecs();
  for (scoped_refptr<RTCRtpCodecParameters> codec : codecs.std_vector()) {
    FlValue *map = fl_value_new_map();
    fl_value_set_string_take(
        map, "name", fl_value_new_string(codec->name().std_string().c_str()));
    fl_value_set_string_take(map, "payloadType",
                             fl_value_new_int(codec->payload_type()));
    fl_value_set_string_take(map, "clockRate", fl_value_new_int(codec->clock_rate()));
    fl_value_set_string_take(map, "numChannels", fl_value_new_int(codec->num_channels()));

    FlValue *param = fl_value_new_map();
    auto parameters = codec->parameters();
    for (auto item : parameters.std_vector()) {
      fl_value_set_string_take(
          param, item.first.std_string().c_str(),
          fl_value_new_string(item.second.std_string().c_str()));
    }
    fl_value_set_string_take(map, "parameters", param);
    fl_value_set_string_take(
        map, "kind",
        fl_value_new_string(RTCMediaTypeToString(codec->kind()).c_str()));

    fl_value_append_take(codecs_info, map);
  }
  fl_value_set_string_take(info, "codecs", codecs_info);

  return info;
}

FlValue* dtmfSenderToMap(scoped_refptr<RTCDtmfSender> dtmfSender,
                         std::string id) {
  FlValue *info = fl_value_new_map();
  if (nullptr != dtmfSender.get()) {
    fl_value_set_string_take(info, "dtmfSenderId", fl_value_new_string(id.c_str()));
    if (dtmfSender.get()) {
      fl_value_set_string_take(info, "interToneGap",
                               fl_value_new_int(dtmfSender->inter_tone_gap()));
      fl_value_set_string_take(info, "duration", fl_value_new_int(dtmfSender->duration()));
    }
  }

  return info;
}

FlValue *mediaTrackToMap(
    libwebrtc::scoped_refptr<libwebrtc::RTCMediaTrack> track) {
  FlValue *info = fl_value_new_map();
  if (nullptr == track.get()) {
    return info;
  }

  fl_value_set_string_take(info, "id", fl_value_new_string(track->id().std_string().c_str()));
  fl_value_set_string_take(info, "kind", fl_value_new_string(track->kind().std_string().c_str()));

  std::string kind = track->kind().std_string();
  if (0 == kind.compare("video")) {
    fl_value_set_string_take(
        info, "readyState",
        fl_value_new_int(static_cast<RTCVideoTrack*>(track.get())->state()));
    fl_value_set_string_take(info, "label", fl_value_new_string("video"));
  } else if (0 == kind.compare("audio")) {
    fl_value_set_string_take(
        info, "readyState",
        fl_value_new_int(static_cast<RTCAudioTrack*>(track.get())->state()));
    fl_value_set_string_take(info, "label", fl_value_new_string("audio"));
  }
  fl_value_set_string_take(info, "enabled", fl_value_new_bool(track->enabled()));

  return info;
}

FlValue *rtpSenderToMap(
    libwebrtc::scoped_refptr<libwebrtc::RTCRtpSender> sender) {
  FlValue *info = fl_value_new_map();
  std::string id = sender->id().std_string();
  fl_value_set_string_take(info, "senderId", fl_value_new_string(id.c_str()));
  fl_value_set_string_take(info, "ownsTrack", fl_value_new_bool(true));
  fl_value_set_string_take(info, "dtmfSender", dtmfSenderToMap(sender->dtmf_sender(), id));
  fl_value_set_string_take(info, "rtpParameters", rtpParametersToMap(sender->parameters()));
  fl_value_set_string_take(info, "track", mediaTrackToMap(sender->track()));

  return info;
}

std::string trackStateToString(libwebrtc::RTCMediaTrack::RTCTrackState state) {
  switch (state) {
    case libwebrtc::RTCMediaTrack::kLive:
      return "live";
    case libwebrtc::RTCMediaTrack::kEnded:
      return "ended";
    default:
      return "";
  }
}

FlValue *rtpReceiverToMap(
    libwebrtc::scoped_refptr<libwebrtc::RTCRtpReceiver> receiver) {
  FlValue *info = fl_value_new_map();
  fl_value_set_string_take(info, "receiverId", fl_value_new_string(receiver->id().std_string().c_str()));
  fl_value_set_string_take(info, "rtpParameters", rtpParametersToMap(receiver->parameters()));
  fl_value_set_string_take(info, "track", mediaTrackToMap(receiver->track()));

  return info;
}

FlValue *transceiverToMap(scoped_refptr<RTCRtpTransceiver> transceiver) {
  FlValue *info = fl_value_new_map();
  std::string mid = transceiver->mid().std_string();
  fl_value_set_string_take(info, "transceiverId", fl_value_new_string(mid.c_str()));
  fl_value_set_string_take(info, "mid", fl_value_new_string(mid.c_str()));
  fl_value_set_string_take(
      info, "direction",
      fl_value_new_string(
          transceiverDirectionString(transceiver->direction()).c_str()));
  fl_value_set_string_take(info, "sender", rtpSenderToMap(transceiver->sender()));
  fl_value_set_string_take(info, "receiver", rtpReceiverToMap(transceiver->receiver()));

  return info;
}

FlValue *mediaStreamToMap(scoped_refptr<RTCMediaStream> stream,
                              std::string id) {
  FlValue *params = fl_value_new_map();
  fl_value_set_string_take(
      params, "streamId",
      fl_value_new_string(stream->id().std_string().c_str()));
  fl_value_set_string_take(params, "ownerTag", fl_value_new_string(id.c_str()));

  FlValue *audioTracks = fl_value_new_list();
  auto audio_tracks = stream->audio_tracks();
  for (scoped_refptr<RTCAudioTrack> val : audio_tracks.std_vector()) {
    fl_value_append_take(audioTracks, mediaTrackToMap(val));
  }
  fl_value_set_string_take(params, "audioTracks", audioTracks);

  FlValue *videoTracks = fl_value_new_list();
  auto video_tracks = stream->video_tracks();
  for (scoped_refptr<RTCVideoTrack> val : video_tracks.std_vector()) {
    fl_value_append_take(videoTracks, mediaTrackToMap(val));
  }
  fl_value_set_string_take(params, "videoTracks", videoTracks);
  return params;
}

void FlutterPeerConnection::CreateRTCPeerConnection(
    const FlValue *configurationMap,
    const FlValue *constraintsMap,
    FlMethodCall *result) {
  FL_LOGI("configuration = %s", fl_value_to_string(const_cast<FlValue *>(configurationMap)));
  base_->ParseRTCConfiguration(configurationMap, base_->configuration_);
  FL_LOGI(" constraints = %s", fl_value_to_string(const_cast<FlValue *>(constraintsMap)));
  scoped_refptr<RTCMediaConstraints> constraints =
      base_->ParseMediaConstraints(constraintsMap);

  std::string uuid = base_->GenerateUUID();
  scoped_refptr<RTCPeerConnection> pc =
      base_->factory_->Create(base_->configuration_, constraints);
  base_->peerconnections_[uuid] = pc;

  std::string event_channel = "FlutterWebRTC/peerConnectoinEvent" + uuid;

  std::unique_ptr<FlutterPeerConnectionObserver> observer(
      new FlutterPeerConnectionObserver(base_, pc, base_->messenger_,
                                        event_channel, uuid));

  base_->peerconnection_observers_[uuid] = std::move(observer);

  g_autoptr(FlValue) params = fl_value_new_map();
  fl_value_set_string_take(params, "peerConnectionId", fl_value_new_string(uuid.c_str()));
  fl_method_call_respond_success(result, params, nullptr);
}

void FlutterPeerConnection::RTCPeerConnectionClose(
    RTCPeerConnection* pc,
    const std::string& uuid,
    FlMethodCall *result) {
  pc->Close();
  fl_method_call_respond_success(result, nullptr, nullptr);
}

void FlutterPeerConnection::RTCPeerConnectionDispose(
    RTCPeerConnection* pc,
    const std::string& uuid,
    FlMethodCall *result) {
  auto it = base_->peerconnection_observers_.find(uuid);
  if (it != base_->peerconnection_observers_.end())
    base_->peerconnection_observers_.erase(it);
  fl_method_call_respond_success(result, nullptr, nullptr);
}

// TODO: check thread
void FlutterPeerConnection::CreateOffer(
    const FlValue *constraintsMap,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  scoped_refptr<RTCMediaConstraints> constraints =
      base_->ParseMediaConstraints(constraintsMap);

  g_object_ref(result);
  pc->CreateOffer(
      [result](const libwebrtc::string sdp, const libwebrtc::string type) {
        g_autoptr(FlValue) params = fl_value_new_map();
        fl_value_set_string_take(params, "sdp", fl_value_new_string(sdp.c_string()));
        fl_value_set_string_take(params, "type", fl_value_new_string(type.c_string()));
        fl_method_call_respond_success(result, params, nullptr);
        g_object_unref(result);
      },
      [result](const char* error) {
        fl_method_call_respond_error(result, "createOfferFailed", error, nullptr, nullptr);
        g_object_unref(result);
      },
      constraints);
}

void FlutterPeerConnection::CreateAnswer(
    const FlValue *constraintsMap,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  scoped_refptr<RTCMediaConstraints> constraints =
      base_->ParseMediaConstraints(constraintsMap);

  g_object_ref(result);
  pc->CreateAnswer(
      [result](const libwebrtc::string sdp, const libwebrtc::string type) {
        FL_LOGI("on_success");
        g_autoptr(FlValue) params = fl_value_new_map();
        fl_value_set_string_take(params, "sdp", fl_value_new_string(sdp.c_string()));
        fl_value_set_string_take(params, "type", fl_value_new_string(type.c_string()));
        fl_method_call_respond_success(result, params, nullptr);
        g_object_unref(result);
      },
      [result](const std::string& error) {
        FL_LOGI("on_failure");
        fl_method_call_respond_error(result, "createAnswerFailed", error.c_str(), nullptr, nullptr);
        g_object_unref(result);
      },
      constraints);
  FL_LOGI("answer done");
}

void FlutterPeerConnection::SetLocalDescription(
    RTCSessionDescription* sdp,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  g_object_ref(result);
  pc->SetLocalDescription(
      sdp->sdp(), sdp->type(),
      [result]() {
        fl_method_call_respond_success(result, nullptr, nullptr);
        g_object_unref(result);
      },
      [result](const char* error) {
        fl_method_call_respond_error(result, "setLocalDescriptionFailed", error,
                                     nullptr, nullptr);
        g_object_unref(result);
      });
}

void FlutterPeerConnection::SetRemoteDescription(
    RTCSessionDescription* sdp,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  g_object_ref(result);
  pc->SetRemoteDescription(
      sdp->sdp(), sdp->type(),
      [result]() {
        fl_method_call_respond_success(result, nullptr, nullptr);
        g_object_unref(result);
      },
      [result](const char* error) {
        fl_method_call_respond_error(result, "setRemoteDescriptionFailed",
                                     error, nullptr, nullptr);
        g_object_unref(result);
      });
}

void FlutterPeerConnection::GetLocalDescription(
    RTCPeerConnection* pc,
    FlMethodCall *resulte) {
  g_object_ref(resulte);
  pc->GetLocalDescription(
      [resulte](const char* sdp, const char* type) {
        g_autoptr(FlValue) params = fl_value_new_map();
        fl_value_set_string_take(params, "sdp", fl_value_new_string(sdp));
        fl_value_set_string_take(params, "type", fl_value_new_string(type));
        fl_method_call_respond_success(resulte, params, nullptr);
        g_object_unref(resulte);
      },
      [resulte](const std::string& error) {
        fl_method_call_respond_error(resulte, "GetLocalDescription",
                                     error.c_str(), nullptr, nullptr);
        g_object_unref(resulte);
      });
}

void FlutterPeerConnection::GetRemoteDescription(
    RTCPeerConnection* pc,
    FlMethodCall *resulte) {
  g_object_ref(resulte);
  pc->GetRemoteDescription(
      [resulte](const char* sdp, const char* type) {
        g_autoptr(FlValue) params = fl_value_new_map();
        fl_value_set_string_take(params, "sdp", fl_value_new_string(sdp));
        fl_value_set_string_take(params, "type", fl_value_new_string(type));
        fl_method_call_respond_success(resulte, params, nullptr);
        g_object_unref(resulte);
      },
      [resulte](const std::string& error) {
        fl_method_call_respond_error(resulte, "GetRemoteDescription",
                                     error.c_str(), nullptr, nullptr);
        g_object_unref(resulte);
      });
}

scoped_refptr<RTCRtpTransceiverInit>
FlutterPeerConnection::mapToRtpTransceiverInit(const FlValue *params) {
  FlValue *paramsV = const_cast<FlValue *>(params);

  FlValue *streamIds = findList(paramsV, "streamIds");
  std::vector<string> stream_ids;
  for (size_t i = 0; i < fl_value_get_length(streamIds); i++) {
    std::string id = fl_value_get_string(fl_value_get_list_value(streamIds, i));
    stream_ids.push_back(id.c_str());
  }
  RTCRtpTransceiverDirection dir = RTCRtpTransceiverDirection::kInactive;
  FlValue *direction = findEncodableValue(params, "direction");
  if (direction) {
    dir = stringToTransceiverDirection(fl_value_get_string(direction));
  }
  FlValue *sendEncodings = findList(params, "sendEncodings");
  std::vector<scoped_refptr<RTCRtpEncodingParameters>> encodings;
  for (size_t i = 0; i < fl_value_get_length(sendEncodings); i++) {
    FlValue *value = fl_value_get_list_value(sendEncodings, i);
    encodings.push_back(mapToEncoding(value));
  }
  scoped_refptr<RTCRtpTransceiverInit> init =
      RTCRtpTransceiverInit::Create(dir, stream_ids, encodings);
  return init;
}

RTCRtpTransceiverDirection FlutterPeerConnection::stringToTransceiverDirection(
    std::string direction) {
  if (0 == direction.compare("sendrecv")) {
    return RTCRtpTransceiverDirection::kSendRecv;
  } else if (0 == direction.compare("sendonly")) {
    return RTCRtpTransceiverDirection::kSendOnly;
  } else if (0 == direction.compare("recvonly")) {
    return RTCRtpTransceiverDirection::kRecvOnly;
  } else if (0 == direction.compare("stoped")) {
    return RTCRtpTransceiverDirection::kStopped;
  } else if (0 == direction.compare("inactive")) {
    return RTCRtpTransceiverDirection::kInactive;
  }
  return RTCRtpTransceiverDirection::kInactive;
}

libwebrtc::scoped_refptr<libwebrtc::RTCRtpEncodingParameters>
FlutterPeerConnection::mapToEncoding(const FlValue *params) {
  libwebrtc::scoped_refptr<libwebrtc::RTCRtpEncodingParameters> encoding =
      RTCRtpEncodingParameters::Create();

  encoding->set_active(true);
  encoding->set_scale_resolution_down_by(1.0);

  FlValue *value = findEncodableValue(params, "active");
  if (value) {
    encoding->set_active(fl_value_get_bool(value));
  }

  value = findEncodableValue(params, "rid");
  if (value) {
    const std::string rid = fl_value_get_string(value);
    encoding->set_rid(rid.c_str());
  }

  value = findEncodableValue(params, "ssrc");
  if (value) {
    encoding->set_ssrc((uint32_t)fl_value_get_int(value));
  }

  value = findEncodableValue(params, "minBitrate");
  if (value) {
    encoding->set_min_bitrate_bps(fl_value_get_int(value));
  }

  value = findEncodableValue(params, "maxBitrate");
  if (value) {
    encoding->set_max_bitrate_bps(fl_value_get_int(value));
  }

  value = findEncodableValue(params, "maxFramerate");
  if (value) {
    encoding->set_max_framerate(fl_value_get_int(value));
  }

  value = findEncodableValue(params, "numTemporalLayers");
  if (value) {
    encoding->set_num_temporal_layers(fl_value_get_int(value));
  }

  value = findEncodableValue(params, "scaleResolutionDownBy");
  if (value) {
    encoding->set_scale_resolution_down_by(fl_value_get_int(value));
  }

  return encoding;
}

RTCMediaType stringToMediaType(const std::string& mediaType) {
  RTCMediaType type = RTCMediaType::ANY;
  if (mediaType == "audio")
    type = RTCMediaType::AUDIO;
  else if (mediaType == "video")
    type = RTCMediaType::VIDEO;
  return type;
}

void FlutterPeerConnection::AddTransceiver(
    RTCPeerConnection* pc,
    const std::string& trackId,
    const std::string& mediaType,
    const FlValue *transceiverInit,
    FlMethodCall *resulte) {
  RTCMediaTrack* track = base_->MediaTrackForId(trackId);
  RTCMediaType type = stringToMediaType(mediaType);

  FlValue *transceiverInitV = const_cast<FlValue *>(transceiverInit);
  if (0 < fl_value_get_length(transceiverInitV)) {
    auto transceiver =
        track != nullptr ? pc->AddTransceiver(
                               track, mapToRtpTransceiverInit(transceiverInit))
                         : pc->AddTransceiver(
                               type, mapToRtpTransceiverInit(transceiverInit));
    if (nullptr != transceiver.get()) {
      g_autoptr(FlValue) param = transceiverToMap(transceiver);
      fl_method_call_respond_success(resulte, param, nullptr);
      return;
    }
    fl_method_call_respond_error(resulte,
                                 "AddTransceiver(track | mediaType, init)",
                                 "AddTransceiver error", nullptr, nullptr);
  } else {
    auto transceiver =
        track != nullptr ? pc->AddTransceiver(track) : pc->AddTransceiver(type);
    if (nullptr != transceiver.get()) {
      g_autoptr(FlValue) param = transceiverToMap(transceiver);
      fl_method_call_respond_success(resulte, param, nullptr);
      return;
    }
    fl_method_call_respond_error(resulte, "AddTransceiver(track, mediaType)",
                                 "AddTransceiver error", nullptr, nullptr);
  }
}

void FlutterPeerConnection::GetTransceivers(
    RTCPeerConnection* pc,
    FlMethodCall *resulte) {
  g_autoptr(FlValue) map = fl_value_new_map();

  FlValue *info = fl_value_new_list();
  auto transceivers = pc->transceivers();
  for (scoped_refptr<RTCRtpTransceiver> transceiver :
       transceivers.std_vector()) {
    fl_value_append_take(info, transceiverToMap(transceiver));
  }

  fl_value_set_string_take(map, "transceivers", info);
  fl_method_call_respond_success(resulte, map, nullptr);
}

void FlutterPeerConnection::GetReceivers(
    RTCPeerConnection* pc,
    FlMethodCall *resulte) {
  g_autoptr(FlValue) map = fl_value_new_map();

  FlValue *info = fl_value_new_list();
  auto receivers = pc->receivers();
  for (scoped_refptr<RTCRtpReceiver> receiver : receivers.std_vector()) {
    fl_value_append_take(info, rtpReceiverToMap(receiver));
  }

  fl_value_set_string_take(map, "receivers", info);
  fl_method_call_respond_success(resulte, map, nullptr);
}

void FlutterPeerConnection::RtpSenderDispose(
    RTCPeerConnection* pc,
    std::string rtpSenderId,
    FlMethodCall *resulte) {
  auto sender = GetRtpSenderById(pc, rtpSenderId);
  if (nullptr == sender.get()) {
    fl_method_call_respond_error(resulte, "rtpSenderDispose", "sender is null", nullptr, nullptr);
    return;
  }
  // TODO RtpSenderDispose
  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::RtpSenderSetTrack(
    RTCPeerConnection* pc,
    RTCMediaTrack* track,
    std::string rtpSenderId,
    FlMethodCall *resulte) {
  auto sender = GetRtpSenderById(pc, rtpSenderId);
  if (nullptr == sender.get()) {
    fl_method_call_respond_error(resulte, "rtpSenderDispose", "sender is null", nullptr, nullptr);
    return;
  }
  sender->set_track(track);
  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::RtpSenderReplaceTrack(
    RTCPeerConnection* pc,
    RTCMediaTrack* track,
    std::string rtpSenderId,
    FlMethodCall *resulte) {
  auto sender = GetRtpSenderById(pc, rtpSenderId);
  if (nullptr == sender.get()) {
    fl_method_call_respond_error(resulte, "rtpSenderDispose", "sender is null", nullptr, nullptr);
    return;
  }

  sender->set_track(track);
  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

scoped_refptr<RTCRtpParameters> FlutterPeerConnection::updateRtpParameters(
    const FlValue *newParameters,
    scoped_refptr<RTCRtpParameters> parameters) {
  FlValue *encodings = findList(newParameters, "encodings");
  size_t encodings_size = fl_value_get_length(encodings);
  size_t i = 0;

  auto params = parameters->encodings();
  for (auto param : params.std_vector()) {
    if (i < encodings_size) {
      FlValue *map = fl_value_get_list_value(encodings, i);
      FlValue *value = findEncodableValue(map, "active");
      if (value) {
        param->set_active(fl_value_get_bool(value));
      }

      value = findEncodableValue(map, "maxBitrate");
      if (value) {
        param->set_max_bitrate_bps(fl_value_get_int(value));
      }

      value = findEncodableValue(map, "minBitrate");
      if (value) {
        param->set_min_bitrate_bps(fl_value_get_int(value));
      }

      value = findEncodableValue(map, "maxFramerate");
      if (value) {
        param->set_max_framerate(fl_value_get_int(value));
      }
      value = findEncodableValue(map, "numTemporalLayers");
      if (value) {
        param->set_num_temporal_layers(fl_value_get_int(value));
      }
      value = findEncodableValue(map, "scaleResolutionDownBy");
      if (value) {
        param->set_scale_resolution_down_by(fl_value_get_int(value));
      }

      i++;
    }
  }

  return parameters;
}

void FlutterPeerConnection::RtpSenderSetParameters(
    RTCPeerConnection* pc,
    std::string rtpSenderId,
    const FlValue *parameters,
    FlMethodCall *resulte) {
  auto sender = GetRtpSenderById(pc, rtpSenderId);
  if (nullptr == sender.get()) {
    fl_method_call_respond_error(resulte, "rtpSenderDispose", "sender is null", nullptr, nullptr);
    return;
  }

  auto param = sender->parameters();
  param = updateRtpParameters(parameters, param);
  sender->set_parameters(param);
  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::RtpTransceiverStop(
    RTCPeerConnection* pc,
    std::string rtpTransceiverId,
    FlMethodCall *resulte) {
  auto transceiver = getRtpTransceiverById(pc, rtpTransceiverId);
  if (nullptr == transceiver.get()) {
    fl_method_call_respond_error(resulte, "rtpTransceiverStop", "transceiver is null", nullptr, nullptr);
    return;
  }
  transceiver->StopInternal();
  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::RtpTransceiverGetCurrentDirection(
    RTCPeerConnection* pc,
    std::string rtpTransceiverId,
    FlMethodCall *resulte) {
  auto transceiver = getRtpTransceiverById(pc, rtpTransceiverId);
  if (nullptr == transceiver.get()) {
    fl_method_call_respond_error(resulte, "rtpTransceiverGetCurrentDirection",
                      "transceiver is null", nullptr, nullptr);
    return;
  }
  g_autoptr(FlValue) map = fl_value_new_map();
  fl_value_set_string_take(map, "result",
                           fl_value_new_string(transceiverDirectionString(
                               transceiver->direction()).c_str()));
  fl_method_call_respond_success(resulte, map, nullptr);
}

void FlutterPeerConnection::SetConfiguration(
    RTCPeerConnection* pc,
    const FlValue *configuration,
    FlMethodCall *resulte) {
  // TODO pc->SetConfiguration();

  fl_method_call_respond_not_implemented(resulte, nullptr);
}

void FlutterPeerConnection::CaptureFrame(
    RTCVideoTrack* track,
    std::string path,
    FlMethodCall *resulte) {
  // TODO pc->CaptureFrame();

  fl_method_call_respond_not_implemented(resulte, nullptr);
}

scoped_refptr<RTCRtpTransceiver> FlutterPeerConnection::getRtpTransceiverById(
    RTCPeerConnection* pc,
    std::string id) {
  scoped_refptr<RTCRtpTransceiver> result;
  auto transceivers = pc->transceivers();
  for (scoped_refptr<RTCRtpTransceiver> transceiver :
       transceivers.std_vector()) {
    std::string mid = transceiver->mid().std_string();
    if (nullptr == result.get() && 0 == id.compare(mid)) {
      result = transceiver;
    }
  }
  return result;
}

void FlutterPeerConnection::RtpTransceiverSetDirection(
    RTCPeerConnection* pc,
    std::string rtpTransceiverId,
    std::string direction,
    FlMethodCall *resulte) {
  auto transceiver = getRtpTransceiverById(pc, rtpTransceiverId);
  if (nullptr == transceiver.get()) {
    fl_method_call_respond_error(resulte, "RtpTransceiverSetDirection", " transceiver is null", nullptr, nullptr);
    return;
  }
  auto result = transceiver->SetDirectionWithError(
      stringToTransceiverDirection(direction));
  if (result.std_string() == "") {
    fl_method_call_respond_success(resulte, nullptr, nullptr);
  } else {
    fl_method_call_respond_error(resulte, "RtpTransceiverSetDirection",
                                 result.c_string(), nullptr, nullptr);
  }
}

void FlutterPeerConnection::GetSenders(
    RTCPeerConnection* pc,
    FlMethodCall *resulte) {
  g_autoptr(FlValue) map = fl_value_new_map();

  FlValue *info = fl_value_new_list();
  auto senders = pc->senders();
  for (scoped_refptr<RTCRtpSender> sender : senders.std_vector()) {
    fl_value_append_take(info, rtpSenderToMap(sender));
  }
  fl_value_set_string_take(map, "senders", info);

  fl_method_call_respond_success(resulte, map, nullptr);
}

void FlutterPeerConnection::AddIceCandidate(
    RTCIceCandidate* candidate,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  pc->AddCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(),
                   candidate->candidate());
  fl_method_call_respond_success(result, nullptr, nullptr);
}

void FlutterPeerConnection::GetStats(
    const std::string& track_id,
    RTCPeerConnection* pc,
    FlMethodCall *result) {
  // TODO
  fl_method_call_respond_not_implemented(result, nullptr);
}

void FlutterPeerConnection::MediaStreamAddTrack(
    scoped_refptr<RTCMediaStream> stream,
    scoped_refptr<RTCMediaTrack> track,
    FlMethodCall *resulte) {
  std::string kind = track->kind().std_string();
  if (0 == kind.compare("audio")) {
    stream->AddTrack(static_cast<RTCAudioTrack*>(track.get()));
  } else if (0 == kind.compare("video")) {
    stream->AddTrack(static_cast<RTCVideoTrack*>(track.get()));
  }

  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::MediaStreamRemoveTrack(
    scoped_refptr<RTCMediaStream> stream,
    scoped_refptr<RTCMediaTrack> track,
    FlMethodCall *resulte) {
  std::string kind = track->kind().std_string();
  if (0 == kind.compare("audio")) {
    stream->RemoveTrack(static_cast<RTCAudioTrack*>(track.get()));
  } else if (0 == kind.compare("video")) {
    stream->RemoveTrack(static_cast<RTCVideoTrack*>(track.get()));
  }

  fl_method_call_respond_success(resulte, nullptr, nullptr);
}

void FlutterPeerConnection::AddTrack(
    RTCPeerConnection* pc,
    scoped_refptr<RTCMediaTrack> track,
    std::list<std::string> streamIds,
    FlMethodCall *result) {
  std::string kind = track->kind().std_string();
  std::vector<string> streamids;
  for (std::string item : streamIds) {
    streamids.push_back(item.c_str());
  }
  if (0 == kind.compare("audio")) {
    auto sender = pc->AddTrack((RTCAudioTrack*)track.get(), streamids);
    if (sender.get() != nullptr) {
      g_autoptr(FlValue) param = rtpSenderToMap(sender);
      fl_method_call_respond_success(result, param, nullptr);
      return;
    }
  } else if (0 == kind.compare("video")) {
    auto sender = pc->AddTrack((RTCVideoTrack*)track.get(), streamids);
    if (sender.get() != nullptr) {
      g_autoptr(FlValue) param = rtpSenderToMap(sender);
      fl_method_call_respond_success(result, param, nullptr);
      return;
    }
  }

  fl_method_call_respond_success(result, nullptr, nullptr);
}

libwebrtc::scoped_refptr<libwebrtc::RTCRtpSender>
FlutterPeerConnection::GetRtpSenderById(RTCPeerConnection* pc, std::string id) {
  libwebrtc::scoped_refptr<libwebrtc::RTCRtpSender> result;
  auto senders = pc->senders();
  for (scoped_refptr<RTCRtpSender> item : senders.std_vector()) {
    std::string itemId = item->id().std_string();
    if (nullptr == result.get() && 0 == id.compare(itemId)) {
      result = item;
    }
  }
  return result;
}

void FlutterPeerConnection::RemoveTrack(
    RTCPeerConnection* pc,
    std::string senderId,
    FlMethodCall *result) {
  auto sender = GetRtpSenderById(pc, senderId);
  if (nullptr == sender.get()) {
    fl_method_call_respond_error(result, "RemoveTrack", "not find RtpSender", nullptr, nullptr);
    return;
  }

  g_autoptr(FlValue) map = fl_value_new_map();
  bool res = pc->RemoveTrack(sender);
  fl_value_set_string_take(map, "result", fl_value_new_bool(res));
  fl_method_call_respond_success(result, map, nullptr);
}

FlMethodErrorResponse*
FlutterPeerConnectionObserver::FlEventChannelListenCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterPeerConnectionObserver *thiz = reinterpret_cast<FlutterPeerConnectionObserver *>(user_data);
  thiz->event_sink_ = true;
  return nullptr;
}

FlMethodErrorResponse*
FlutterPeerConnectionObserver::FlEventChannelCancelCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterPeerConnectionObserver *thiz = reinterpret_cast<FlutterPeerConnectionObserver *>(user_data);
  thiz->event_sink_ = false;
  return nullptr;
}

FlutterPeerConnectionObserver::FlutterPeerConnectionObserver(
    FlutterWebRTCBase* base,
    scoped_refptr<RTCPeerConnection> peerconnection,
    FlBinaryMessenger* messenger,
    const std::string& channel_name,
    std::string& peerConnectionId)
    : peerconnection_(peerconnection),
      base_(base),
      id_(peerConnectionId) {

  event_sink_ = false;
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  event_channel_ = fl_event_channel_new(messenger, channel_name.c_str(), FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(event_channel_, FlEventChannelListenCB, FlEventChannelCancelCB, this, nullptr);

  peerconnection->RegisterRTCPeerConnectionObserver(this);
}

static const char* iceConnectionStateString(RTCIceConnectionState state) {
  switch (state) {
    case RTCIceConnectionStateNew:
      return "new";
    case RTCIceConnectionStateChecking:
      return "checking";
    case RTCIceConnectionStateConnected:
      return "connected";
    case RTCIceConnectionStateCompleted:
      return "completed";
    case RTCIceConnectionStateFailed:
      return "failed";
    case RTCIceConnectionStateDisconnected:
      return "disconnected";
    case RTCIceConnectionStateClosed:
      return "closed";
    case RTCIceConnectionStateMax:
      return "statemax";
  }
  return "";
}

static const char* signalingStateString(RTCSignalingState state) {
  switch (state) {
    case RTCSignalingStateStable:
      return "stable";
    case RTCSignalingStateHaveLocalOffer:
      return "have-local-offer";
    case RTCSignalingStateHaveLocalPrAnswer:
      return "have-local-pranswer";
    case RTCSignalingStateHaveRemoteOffer:
      return "have-remote-offer";
    case RTCSignalingStateHaveRemotePrAnswer:
      return "have-remote-pranswer";
    case RTCSignalingStateClosed:
      return "closed";
  }
  return "";
}
void FlutterPeerConnectionObserver::OnSignalingState(RTCSignalingState state) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("signalingState"));
    fl_value_set_string_take(params, "state", fl_value_new_string(signalingStateString(state)));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

static const char* iceGatheringStateString(RTCIceGatheringState state) {
  switch (state) {
    case RTCIceGatheringStateNew:
      return "new";
    case RTCIceGatheringStateGathering:
      return "gathering";
    case RTCIceGatheringStateComplete:
      return "complete";
  }
  return "";
}

void FlutterPeerConnectionObserver::OnIceGatheringState(
    RTCIceGatheringState state) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("iceGatheringState"));
    fl_value_set_string_take(params, "state", fl_value_new_string(iceGatheringStateString(state)));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnIceConnectionState(
    RTCIceConnectionState state) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("iceConnectionState"));
    fl_value_set_string_take(params, "state", fl_value_new_string(iceConnectionStateString(state)));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnIceCandidate(
    scoped_refptr<RTCIceCandidate> candidate) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("onCandidate"));

    FlValue *cand = fl_value_new_map();
    fl_value_set_string_take(cand, "candidate", fl_value_new_string(candidate->candidate().c_string()));
    fl_value_set_string_take(cand, "sdpMLineIndex", fl_value_new_int(candidate->sdp_mline_index()));
    fl_value_set_string_take(cand, "sdpMid", fl_value_new_string(candidate->sdp_mid().c_string()));

    fl_value_set_string_take(params, "candidate", cand);
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnAddStream(
    scoped_refptr<RTCMediaStream> stream) {
  std::string streamId = stream->id().std_string();

  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("onAddStream"));
    fl_value_set_string_take(params, "streamId", fl_value_new_string(streamId.c_str()));

    FlValue *audioTracks = fl_value_new_list();
    auto audio_tracks = stream->audio_tracks();
    for (scoped_refptr<RTCAudioTrack> track : audio_tracks.std_vector()) {
      FlValue *audioTrack = fl_value_new_map();
      fl_value_set_string_take(audioTrack, "id", fl_value_new_string(track->id().c_string()));
      fl_value_set_string_take(audioTrack, "label", fl_value_new_string(track->id().c_string()));
      fl_value_set_string_take(audioTrack, "kind", fl_value_new_string(track->kind().c_string()));
      fl_value_set_string_take(audioTrack, "enabled", fl_value_new_bool(track->enabled()));
      fl_value_set_string_take(audioTrack, "remote", fl_value_new_bool(true));
      fl_value_set_string_take(audioTrack, "readyState", fl_value_new_string("live"));

      fl_value_append_take(audioTracks, audioTrack);
    }
    fl_value_set_string_take(params, "audioTracks", audioTracks);

    FlValue *videoTracks = fl_value_new_list();
    auto video_tracks = stream->video_tracks();
    for (scoped_refptr<RTCVideoTrack> track : video_tracks.std_vector()) {
      FlValue *videoTrack = fl_value_new_map();

      fl_value_set_string_take(videoTrack, "id", fl_value_new_string(track->id().c_string()));
      fl_value_set_string_take(videoTrack, "label", fl_value_new_string(track->id().c_string()));
      fl_value_set_string_take(videoTrack, "kind", fl_value_new_string(track->kind().c_string()));
      fl_value_set_string_take(videoTrack, "enabled", fl_value_new_bool(track->enabled()));
      fl_value_set_string_take(videoTrack, "remote", fl_value_new_bool(true));
      fl_value_set_string_take(videoTrack, "readyState", fl_value_new_string("live"));

      fl_value_append_take(videoTracks, videoTrack);
    }
    remote_streams_[streamId] = scoped_refptr<RTCMediaStream>(stream);
    fl_value_set_string_take(params,"videoTracks", videoTracks);
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnRemoveStream(
    scoped_refptr<RTCMediaStream> stream) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("onRemoveStream"));
    fl_value_set_string_take(params, "streamId", fl_value_new_string(stream->label().c_string()));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
  RemoveStreamForId(stream->label().std_string());
}

void FlutterPeerConnectionObserver::OnAddTrack(
    vector<scoped_refptr<RTCMediaStream>> streams,
    scoped_refptr<RTCRtpReceiver> receiver) {
  auto track = receiver->track();

  std::vector<scoped_refptr<RTCMediaStream>> mediaStreams;
  for (scoped_refptr<RTCMediaStream> stream : streams.std_vector()) {
    mediaStreams.push_back(stream);

    if (event_sink_) {
      g_autoptr(FlValue) params = fl_value_new_map();
      fl_value_set_string_take(params, "event",
                               fl_value_new_string("onAddTrack"));
      fl_value_set_string_take(params, "streamId", fl_value_new_string(stream->label().c_string()));
      fl_value_set_string_take(params, "trackId", fl_value_new_string(track->id().c_string()));
        FlValue *audioTrack = fl_value_new_map();
        fl_value_set_string_take(audioTrack, "id", fl_value_new_string(track->id().c_string()));
        fl_value_set_string_take(audioTrack, "label", fl_value_new_string(track->id().c_string()));
        fl_value_set_string_take(audioTrack, "kind", fl_value_new_string(track->kind().c_string()));
        fl_value_set_string_take(audioTrack, "enabled", fl_value_new_bool(track->enabled()));
        fl_value_set_string_take(audioTrack, "remote", fl_value_new_bool(true));
        fl_value_set_string_take(audioTrack, "readyState", fl_value_new_string("live"));
      fl_value_set_string_take(params, "track", audioTrack);

      fl_event_channel_send(event_channel_, params, nullptr, nullptr);
    }
  }
}

void FlutterPeerConnectionObserver::OnTrack(
    scoped_refptr<RTCRtpTransceiver> transceiver) {
  if (event_sink_) {
    auto receiver = transceiver->receiver();
    g_autoptr(FlValue) params = fl_value_new_map();
      FlValue *streams_info = fl_value_new_list();
      auto streams = receiver->streams();
      for (scoped_refptr<RTCMediaStream> item : streams.std_vector()) {
        fl_value_append_take(streams_info, mediaStreamToMap(item, id_));
      }
    fl_value_set_string_take(params, "event", fl_value_new_string("onTrack"));
    fl_value_set_string_take(params, "streams", streams_info);
    fl_value_set_string_take(params, "track", mediaTrackToMap(receiver->track()));
    fl_value_set_string_take(params, "receiver", rtpReceiverToMap(receiver));
    fl_value_set_string_take(params, "transceiver", transceiverToMap(transceiver));

    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnRemoveTrack(
    scoped_refptr<RTCRtpReceiver> receiver) {
    auto track = receiver->track();
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("onRemoveTrack"));
    fl_value_set_string_take(params, "trackId", fl_value_new_string(track->id().c_string()));
    fl_value_set_string_take(params, "track", mediaTrackToMap(track));
    fl_value_set_string_take(params, "receiver", rtpReceiverToMap(receiver));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

// void FlutterPeerConnectionObserver::OnRemoveTrack(
//    scoped_refptr<RTCMediaStream> stream,
//    scoped_refptr<RTCMediaTrack> track) {
//  if (event_sink_ != nullptr) {
//    EncodableMap params;
//    params[EncodableValue("event")] = "onRemoveTrack";
//    params[EncodableValue("streamId")] = stream->label();
//    params[EncodableValue("trackId")] = track->id();
//
//    EncodableMap videoTrack;
//    videoTrack[EncodableValue("id")] = track->id();
//    videoTrack[EncodableValue("label")] = track->id();
//    videoTrack[EncodableValue("kind")] = track->kind();
//    videoTrack[EncodableValue("enabled")] = track->enabled();
//    videoTrack[EncodableValue("remote")] = true;
//    videoTrack[EncodableValue("readyState")] = "live";
//    params[EncodableValue("track")] = videoTrack;
//
//    event_sink_->Success(EncodableValue(params));
//  }
//}

void FlutterPeerConnectionObserver::OnDataChannel(
    scoped_refptr<RTCDataChannel> data_channel) {

  int channel_id = data_channel->id();

  base_->lock();
  if (base_->data_channel_observers_.find(channel_id) !=
      base_->data_channel_observers_.end()) {
    for(int i = 1024; i < 65535; i++){
      if(base_->data_channel_observers_.find(i) ==
      base_->data_channel_observers_.end()){
        channel_id = i;
        break;
      }
    }
  }
   base_->unlock();

  std::string event_channel =
      "FlutterWebRTC/dataChannelEvent" + id_ + std::to_string(channel_id);

  std::unique_ptr<FlutterRTCDataChannelObserver> observer(
      new FlutterRTCDataChannelObserver(data_channel, base_->messenger_,
                                        event_channel));

  base_->lock();
  base_->data_channel_observers_[channel_id] = std::move(observer);
  base_->unlock();

  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("didOpenDataChannel"));
    fl_value_set_string_take(params, "id", fl_value_new_int(channel_id));
    fl_value_set_string_take(params, "label", fl_value_new_string(data_channel->label().c_string()));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

void FlutterPeerConnectionObserver::OnRenegotiationNeeded() {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("onRenegotiationNeeded"));
    fl_event_channel_send(event_channel_, params, nullptr, nullptr);
  }
}

scoped_refptr<RTCMediaStream> FlutterPeerConnectionObserver::MediaStreamForId(
    const std::string& id) {
  auto it = remote_streams_.find(id);
  if (it != remote_streams_.end())
    return (*it).second;
  return nullptr;
}

scoped_refptr<RTCMediaTrack> FlutterPeerConnectionObserver::MediaTrackForId(const std::string& id) {
    for (auto it = remote_streams_.begin(); it != remote_streams_.end(); it++)
    {
        auto remoteStream = (*it).second;
        auto audio_tracks = remoteStream->audio_tracks();
        for (auto track : audio_tracks.std_vector()) {
            if (track->id().std_string() == id) {
                return track;
            }
        }
        auto video_tracks = remoteStream->video_tracks();
        for (auto track : video_tracks.std_vector()) {
            if (track->id().std_string() == id) {
                return track;
            }
        }
    }
    return nullptr;
}

void FlutterPeerConnectionObserver::RemoveStreamForId(const std::string& id) {
  auto it = remote_streams_.find(id);
  if (it != remote_streams_.end())
    remote_streams_.erase(it);
}

}  // namespace flutter_webrtc_plugin
