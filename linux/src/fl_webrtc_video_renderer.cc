#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:video_renderer"

#include "fl_webrtc_video_renderer.h"

#include <unistd.h>
#include <sys/syscall.h>

struct _FlWebrtcVideoPixelBufferTexture {
  FlPixelBufferTexture parent_instance;
};

G_DEFINE_TYPE(FlWebrtcVideoPixelBufferTexture,
              fl_webrtc_video_pixel_buffer_texture,
              fl_pixel_buffer_texture_get_type())

// This method is called on Render Thread. Be careful with your
// cross-thread operation.
// @width and @height are initially stored the canvas size in Flutter.
static gboolean
fl_webrtc_video_pixel_buffer_texture_copy_pixels (FlPixelBufferTexture* texture,
                                                  const uint8_t** out_buffer,
                                                  uint32_t* width,
                                                  uint32_t* height,
                                                  GError** error) {

  // You must prepare your pixel buffer in RGBA format.
  // So you may do some format conversion first if your original pixel
  // buffer is not in RGBA format.
  // manage_your_pixel_buffer_here ();

  //if (your_operations_are_successfully_finished) {
  //  // Directly return pointer to your pixel buffer here.
  //  // Flutter takes content of your pixel buffer after this function
  //  // is finished. So you must make the buffer live long enough until
  //  // next tick of Render Thread.
  //  // If it is hard to manage lifetime of your pixel buffer, you should
  //  // take look into #FlTextureGL.

  //  *out_buffer = buffer;
  //  *width = real_width_of_buffer;
  //  *height = real_height_of_buffer;
  //  return TRUE;
  //}

  // set @error to report failure.
  return FALSE;
}

static void fl_webrtc_video_pixel_buffer_texture_class_init(FlWebrtcVideoPixelBufferTextureClass *klass) {
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = fl_webrtc_video_pixel_buffer_texture_copy_pixels;
}

static void fl_webrtc_video_pixel_buffer_texture_init(FlWebrtcVideoPixelBufferTexture *self) {
}

struct _FlWebrtcVideoRenderer {
    GObject parent_instance;
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
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);

    g_autoptr(FlMethodResponse) response = FL_METHOD_RESPONSE(fl_method_not_implemented_response_new());
    fl_method_call_respond(method_call, response, nullptr);
}

static void set_media_stream_impl(FlWebrtcVideoRenderer* self, FlMethodCall* method_call) {
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
}

static void dispose_impl(FlWebrtcVideoRenderer* self, FlMethodCall* method_call) {
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
}

static void fl_webrtc_video_renderer_method_iface_init(FlWebrtcVideoRendererMethodInterface *iface) {
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
    iface->create_texture = create_texture_impl;
    iface->set_media_stream = set_media_stream_impl;
    iface->dispose = dispose_impl;
}

static void fl_webrtc_video_renderer_dispose(GObject *object) {
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
    G_OBJECT_CLASS(fl_webrtc_video_renderer_parent_class)->dispose(object);
}

static void fl_webrtc_video_renderer_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_video_renderer_parent_class)->finalize(object);
}

static void fl_webrtc_video_renderer_class_init(FlWebrtcVideoRendererClass *klass) {
  g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_video_renderer_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_video_renderer_finalize;
}

static void fl_webrtc_video_renderer_init(FlWebrtcVideoRenderer *self) {
    g_message("{%ld} %s:%d", syscall(SYS_gettid), __func__, __LINE__);
}
