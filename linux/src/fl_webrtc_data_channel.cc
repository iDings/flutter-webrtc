#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:data_channel"

#include "fl_webrtc_data_channel.h"

struct _FlWebrtcDataChannel {
    GObject parent_instance;
};

static void fl_webrtc_data_channel_method_iface_init(FlWebrtcDataChannelMethodInterface *iface);
G_DEFINE_TYPE_WITH_CODE(FlWebrtcDataChannel, fl_webrtc_data_channel, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(FL_TYPE_WEBRTC_DATA_CHANNEL_METHOD,
                        fl_webrtc_data_channel_method_iface_init))

static void create_impl(FlWebrtcDataChannel* self, FlMethodCall* method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void close_impl(FlWebrtcDataChannel* self, FlMethodCall* method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void send_impl(FlWebrtcDataChannel* self, FlMethodCall* method_call) {
    g_message("%s:%d", __func__, __LINE__);
}

static void fl_webrtc_data_channel_method_iface_init(FlWebrtcDataChannelMethodInterface *iface) {
    iface->create = create_impl;
    iface->close = close_impl;
    iface->send = send_impl;
}

static void fl_webrtc_data_channel_dispose(GObject *object) {
    g_message("%s:%d", __func__, __LINE__);
    G_OBJECT_CLASS(fl_webrtc_data_channel_parent_class)->dispose(object);
}

static void fl_webrtc_data_channel_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_data_channel_parent_class)->finalize(object);
}

static void fl_webrtc_data_channel_class_init(FlWebrtcDataChannelClass *klass) {
    g_message("%s:%d", __func__, __LINE__);
    G_OBJECT_CLASS(klass)->dispose = fl_webrtc_data_channel_dispose;
    G_OBJECT_CLASS(klass)->finalize = fl_webrtc_data_channel_finalize;
}

static void fl_webrtc_data_channel_init(FlWebrtcDataChannel *self) {
    g_message("%s:%d", __func__, __LINE__);
}

G_DEFINE_INTERFACE(FlWebrtcDataChannelMethod, fl_webrtc_data_channel_method, G_TYPE_OBJECT)
static void fl_webrtc_data_channel_method_default_init(FlWebrtcDataChannelMethodInterface *iface) {
    g_signal_new("createdatachannel", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcDataChannelMethodInterface, create),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("dataChannelSend", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcDataChannelMethodInterface, send),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("dataChannelClose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcDataChannelMethodInterface, close),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);
}
