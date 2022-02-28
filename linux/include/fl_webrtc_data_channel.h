#ifndef __FL_WEBRTC_DATA_CHANNEL_H__
#define __FL_WEBRTC_DATA_CHANNEL_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_DATA_CHANNEL fl_webrtc_data_channel_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcDataChannel, fl_webrtc_data_channel, FL, WEBRTC_DATA_CHANNEL, GObject)

#define FL_TYPE_WEBRTC_DATA_CHANNEL_METHOD fl_webrtc_data_channel_method_get_type()
G_DECLARE_INTERFACE(FlWebrtcDataChannelMethod,
                    fl_webrtc_data_channel_method,
                    FL,
                    WEBRTC_DATA_CHANNEL_METHOD,
                    GObject)
struct _FlWebrtcDataChannelMethodInterface {
    GTypeInterface g_iface;

    void (*create)(FlWebrtcDataChannel* self, FlMethodCall* method_call);
    void (*send)(FlWebrtcDataChannel* self, FlMethodCall* method_call);
    void (*close)(FlWebrtcDataChannel* self, FlMethodCall* method_call);
};

G_END_DECLS

#endif
