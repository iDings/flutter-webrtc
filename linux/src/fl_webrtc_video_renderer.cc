#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:video_renderer"

#include "fl_webrtc_context.h"
#include "fl_webrtc_video_renderer.h"
#include "fl_webrtc_video_texture.h"
#include "fl_webrtc_log.h"

#include <inttypes.h>
#include <unistd.h>
#include <sys/syscall.h>

struct _FlWebrtcVideoRenderer {
    GObject parent_instance;
    FlWebrtcContext *context;
    GList *textures;
};

static void fl_webrtc_video_renderer_method_iface_init(FlWebrtcVideoRendererMethodInterface *iface);
G_DEFINE_TYPE_WITH_CODE(FlWebrtcVideoRenderer, fl_webrtc_video_renderer, G_TYPE_OBJECT,
                        G_IMPLEMENT_INTERFACE(FL_TYPE_WEBRTC_VIDEO_RENDERER_METHOD,
                        fl_webrtc_video_renderer_method_iface_init))

G_DEFINE_INTERFACE(FlWebrtcVideoRendererMethod, fl_webrtc_video_renderer_method, G_TYPE_OBJECT)
static void fl_webrtc_video_renderer_method_default_init(FlWebrtcVideoRendererMethodInterface *iface) {
    g_signal_new("createVideoRenderer", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcVideoRendererMethodInterface, create_texture),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("videoRendererSetSrcObject", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcVideoRendererMethodInterface, set_media_stream),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);

    g_signal_new("videoRendererDispose", G_TYPE_FROM_INTERFACE(iface), G_SIGNAL_RUN_LAST,
            G_STRUCT_OFFSET(FlWebrtcVideoRendererMethodInterface, dispose),
            NULL, NULL, NULL, G_TYPE_NONE, 1, G_TYPE_OBJECT);
}

static void create_texture_impl(FlWebrtcVideoRenderer* self, FlMethodCall* method_call) {
    FL_WEBRTC_LOGI("Enter");

    FlWebrtcVideoTexture *video_texture = fl_webrtc_video_texture_new(self->context);
    self->textures = g_list_append(self->textures, video_texture);

    g_autoptr(FlValue) result = fl_value_new_map();
    fl_value_set_string_take(result, "textureId", fl_value_new_int(fl_webrtc_video_texture_id(video_texture)));
    g_autoptr(FlMethodResponse) response = FL_METHOD_RESPONSE(fl_method_success_response_new(result));
    fl_method_call_respond(method_call, response, nullptr);
}

static void set_media_stream_impl(FlWebrtcVideoRenderer* self, FlMethodCall* method_call) {
  FL_WEBRTC_TRACE("Enter");
  FlValue *args = fl_method_call_get_args(method_call);
  FlValue *stream_id = fl_value_lookup_string(args, "streamId");
  FlValue *texture_id = fl_value_lookup_string(args, "textureId");

  FL_WEBRTC_LOGI("stream_id:%s texture_id:%" PRIi64, fl_value_get_string(stream_id), fl_value_get_int(texture_id));
  return;
}

static void dispose_impl(FlWebrtcVideoRenderer* self, FlMethodCall* method_call) {
  FL_WEBRTC_LOGI("Enter");
  FlValue *args = fl_method_call_get_args(method_call);
  FlValue *texture_id = fl_value_lookup_string(args, "textureId");
  GList *elem = g_list_find(self->textures, GINT_TO_POINTER(fl_value_get_int(texture_id)));
  if (!elem) {
    g_autoptr(FlMethodResponse) response = FL_METHOD_RESPONSE(
        fl_method_error_response_new("VideoRendererDisposeFailed", "VideoRendererDispose() texture not found!", NULL));
    fl_method_call_respond(method_call, response, nullptr);
    return;
  }

  self->textures = g_list_remove_link(self->textures, elem);
  FlWebrtcVideoTexture *video_texture = static_cast<FlWebrtcVideoTexture *>(elem->data);
  FL_WEBRTC_LOGI("disposing texture_id:%" PRIi64, fl_webrtc_video_texture_id(video_texture));
  g_object_unref(video_texture);
  g_list_free(elem);
  g_autoptr(FlMethodResponse) response = FL_METHOD_RESPONSE(fl_method_success_response_new(NULL));
  fl_method_call_respond(method_call, response, nullptr);
  return;
}

static void fl_webrtc_video_renderer_method_iface_init(FlWebrtcVideoRendererMethodInterface *iface) {
    FL_WEBRTC_LOGI("Enter");
    iface->create_texture = create_texture_impl;
    iface->set_media_stream = set_media_stream_impl;
    iface->dispose = dispose_impl;
}

static void fl_webrtc_video_renderer_dispose(GObject *object) {
    FL_WEBRTC_LOGI("Enter");
    G_OBJECT_CLASS(fl_webrtc_video_renderer_parent_class)->dispose(object);
}

static void fl_webrtc_video_renderer_finalize(GObject *object) {
  FL_WEBRTC_LOGI("Enter");
  FlWebrtcVideoRenderer *self = FL_WEBRTC_VIDEO_RENDERER(object);
  g_list_free_full(self->textures, g_object_unref);
  G_OBJECT_CLASS(fl_webrtc_video_renderer_parent_class)->finalize(object);
}

static void fl_webrtc_video_renderer_class_init(FlWebrtcVideoRendererClass *klass) {
  FL_WEBRTC_LOGI("Enter");
  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_video_renderer_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_video_renderer_finalize;
}

static void fl_webrtc_video_renderer_init(FlWebrtcVideoRenderer *self) {
  FL_WEBRTC_LOGI("Enter");
}

FlWebrtcVideoRenderer *fl_webrtc_video_renderer_new(FlWebrtcContext *context) {
  FlWebrtcVideoRenderer *thiz = FL_WEBRTC_VIDEO_RENDERER(g_object_new(FL_TYPE_WEBRTC_VIDEO_RENDERER, NULL));
  g_assert(thiz != nullptr);

  thiz->context = FL_WEBRTC_CONTEXT(context);
  return thiz;
}
