#include "flutter_data_channel.h"
#include "flutter_glog.h"

#include <vector>
#include <string>

namespace flutter_webrtc_plugin {

FlMethodErrorResponse*
FlutterRTCDataChannelObserver::FlEventChannelListenCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterRTCDataChannelObserver *thiz = reinterpret_cast<FlutterRTCDataChannelObserver *>(user_data);
  thiz->event_sink_ = true;
  return nullptr;
}

FlMethodErrorResponse*
FlutterRTCDataChannelObserver::FlEventChannelCancelCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterRTCDataChannelObserver *thiz = reinterpret_cast<FlutterRTCDataChannelObserver *>(user_data);
  thiz->event_sink_ = false;
  return nullptr;
}

FlutterRTCDataChannelObserver::FlutterRTCDataChannelObserver(
    scoped_refptr<RTCDataChannel> data_channel, FlBinaryMessenger *messenger,
    const std::string &name)
    : data_channel_(data_channel) {
  event_sink_ = false;
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  event_channel_ = fl_event_channel_new(messenger, name.c_str(), FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(event_channel_, FlEventChannelListenCB, FlEventChannelCancelCB, this, nullptr);
  data_channel_->RegisterObserver(this);
}

FlutterRTCDataChannelObserver::~FlutterRTCDataChannelObserver() { g_object_unref(event_channel_); }

void FlutterDataChannel::CreateDataChannel(
    const std::string& peerConnectionId,
    const std::string &label,
    const FlValue *dataChannelDict,
    RTCPeerConnection *pc,
    FlMethodCall *result) {

  RTCDataChannelInit init;
  FlValue *id_ = fl_value_lookup_string(const_cast<FlValue *>(dataChannelDict), "id");
  if (id_ != nullptr) init.id = fl_value_get_int(id_);

  base_->lock();
  if (base_->data_channel_observers_.find( init.id) !=
      base_->data_channel_observers_.end()) {
    for(int i = 1024; i < 65535; i++){
      if(base_->data_channel_observers_.find(i) ==
      base_->data_channel_observers_.end()){
         init.id = i;
        break;
      }
    }
  }
  base_->unlock();

  FlValue *ordered_ = fl_value_lookup_string(const_cast<FlValue *>(dataChannelDict), "ordered");
  if (ordered_) init.ordered = fl_value_get_bool(ordered_);

  FlValue *maxRetransmits_ = fl_value_lookup_string(const_cast<FlValue *>(dataChannelDict), "maxRetransmits");
  if (maxRetransmits_) init.maxRetransmits = fl_value_get_int(maxRetransmits_);

  std::string protocol = "sctp";
  FlValue *protocol_ = fl_value_lookup_string(const_cast<FlValue *>(dataChannelDict), "protocol");
  if (protocol_) protocol = fl_value_get_string(protocol_);
  init.protocol = protocol;

  FlValue *negotiated_ = fl_value_lookup_string(const_cast<FlValue *>(dataChannelDict), "negotiated");
  init.negotiated = fl_value_get_bool(negotiated_);

  scoped_refptr<RTCDataChannel> data_channel =
      pc->CreateDataChannel(label.c_str(), &init);

  std::string event_channel = "FlutterWebRTC/dataChannelEvent" +
                              peerConnectionId + std::to_string(init.id);

  std::unique_ptr<FlutterRTCDataChannelObserver> observer(
      new FlutterRTCDataChannelObserver(data_channel, base_->messenger_,
                                        event_channel));

  base_->lock();
  base_->data_channel_observers_[init.id] = std::move(observer);
  base_->unlock();

  g_autoptr(FlValue) params = fl_value_new_map();
  fl_value_set_string_take(params, "id", fl_value_new_int(init.id));
  fl_value_set_string_take(params, "label", fl_value_new_string(data_channel->label().std_string().c_str()));
  g_autoptr(FlMethodResponse) response = FL_METHOD_RESPONSE(fl_method_success_response_new(params));
  fl_method_call_respond(result, response, nullptr);
}

void FlutterDataChannel::DataChannelSend(
    RTCDataChannel *data_channel, const std::string &type,
    const FlValue *data,
    FlMethodCall *result) {
  bool is_binary = type == "binary";
  FlValue *_data = const_cast<FlValue *>(data);
  if (is_binary && (fl_value_get_type(_data) == FL_VALUE_TYPE_UINT8_LIST)) {
    const uint8_t *buffer = fl_value_get_uint8_list(_data);
    size_t size = fl_value_get_length(_data);
    string binary = std::string((const char *)buffer, (unsigned)size);
    data_channel->Send(binary, true);
  } else {
    std::string str = fl_value_get_string(_data);
    data_channel->Send(str, false);
  }

  fl_method_call_respond_success(result, nullptr, nullptr);
}

void FlutterDataChannel::DataChannelClose(
    RTCDataChannel *data_channel,
    FlMethodCall *result) {
  int id = data_channel->id();
  data_channel->Close();
  auto it = base_->data_channel_observers_.find(id);
  if (it != base_->data_channel_observers_.end())
    base_->data_channel_observers_.erase(it);

  fl_method_call_respond_success(result, nullptr, nullptr);
}

RTCDataChannel *FlutterDataChannel::DataChannelFormId(int id) {
  auto it = base_->data_channel_observers_.find(id);

  if (it != base_->data_channel_observers_.end()) {
    FlutterRTCDataChannelObserver *observer = it->second.get();
    scoped_refptr<RTCDataChannel> data_channel = observer->data_channel();
    return data_channel.get();
  }
  return nullptr;
}

static const char *DataStateString(RTCDataChannelState state) {
  switch (state) {
    case RTCDataChannelConnecting:
      return "connecting";
    case RTCDataChannelOpen:
      return "open";
    case RTCDataChannelClosing:
      return "closing";
    case RTCDataChannelClosed:
      return "closed";
  }
  return "";
}

void FlutterRTCDataChannelObserver::OnStateChange(RTCDataChannelState state) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("dataChannelStateChanged"));
    fl_value_set_string_take(params, "id", fl_value_new_int(data_channel_->id()));
    fl_value_set_string_take(params, "state", fl_value_new_string(DataStateString(state)));
    g_autoptr(GError) error = nullptr;
    if (!fl_event_channel_send(event_channel_, params, nullptr, &error)) {
      FL_LOGW("Failed to send event: %s", error->message);
    }
  }
}

void FlutterRTCDataChannelObserver::OnMessage(const char *buffer, int length,
                                              bool binary) {
  if (event_sink_) {
    g_autoptr(FlValue) params = fl_value_new_map();
    fl_value_set_string_take(params, "event", fl_value_new_string("dataChannelReceiveMessage"));
    fl_value_set_string_take(params, "id", fl_value_new_int(data_channel_->id()));
    fl_value_set_string_take(params, "type", fl_value_new_string(binary ? "binary" : "text"));

    FlValue *data_;
    if (!binary) data_ = fl_value_new_string(buffer);
    else data_ = fl_value_new_uint8_list(reinterpret_cast<const uint8_t *>(buffer), length);
    fl_value_set_string_take(params, "data", data_);
    g_autoptr(GError) error = nullptr;
    if (!fl_event_channel_send(event_channel_, params, nullptr, &error)) {
      FL_LOGW("Failed to send event: %s", error->message);
    }
  }
}
}  // namespace flutter_webrtc_plugin
