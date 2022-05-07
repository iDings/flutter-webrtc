#ifndef FLUTTER_WEBRTC_RTC_PEER_CONNECTION_HXX
#define FLUTTER_WEBRTC_RTC_PEER_CONNECTION_HXX

#include "flutter_webrtc_base.h"

namespace flutter_webrtc_plugin {

class FlutterPeerConnectionObserver : public RTCPeerConnectionObserver {
 public:
  FlutterPeerConnectionObserver(FlutterWebRTCBase* base,
                                scoped_refptr<RTCPeerConnection> peerconnection,
                                FlBinaryMessenger* messenger,
                                const std::string& channel_name,
                                std::string& peerConnectionId);

  virtual void OnSignalingState(RTCSignalingState state) override;
  virtual void OnIceGatheringState(RTCIceGatheringState state) override;
  virtual void OnIceConnectionState(RTCIceConnectionState state) override;
  virtual void OnIceCandidate(
      scoped_refptr<RTCIceCandidate> candidate) override;
  virtual void OnAddStream(scoped_refptr<RTCMediaStream> stream) override;
  virtual void OnRemoveStream(scoped_refptr<RTCMediaStream> stream) override;

  virtual void OnTrack(scoped_refptr<RTCRtpTransceiver> transceiver) override;
  virtual void OnAddTrack(vector<scoped_refptr<RTCMediaStream>> streams,
                          scoped_refptr<RTCRtpReceiver> receiver) override;
  virtual void OnRemoveTrack(scoped_refptr<RTCRtpReceiver> receiver) override;
  virtual void OnDataChannel(
      scoped_refptr<RTCDataChannel> data_channel) override;
  virtual void OnRenegotiationNeeded() override;

  scoped_refptr<RTCMediaStream> MediaStreamForId(const std::string& id);

  scoped_refptr<RTCMediaTrack> MediaTrackForId(const std::string& id);

  void RemoveStreamForId(const std::string& id);

 private:
  static FlMethodErrorResponse* FlEventChannelListenCB(FlEventChannel* channel,
                                                FlValue* args,
                                                gpointer user_data);
  static FlMethodErrorResponse* FlEventChannelCancelCB(FlEventChannel* channel,
                                                FlValue* args,
                                                gpointer user_data);

  scoped_refptr<RTCPeerConnection> peerconnection_;
  std::map<std::string, scoped_refptr<RTCMediaStream>> remote_streams_;
  FlutterWebRTCBase* base_;
  std::string id_;
  FlEventChannel* event_channel_;
  bool event_sink_;
  };

class FlutterPeerConnection {
 public:
  FlutterPeerConnection(FlutterWebRTCBase* base) : base_(base) {}

  void CreateRTCPeerConnection(
      const FlValue *configuration,
      const FlValue *constraints,
      FlMethodCall *result);

  void RTCPeerConnectionClose(
      RTCPeerConnection* pc,
      const std::string& uuid,
      FlMethodCall *result);

  void RTCPeerConnectionDispose(
      RTCPeerConnection* pc,
      const std::string& uuid,
      FlMethodCall *result);

  void CreateOffer(const FlValue *constraints,
                   RTCPeerConnection* pc,
                   FlMethodCall *result);

  void CreateAnswer(const FlValue *constraints,
                    RTCPeerConnection* pc,
                    FlMethodCall *result);

  void SetLocalDescription(
      RTCSessionDescription* sdp,
      RTCPeerConnection* pc,
      FlMethodCall *result);

  void SetRemoteDescription(
      RTCSessionDescription* sdp,
      RTCPeerConnection* pc,
      FlMethodCall *result);

  void GetLocalDescription(
      RTCPeerConnection* pc,
      FlMethodCall *result);

  void GetRemoteDescription(
      RTCPeerConnection* pc,
      FlMethodCall *resulte);

  scoped_refptr<RTCRtpTransceiverInit> mapToRtpTransceiverInit(
      const FlValue *transceiverInit);

  RTCRtpTransceiverDirection stringToTransceiverDirection(
      std::string direction);

  libwebrtc::scoped_refptr<libwebrtc::RTCRtpEncodingParameters> mapToEncoding(
      const FlValue *parameters);

  void AddTransceiver(RTCPeerConnection* pc,
                      const std::string& trackId,
                      const std::string& mediaType,
                      const FlValue *transceiverInit,
                      FlMethodCall *resulte);

  void GetTransceivers(RTCPeerConnection* pc,
                       FlMethodCall *resulte);

  void GetReceivers(RTCPeerConnection* pc,
                    FlMethodCall *resulte);

  void RtpSenderDispose(RTCPeerConnection* pc,
                        std::string rtpSenderId,
                        FlMethodCall *resulte);

  void RtpSenderSetTrack(RTCPeerConnection* pc,
                         RTCMediaTrack* track,
                         std::string rtpSenderId,
                         FlMethodCall *resulte);

  void RtpSenderReplaceTrack(
      RTCPeerConnection* pc,
      RTCMediaTrack* track,
      std::string rtpSenderId,
      FlMethodCall *resulte);

  scoped_refptr<RTCRtpParameters> updateRtpParameters(
      const FlValue *newParameters,
      scoped_refptr<RTCRtpParameters> parameters);

  void RtpSenderSetParameters(
      RTCPeerConnection* pc,
      std::string rtpSenderId,
      const FlValue *parameters,
      FlMethodCall *resulte);

  void RtpTransceiverStop(
      RTCPeerConnection* pc,
      std::string rtpTransceiverId,
      FlMethodCall *resulte);

  void RtpTransceiverGetCurrentDirection(
      RTCPeerConnection* pc,
      std::string rtpTransceiverId,
      FlMethodCall *resulte);

  void SetConfiguration(RTCPeerConnection* pc,
                        const FlValue *configuration,
                        FlMethodCall *resulte);

  void CaptureFrame(RTCVideoTrack* track,
                    std::string path,
                    FlMethodCall *resulte);

  scoped_refptr<RTCRtpTransceiver> getRtpTransceiverById(RTCPeerConnection* pc,
                                                         std::string id);

  void RtpTransceiverSetDirection(
      RTCPeerConnection* pc,
      std::string rtpTransceiverId,
      std::string direction,
      FlMethodCall *resulte);

  void GetSenders(RTCPeerConnection* pc,
                  FlMethodCall *resulte);

  void AddIceCandidate(RTCIceCandidate* candidate,
                       RTCPeerConnection* pc,
                       FlMethodCall *result);

  void GetStats(const std::string& track_id,
                RTCPeerConnection* pc,
                FlMethodCall *result);

  void MediaStreamAddTrack(
      scoped_refptr<RTCMediaStream> stream,
      scoped_refptr<RTCMediaTrack> track,
      FlMethodCall *result);

  void MediaStreamRemoveTrack(
      scoped_refptr<RTCMediaStream> stream,
      scoped_refptr<RTCMediaTrack> track,
      FlMethodCall *result);

  void AddTrack(RTCPeerConnection* pc,
                scoped_refptr<RTCMediaTrack> track,
                std::list<std::string> streamIds,
                FlMethodCall *result);

  libwebrtc::scoped_refptr<libwebrtc::RTCRtpSender> GetRtpSenderById(
      RTCPeerConnection* pc,
      std::string id);

  void RemoveTrack(RTCPeerConnection* pc,
                   std::string senderId,
                   FlMethodCall *result);

 private:
  FlutterWebRTCBase* base_;
};
}  // namespace flutter_webrtc_plugin

#endif  // !FLUTTER_WEBRTC_RTC_PEER_CONNECTION_HXX
