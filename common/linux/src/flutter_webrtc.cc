#include "flutter_webrtc.h"
#include "flutter_glog.h"

#include "flutter_webrtc/flutter_web_r_t_c_plugin.h"

namespace flutter_webrtc_plugin {

FlutterWebRTC::FlutterWebRTC(FlutterWebRTCPlugin* plugin)
    : FlutterWebRTCBase::FlutterWebRTCBase(plugin->messenger(),
                                           plugin->textures()),
      FlutterVideoRendererManager::FlutterVideoRendererManager(this),
      FlutterMediaStream::FlutterMediaStream(this),
      FlutterPeerConnection::FlutterPeerConnection(this),
      FlutterDataChannel::FlutterDataChannel(this) {}

FlutterWebRTC::~FlutterWebRTC() {}

void FlutterWebRTC::HandleMethodCall(FlMethodCall *method_call) {
  std::string method_name = fl_method_call_get_name(method_call);
  FlValue *params = fl_method_call_get_args(method_call);
  FL_LOGI("calling [%s] %s\n", method_name.c_str(), fl_value_to_string(params));
  if (method_name.compare("createPeerConnection") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call, "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const FlValue *configuration = fl_value_lookup_string(params, "configuration");
    const FlValue *constraints = findMap(params, "constraints");
    CreateRTCPeerConnection(configuration, constraints, method_call);
  } else if (method_name.compare("getUserMedia") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call, "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }
    const FlValue *constraints = fl_value_lookup_string(params, "constraints");
    GetUserMedia(constraints, method_call);
  } else if (method_name.compare("getDisplayMedia") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call, "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (method_name.compare("getSources") == 0) {
    GetSources(method_call);
  } else if (method_name.compare("mediaStreamGetTracks") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string streamId = findString(params, "streamId");
    MediaStreamGetTracks(streamId, method_call);
  } else if (method_name.compare("createOffer") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *constraints = findMap(params, "constraints");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "createOfferFailed", "createOffer() peerConnection is null", nullptr, nullptr);
      return;
    }
    CreateOffer(constraints, pc, method_call);
  } else if (method_name.compare("createAnswer") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *constraints = findMap(params, "constraints");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "createAnswerFailed", "createAnswer() peerConnection is null", nullptr, nullptr);
      return;
    }
    CreateAnswer(constraints, pc, method_call);
  } else if (method_name.compare("addStream") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string streamId = findString(params, "streamId");
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    scoped_refptr<RTCMediaStream> stream = MediaStreamForId(streamId);
    if (!stream) {
      fl_method_call_respond_error(method_call,
          "addStreamFailed", "addStream() stream not found!", nullptr, nullptr);
      return;
    }
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "addStreamFailed", "addStream() peerConnection is null", nullptr, nullptr);
      return;
    }
    pc->AddStream(stream);
    fl_method_call_respond_success(method_call, nullptr, nullptr);
  } else if (method_name.compare("removeStream") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string streamId = findString(params, "streamId");
    const std::string peerConnectionId = findString(params, "peerConnectionId");

    scoped_refptr<RTCMediaStream> stream = MediaStreamForId(streamId);
    if (!stream) {
      fl_method_call_respond_error(method_call,
          "removeStreamFailed", "removeStream() stream not found!", nullptr, nullptr);
      return;
    }
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "removeStreamFailed", "removeStream() peerConnection is null", nullptr, nullptr);
      return;
    }
    pc->RemoveStream(stream);
    fl_method_call_respond_success(method_call, nullptr, nullptr);
  } else if (method_name.compare("setLocalDescription") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null arguments received", nullptr, nullptr);
      return;
    }
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *constraints = findMap(params, "description");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "setLocalDescriptionFailed", "setLocalDescription() peerConnection is null", nullptr, nullptr);
      return;
    }

    SdpParseError error;
    scoped_refptr<RTCSessionDescription> description =
        RTCSessionDescription::Create(findString(constraints, "type").c_str(),
                                      findString(constraints, "sdp").c_str(),
                                      &error);

    SetLocalDescription(description.get(), pc, method_call);
  } else if (method_name.compare("setRemoteDescription") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *constraints = findMap(params, "description");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "setRemoteDescriptionFailed", "setRemoteDescription() peerConnection is null", nullptr, nullptr);
      return;
    }

    SdpParseError error;
    scoped_refptr<RTCSessionDescription> description =
        RTCSessionDescription::Create(findString(constraints, "type").c_str(),
                                      findString(constraints, "sdp").c_str(),
                                      &error);

    SetRemoteDescription(description.get(), pc, method_call);
  } else if (method_name.compare("addCandidate") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }
    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *constraints = findMap(params, "candidate");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "addCandidateFailed", "addCandidate() peerConnection is null", nullptr, nullptr);
      return;
    }

    SdpParseError error;
    scoped_refptr<RTCIceCandidate> rtc_candidate =
        RTCIceCandidate::Create(findString(constraints, "candidate").c_str(),
                                findString(constraints, "sdpMid").c_str(),
                                findInt(constraints, "sdpMLineIndex"), &error);

    AddIceCandidate(rtc_candidate.get(), pc, method_call);
  } else if (method_name.compare("getStats") == 0) {
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (method_name.compare("createDataChannel") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "createDataChannelFailed", "createDataChannel() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string label = findString(params, "label");
    const FlValue *dataChannelDict = findMap(params, "dataChannelDict");
    CreateDataChannel(peerConnectionId, label, dataChannelDict, pc, method_call);
  } else if (method_name.compare("dataChannelSend") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "dataChannelSendFailed", "dataChannelSend() peerConnection is null", nullptr, nullptr);
      return;
    }

    int dataChannelId = findInt(params, "dataChannelId");
    const std::string type = findString(params, "type");
    const FlValue *data = findEncodableValue(params, "data");
    RTCDataChannel* data_channel = DataChannelFormId(dataChannelId);
    if (data_channel == nullptr) {
      fl_method_call_respond_error(method_call,
          "dataChannelSendFailed", "dataChannelSend() data_channel is null", nullptr, nullptr);
      return;
    }
    DataChannelSend(data_channel, type, data, method_call);
  } else if (method_name.compare("dataChannelClose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "dataChannelCloseFailed", "dataChannelClose() peerConnection is null", nullptr, nullptr);
      return;
    }

    int dataChannelId = findInt(params, "dataChannelId");
    RTCDataChannel* data_channel = DataChannelFormId(dataChannelId);
    if (data_channel == nullptr) {
      fl_method_call_respond_error(method_call,
          "dataChannelCloseFailed", "dataChannelClose() data_channel is null", nullptr, nullptr);
      return;
    }
    DataChannelClose(data_channel, method_call);
  } else if (method_name.compare("streamDispose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string stream_id = findString(params, "streamId");
    MediaStreamDispose(stream_id, method_call);
  } else if (method_name.compare("mediaStreamTrackSetEnable") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string track_id = findString(params, "trackId");
    FlValue *enable = findEncodableValue(params, "enabled");
    RTCMediaTrack* track = MediaTrackForId(track_id);
    if (nullptr == track) {
      fl_method_call_respond_error(method_call,
          "mediaStreamTrackSetEnableFailed", "mediaStreamTrackSetEnable() track is null", nullptr, nullptr);
      return;
    }
    track->set_enabled(fl_value_get_bool(enable));
    fl_method_call_respond_success(method_call, nullptr, nullptr);
  } else if (method_name.compare("trackDispose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string track_id = findString(params, "trackId");
    MediaStreamTrackDispose(track_id, method_call);
  } else if (method_name.compare("restartIce") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "restartIceFailed", "restartIce() peerConnection is null", nullptr, nullptr);
      return;
    }
    pc->RestartIce();
    fl_method_call_respond_success(method_call, nullptr, nullptr);
  } else if (method_name.compare("peerConnectionClose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "peerConnectionCloseFailed", "peerConnectionClose() peerConnection is null", nullptr, nullptr);
      return;
    }
    RTCPeerConnectionClose(pc, peerConnectionId, method_call);
  } else if (method_name.compare("peerConnectionDispose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "peerConnectionDisposeFailed", "peerConnectionDisposeClose() peerConnection is null", nullptr, nullptr);
      return;
    }
    RTCPeerConnectionDispose(pc, peerConnectionId, method_call);
  } else if (method_name.compare("createVideoRenderer") == 0) {
    CreateVideoRendererTexture(method_call);
  } else if (method_name.compare("videoRendererDispose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    int64_t texture_id = findLongInt(params, "textureId");
    VideoRendererDispose(texture_id, method_call);
  } else if (method_name.compare("videoRendererSetSrcObject") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string stream_id = findString(params, "streamId");
    int64_t texture_id = findLongInt(params, "textureId");
    SetMediaStream(texture_id, stream_id);
    fl_method_call_respond_success(method_call, nullptr, nullptr);
  } else if (method_name.compare("mediaStreamTrackSwitchCamera") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string track_id = findString(params, "trackId");
    MediaStreamTrackSwitchCamera(track_id, method_call);
  } else if (method_name.compare("setVolume") == 0) {
    fl_method_call_respond_not_implemented(method_call, nullptr);
  } else if (method_name.compare("getLocalDescription") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    //const FlValue *constraints = findMap(params, "description");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "GetLocalDescription", "GetLocalDescription() peerConnection is null", nullptr, nullptr);
      return;
    }

    GetLocalDescription(pc, method_call);
  } else if (method_name.compare("getRemoteDescription") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    //const FlValue *constraints = findMap(params, "description");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
            "GetRemoteDescription", "GetRemoteDescription() peerConnection is null", nullptr, nullptr);
      return;
    }

    GetRemoteDescription(pc, method_call);
  } else if (method_name.compare("mediaStreamAddTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string streamId = findString(params, "streamId");
    const std::string trackId = findString(params, "trackId");
    scoped_refptr<RTCMediaStream> stream = MediaStreamForId(streamId);
    if (stream == nullptr) {
      fl_method_call_respond_error(method_call,
          "MediaStreamAddTrack", "MediaStreamAddTrack() stream is null", nullptr, nullptr);
      return;
    }

    scoped_refptr<RTCMediaTrack> track = MediaTracksForId(trackId);
    if (track == nullptr) {
      fl_method_call_respond_error(method_call,
          "MediaStreamAddTrack", "MediaStreamAddTrack() track is null", nullptr, nullptr);
      return;
    }

    MediaStreamAddTrack(stream, track, method_call);
    std::string kind = track->kind().std_string();
    for (int i = 0; i < renders_.size(); i++) {
      FlutterVideoRenderer* renderer = renders_.at(i).get();
      if (renderer->CheckMediaStream(streamId) && 0 == kind.compare("video")) {
        renderer->SetVideoTrack(static_cast<RTCVideoTrack*>(track.get()));
      }
    }
  } else if (method_name.compare("mediaStreamRemoveTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string streamId = findString(params, "streamId");
    const std::string trackId = findString(params, "trackId");
    scoped_refptr<RTCMediaStream> stream = MediaStreamForId(streamId);
    if (stream == nullptr) {
      fl_method_call_respond_error(method_call,
          "MediaStreamRemoveTrack", "MediaStreamRemoveTrack() stream is null", nullptr, nullptr);
      return;
    }

    scoped_refptr<RTCMediaTrack> track = MediaTracksForId(trackId);
    if (track == nullptr) {
      fl_method_call_respond_error(method_call,
          "MediaStreamRemoveTrack", "MediaStreamRemoveTrack() track is null", nullptr, nullptr);
      return;
    }

    MediaStreamRemoveTrack(stream, track, method_call);
    for (int i = 0; i < renders_.size(); i++) {
      FlutterVideoRenderer* renderer = renders_.at(i).get();
      if (renderer->CheckVideoTrack(streamId)) {
        renderer->SetVideoTrack(nullptr);
      }
    }
  } else if (method_name.compare("addTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const std::string trackId = findString(params, "trackId");
    FlValue *streamIds = findList(params, "streamIds");

    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "AddTrack", "AddTrack() peerConnection is null", nullptr, nullptr);
      return;
    }

    scoped_refptr<RTCMediaTrack> track = MediaTracksForId(trackId);
    if (track == nullptr) {
      fl_method_call_respond_error(method_call,
          "AddTrack", "AddTrack() track is null", nullptr, nullptr);
      return;
    }

    std::list<std::string> listId;
    size_t size = fl_value_get_length(streamIds);
    for (size_t i = 0; i < size; i++) {
      const gchar *s = fl_value_get_string(fl_value_get_list_value(streamIds, i));
      listId.push_back(s);
    }

    AddTrack(pc, track, listId, method_call);
  } else if (method_name.compare("removeTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const std::string senderId = findString(params, "senderId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "removeTrack", "removeTrack() peerConnection is null", nullptr, nullptr);
      return;
    }

    RemoveTrack(pc, senderId, method_call);
  } else if (method_name.compare("addTransceiver") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    const FlValue *transceiverInit = findMap(params, "transceiverInit");
    const std::string mediaType = findString(params, "mediaType");
    const std::string trackId = findString(params, "trackId");

    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "addTransceiver", "addTransceiver() peerConnection is null", nullptr, nullptr);
      return;
    }
    AddTransceiver(pc, trackId, mediaType, transceiverInit, method_call);
  } else if (method_name.compare("getTransceivers") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "getTransceivers", "getTransceivers() peerConnection is null", nullptr, nullptr);
      return;
    }

    GetTransceivers(pc, method_call);
  } else if (method_name.compare("getReceivers") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "getReceivers", "getReceivers() peerConnection is null", nullptr, nullptr);
      return;
    }

    GetReceivers(pc, method_call);
  } else if (method_name.compare("getSenders") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "getSenders", "getSenders() peerConnection is null", nullptr, nullptr);
      return;
    }

    GetSenders(pc, method_call);
  } else if (method_name.compare("rtpSenderDispose") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "rtpSenderDispose", "rtpSenderDispose() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string rtpSenderId = findString(params, "rtpSenderId");
    if (0 < rtpSenderId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(method_call,
            "rtpSenderDispose", "rtpSenderDispose() rtpSenderId is null or empty", nullptr, nullptr);
        return;
      }
    }
    RtpSenderDispose(pc, rtpSenderId, method_call);
  } else if (method_name.compare("rtpSenderSetTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "rtpSenderSetTrack", "rtpSenderSetTrack() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string trackId = findString(params, "trackId");
    RTCMediaTrack* track = MediaTrackForId(trackId);
    if (nullptr == track) {
      fl_method_call_respond_error(method_call,
          "rtpSenderSetTrack", "rtpSenderSetTrack() track is null", nullptr, nullptr);
      return;
    }

    const std::string rtpSenderId = findString(params, "rtpSenderId");
    if (0 < rtpSenderId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(method_call,
            "rtpSenderSetTrack", "rtpSenderSetTrack() rtpSenderId is null or empty", nullptr, nullptr);
        return;
      }
    }
    RtpSenderSetTrack(pc, track, rtpSenderId, method_call);
  } else if (method_name.compare("rtpSenderReplaceTrack") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "rtpSenderReplaceTrack", "rtpSenderReplaceTrack() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string trackId = findString(params, "trackId");
    RTCMediaTrack* track = MediaTrackForId(trackId);
    if (nullptr == track) {
      fl_method_call_respond_error(method_call,
          "rtpSenderReplaceTrack", "rtpSenderReplaceTrack() track is null", nullptr, nullptr);
      return;
    }

    const std::string rtpSenderId = findString(params, "rtpSenderId");
    if (0 < rtpSenderId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(method_call,
            "rtpSenderReplaceTrack", "rtpSenderReplaceTrack() rtpSenderId is null or empty", nullptr, nullptr);
        return;
      }
    }
    RtpSenderReplaceTrack(pc, track, rtpSenderId, method_call);
  } else if (method_name.compare("rtpSenderSetParameters") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "rtpSenderSetParameters", "rtpSenderSetParameters() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string rtpSenderId = findString(params, "rtpSenderId");
    if (0 < rtpSenderId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(method_call,
            "rtpSenderSetParameters", "rtpSenderSetParameters() rtpSenderId is null or empty", nullptr, nullptr);
        return;
      }
    }

    FlValue *parameters = findMap(params, "parameters");
    if (0 < fl_value_get_length(parameters)) {
      fl_method_call_respond_error(method_call,
          "rtpSenderSetParameters", "rtpSenderSetParameters() parameters is null or empty", nullptr, nullptr);
      return;
    }

    RtpSenderSetParameters(pc, rtpSenderId, parameters, method_call);
  } else if (method_name.compare("rtpTransceiverStop") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call,
          "rtpTransceiverStop", "rtpTransceiverStop() peerConnection is null", nullptr, nullptr);
      return;
    }

    const std::string rtpTransceiverId = findString(params, "rtpTransceiverId");
    if (0 < rtpTransceiverId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(method_call,
            "rtpTransceiverStop", "rtpTransceiverStop() rtpTransceiverId is null or empty", nullptr, nullptr);
        return;
      }
    }

    RtpTransceiverStop(pc, rtpTransceiverId, method_call);
  } else if (method_name.compare("rtpTransceiverGetCurrentDirection") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(
          method_call, "rtpTransceiverGetCurrentDirection",
          "rtpTransceiverGetCurrentDirection() peerConnection is null", nullptr,
          nullptr);
      return;
    }

    const std::string rtpTransceiverId = findString(params, "rtpTransceiverId");
    if (0 < rtpTransceiverId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(
            method_call, "rtpTransceiverGetCurrentDirection",
            "rtpTransceiverGetCurrentDirection() rtpTransceiverId is "
            "null or empty", nullptr, nullptr);
        return;
      }
    }

    RtpTransceiverGetCurrentDirection(pc, rtpTransceiverId, method_call);
  } else if (method_name.compare("rtpTransceiverSetDirection") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call,
          "Bad Arguments", "Null constraints arguments received", nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(
          method_call, "rtpTransceiverSetDirection",
          "rtpTransceiverSetDirection() peerConnection is null", nullptr,
          nullptr);
      return;
    }

    const std::string rtpTransceiverId = findString(params, "rtpTransceiverId");
    if (0 < rtpTransceiverId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(
            method_call, "rtpTransceiverGetCurrentDirection",
            "rtpTransceiverGetCurrentDirection() rtpTransceiverId is "
            "null or empty", nullptr, nullptr);
        return;
      }
    }

    const std::string direction = findString(params, "direction");
    if (0 < rtpTransceiverId.size()) {
      if (pc == nullptr) {
        fl_method_call_respond_error(
            method_call, "rtpTransceiverGetCurrentDirection",
            "rtpTransceiverGetCurrentDirection() direction is null or empty",
            nullptr, nullptr);
        return;
      }
    }

    RtpTransceiverSetDirection(pc, rtpTransceiverId, direction, method_call);
  } else if (method_name.compare("setConfiguration") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call, "Bad Arguments",
                                   "Null constraints arguments received",
                                   nullptr, nullptr);
      return;
    }

    const std::string peerConnectionId = findString(params, "peerConnectionId");
    RTCPeerConnection* pc = PeerConnectionForId(peerConnectionId);
    if (pc == nullptr) {
      fl_method_call_respond_error(method_call, "setConfiguration",
                                   "setConfiguration() peerConnection is null",
                                   nullptr, nullptr);
      return;
    }

    FlValue *configuration = findMap(params, "configuration");
    if (0 < fl_value_get_length(configuration)) {
      if (pc == nullptr) {
        fl_method_call_respond_error(
            method_call, "setConfiguration",
            "setConfiguration() configuration is null or empty", nullptr,
            nullptr);
        return;
      }
    }
    SetConfiguration(pc, configuration, method_call);
  } else if (method_name.compare("captureFrame") == 0) {
    if (!params) {
      fl_method_call_respond_error(method_call, "Bad Arguments",
                                   "Null constraints arguments received",
                                   nullptr, nullptr);
      return;
    }

    const std::string path = findString(params, "path");
    if (0 < path.size()) {
      fl_method_call_respond_error(method_call, "captureFrame",
                                   "captureFrame() path is null or empty",
                                   nullptr, nullptr);
      return;
    }

    const std::string trackId = findString(params, "trackId");
    RTCMediaTrack* track = MediaTrackForId(trackId);
    if (nullptr == track) {
      fl_method_call_respond_error(method_call, "captureFrame",
                                   "captureFrame() track is null", nullptr,
                                   nullptr);
      return;
    }
    std::string kind = track->kind().std_string();
    if (0 != kind.compare("video")) {
      fl_method_call_respond_error(method_call, "captureFrame",
                                   "captureFrame() track not is video track",
                                   nullptr, nullptr);
      return;
    }
    CaptureFrame((RTCVideoTrack*)track, path, method_call);
  } else {
    fl_method_call_respond_not_implemented(method_call, nullptr);
  }
}

}  // namespace flutter_webrtc_plugin
