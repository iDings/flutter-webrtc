#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:video_texture"

#include "flutter_video_texture.h"
#include "flutter_glog.h"

#include <flutter_linux/flutter_linux.h>
#include <rtc_video_frame.h>
#include <rtc_video_renderer.h>

#include <inttypes.h>

struct _FlWebrtcVideoTexture {
  FlPixelBufferTexture parent_instance;
  FlWebrtcVideoTextureCopyHandler on_copy;
  gpointer user_data;
  GDestroyNotify destroy_notify;
};

G_DEFINE_TYPE(FlWebrtcVideoTexture,
              fl_webrtc_video_texture,
              fl_pixel_buffer_texture_get_type())

// This method is called on Render Thread. Be careful with your
// cross-thread operation.
// @width and @height are initially stored the canvas size in Flutter.
static gboolean
fl_webrtc_video_texture_copy_pixels (FlPixelBufferTexture* texture,
                                                  const uint8_t** out_buffer,
                                                  uint32_t* width,
                                                  uint32_t* height,
                                                  GError** error) {
  FlWebrtcVideoTexture *self = FL_WEBRTC_VIDEO_TEXTURE(texture);
  if (self->on_copy) {
    return self->on_copy(self, out_buffer, width, height, error, self->user_data);
  }

  return true;
}

static void fl_webrtc_video_texture_dispose(GObject *object) {
  //FlWebrtcVideoTexture *self = FL_WEBRTC_VIDEO_TEXTURE(object);
  G_OBJECT_CLASS(fl_webrtc_video_texture_parent_class)->dispose(object);
  return;
}

static void fl_webrtc_video_texture_finalize(GObject *object) {
  G_OBJECT_CLASS(fl_webrtc_video_texture_parent_class)->finalize(object);
  return;
}

static void fl_webrtc_video_texture_class_init(FlWebrtcVideoTextureClass *klass) {
  FL_PIXEL_BUFFER_TEXTURE_CLASS(klass)->copy_pixels = fl_webrtc_video_texture_copy_pixels;

  G_OBJECT_CLASS(klass)->dispose = fl_webrtc_video_texture_dispose;
  G_OBJECT_CLASS(klass)->finalize = fl_webrtc_video_texture_finalize;
  return;
}

static void fl_webrtc_video_texture_init(FlWebrtcVideoTexture *self) {
  return;
}

FlWebrtcVideoTexture *fl_webrtc_video_texture_new(void) {
  FlWebrtcVideoTexture *self;
  self = FL_WEBRTC_VIDEO_TEXTURE((g_object_new(fl_webrtc_video_texture_get_type(), NULL)));
  return self;
}

void fl_webrtc_video_texture_set_handler(FlWebrtcVideoTexture *self,
    FlWebrtcVideoTextureCopyHandler on_copy, gpointer user_data, GDestroyNotify destroy_notify) {
  if (self->user_data && self->destroy_notify)
    self->destroy_notify(self->user_data);

  self->on_copy = on_copy;
  self->user_data = user_data;
  self->destroy_notify = destroy_notify;
  return;
}

int64_t fl_webrtc_video_texture_id(FlWebrtcVideoTexture *self) {
  return reinterpret_cast<int64_t>(self);
}
