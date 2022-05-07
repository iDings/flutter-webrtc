#ifndef __FL_WEBRTC_CONTEXT_H__
#define __FL_WEBRTC_CONTEXT_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_CONTEXT fl_webrtc_context_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcContext, fl_webrtc_context, FL, WEBRTC_CONTEXT, GObject)

FlWebrtcContext *fl_webrtc_context_new(FlBinaryMessenger *message, FlTextureRegistrar *texture);
FlBinaryMessenger *fl_webrtc_context_messenger(FlWebrtcContext *self);
FlTextureRegistrar *fl_webrtc_context_texture_registrar(FlWebrtcContext *self);
gboolean fl_webrtc_context_invoke_method(FlWebrtcContext *self, FlMethodCall *method_call);

G_END_DECLS
#endif
