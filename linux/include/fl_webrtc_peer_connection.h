#ifndef __FL_WEBRTC_PEER_CONNECTION_H__
#define __FL_WEBRTC_PEER_CONNECTION_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_PEER_CONNECTION fl_webrtc_peer_connection_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcPeerConnection, fl_webrtc_peer_connection, FL, WEBRTC_PEER_CONNECTION, GObject)

#define FL_TYPE_WEBRTC_PEER_CONNECTION_METHOD fl_webrtc_peer_connection_method_get_type()
G_DECLARE_INTERFACE(FlWebrtcPeerConnectionMethod,
                    fl_webrtc_peer_connection_method,
                    FL,
                    WEBRTC_PEER_CONNECTION_METHOD,
                    GObject)

struct _FlWebrtcPeerConnectionMethodInterface {
    GTypeInterface g_iface;

    /* method call signals */
    void (*create_peer_connection)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*close_peer_connection)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*dispose_peer_connection)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);

    void (*create_offer)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*create_answer)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);

    void (*set_local_description)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*set_remote_description)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*get_local_description)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);

    void (*add_transceiver)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*get_transceivers)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);

    void (*rtp_sender_dispose)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_sender_set_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_sender_replace_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_sender_set_parameters)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_transceiver_stop)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_transceiver_get_current_direction)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*rtp_transceiver_set_direction)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);

    void (*set_configuration)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*capture_frame)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*get_senders)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*add_ice_candidate)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*get_stats)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*media_stream_add_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*media_stream_remove_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*add_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
    void (*remove_track)(FlWebrtcPeerConnection* self, FlMethodCall* method_call);
};

G_END_DECLS

#endif
