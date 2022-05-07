#ifndef FLUTTER_WEBRTC_RTC_DATA_CHANNEL_HXX
#define FLUTTER_WEBRTC_RTC_DATA_CHANNEL_HXX

#include "flutter_webrtc_base.h"

namespace flutter_webrtc_plugin {

class FlutterRTCDataChannelObserver : public RTCDataChannelObserver {
 public:
  FlutterRTCDataChannelObserver(scoped_refptr<RTCDataChannel> data_channel,
                                FlBinaryMessenger *messenger,
                                const std::string &channel_name);
  virtual ~FlutterRTCDataChannelObserver();

  virtual void OnStateChange(RTCDataChannelState state) override;
  virtual void OnMessage(const char *buffer, int length, bool binary) override;
  scoped_refptr<RTCDataChannel> data_channel() { return data_channel_; }

  static FlMethodErrorResponse *FlEventChannelListenCB(FlEventChannel* channel, FlValue* args, gpointer user_data);
  static FlMethodErrorResponse *FlEventChannelCancelCB(FlEventChannel* channel, FlValue* args, gpointer user_data);

 private:
  scoped_refptr<RTCDataChannel> data_channel_;
  bool event_sink_;
  FlEventChannel *event_channel_;
};

class FlutterDataChannel {
 public:
  FlutterDataChannel(FlutterWebRTCBase *base) : base_(base) {}

  void CreateDataChannel(const std::string& peerConnectionId,
                         const std::string& label,
                         const FlValue *dataChannelDict,
                         RTCPeerConnection *pc,
                         FlMethodCall *method_call);

  void DataChannelSend(RTCDataChannel *data_channel, const std::string &type,
                       const FlValue *data,
                       FlMethodCall *method_call);

  void DataChannelClose(RTCDataChannel *data_channel,
                        FlMethodCall *method_call);

  RTCDataChannel *DataChannelFormId(int id);

 private:
  FlutterWebRTCBase *base_;
};

}  // namespace flutter_webrtc_plugin

#endif  // !FLUTTER_WEBRTC_RTC_DATA_CHANNEL_HXX
