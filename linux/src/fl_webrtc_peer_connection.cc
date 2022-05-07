#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:peer_connection"

#include "fl_webrtc_peer_connection.h"
#include "fl_webrtc_context.h"
#include "fl_webrtc_log.h"

struct _FlWebrtcPeerConnection {
    GObject parent_instance;
    FlWebrtcContext *context;
};

static void fl_webrtc_peer_connection_method_iface_init(FlWebrtcPeerConnectionMethodInterface *iface);
G_DEFINE_TYPE_WITH_CODE(FlWebrtcPeerConnection, fl_webrtc_peer_connection, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(FL_TYPE_WEBRTC_PEER_CONNECTION_METHOD,
                        fl_webrtc_peer_connection_method_iface_init))

static void create_peer_connection_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void close_peer_connection_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void dispose_peer_connection_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void create_offer_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void create_answer_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void set_local_description_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void set_remote_description_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_local_description_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void add_transceiver_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_transceivers_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_sender_dispose_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_sender_set_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_sender_replace_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_sender_set_parameters_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_transceiver_stop_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_transceiver_get_current_direction_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void rtp_transceiver_set_direction_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void set_configuration_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void capture_frame_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_senders_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void add_ice_candidate_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void get_stats_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void media_stream_add_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void media_stream_remove_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void add_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void remove_track_impl(FlWebrtcPeerConnection* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
}

static void fl_webrtc_peer_connection_method_iface_init(FlWebrtcPeerConnectionMethodInterface *iface) {
  FL_WEBRTC_LOGI("Enter");
  iface->create_peer_connection = create_peer_connection_impl;
  iface->close_peer_connection = close_peer_connection_impl;
  iface->dispose_peer_connection = dispose_peer_connection_impl;
  iface->create_offer = create_offer_impl;
  iface->create_answer = create_answer_impl;
  iface->set_local_description = set_local_description_impl;
  iface->set_remote_description = set_remote_description_impl;
  iface->get_local_description = get_local_description_impl;
  iface->add_transceiver = add_transceiver_impl;
  iface->get_transceivers = get_transceivers_impl;
  iface->rtp_sender_dispose = rtp_sender_dispose_impl;
  iface->rtp_sender_set_track = rtp_sender_set_track_impl;
  iface->rtp_sender_replace_track = rtp_sender_replace_track_impl;
  iface->rtp_sender_set_parameters = rtp_sender_set_parameters_impl;
  iface->rtp_transceiver_stop = rtp_transceiver_stop_impl;
  iface->rtp_transceiver_get_current_direction = rtp_transceiver_get_current_direction_impl;
  iface->rtp_transceiver_set_direction = rtp_transceiver_set_direction_impl;
  iface->set_configuration = set_configuration_impl;
  iface->capture_frame = capture_frame_impl;
  iface->get_senders = get_senders_impl;
  iface->add_ice_candidate = add_ice_candidate_impl;
  iface->get_stats = get_stats_impl;
  iface->media_stream_add_track = media_stream_add_track_impl;
  iface->media_stream_remove_track = media_stream_remove_track_impl;
  iface->add_track = add_track_impl;
  iface->remove_track = remove_track_impl;
}

static void fl_webrtc_peer_connection_dispose(GObject* object) {
  G_OBJECT_CLASS(fl_webrtc_peer_connection_parent_class)->dispose(object);
}

static void fl_webrtc_peer_connection_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_peer_connection_parent_class)->finalize(object);
}

static void fl_webrtc_peer_connection_class_init(FlWebrtcPeerConnectionClass *klass) {
  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_peer_connection_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_peer_connection_finalize;
}

static void fl_webrtc_peer_connection_init(FlWebrtcPeerConnection *self) {
}

G_DEFINE_INTERFACE(FlWebrtcPeerConnectionMethod, fl_webrtc_peer_connection_method, G_TYPE_OBJECT)
  static void fl_webrtc_peer_connection_method_default_init(FlWebrtcPeerConnectionMethodInterface *iface) {
    g_signal_new("createPeerConnection", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, create_peer_connection),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("peerConnectionClose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, close_peer_connection),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("peerConnectionDispose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, dispose_peer_connection),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("createOffer", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, create_offer),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("createAnswer", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, create_answer),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("setLocalDescription", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, set_local_description),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("setRemoteDescription", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, set_remote_description),
        NULL, NULL, g_cclosure_marshal_VOID__POINTER, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("getLocalDescription", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, get_local_description),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("addTransceiver", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, add_transceiver),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("getTransceivers", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, get_transceivers),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpSenderDispose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_sender_dispose),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpSenderSetTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_sender_set_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpSenderReplaceTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_sender_replace_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpSenderSetParameters", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_sender_set_parameters),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpTransceiverStop", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_transceiver_stop),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpTransceiverGetCurrentDirection", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_transceiver_get_current_direction),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("rtpTransceiverSetDirection", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, rtp_transceiver_set_direction),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("setConfiguration", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, set_configuration),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("captureFrame", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, capture_frame),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("getSenders", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, get_senders),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("addCandidate", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, add_ice_candidate),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("getStats", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, get_stats),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("mediaStreamAddTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, media_stream_add_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("mediaStreamRemoveTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, media_stream_remove_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("addTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, add_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);

    g_signal_new("removeTrack", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
        G_STRUCT_OFFSET(FlWebrtcPeerConnectionMethodInterface, remove_track),
        NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_POINTER);
  }

FlWebrtcPeerConnection *fl_webrtc_peer_connection_new(FlWebrtcContext *context) {
  FlWebrtcPeerConnection *self = FL_WEBRTC_PEER_CONNECTION(g_object_new(FL_TYPE_WEBRTC_PEER_CONNECTION, NULL));
  self->context = FL_WEBRTC_CONTEXT(context);
  return self;
}
