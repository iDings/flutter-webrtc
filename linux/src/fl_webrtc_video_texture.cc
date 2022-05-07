#define G_LOG_USE_STRUCTURED
#define G_LOG_DOMAIN "fl_webrtc:video_texture"

#include "fl_webrtc_video_texture.h"
#include "fl_webrtc_log.h"

#include <flutter_linux/flutter_linux.h>
#include <rtc_video_frame.h>
#include <rtc_video_renderer.h>

#include <inttypes.h>

using namespace libwebrtc;

class FlWebrtcVideoFrameListener: public RTCVideoRenderer<scoped_refptr<RTCVideoFrame>> {
 public:
  virtual void OnFrame(scoped_refptr<RTCVideoFrame> frame) override;
  std::string media_stream_id;

  public:
    FlWebrtcVideoFrameListener(FlWebrtcVideoTexture *texture);
    ~FlWebrtcVideoFrameListener();
    FlWebrtcVideoFrameListener(const FlWebrtcVideoFrameListener&) = delete;
    FlWebrtcVideoFrameListener& operator=(const FlWebrtcVideoFrameListener&) = delete;

 private:
  //RTCVideoFrame::VideoRotation rotation_ = RTCVideoFrame::kVideoRotation_0;
  FlWebrtcVideoTexture *video_texture;
};

static void fl_webrtc_video_texture_on_frame(FlWebrtcVideoTexture *video_texture, scoped_refptr<RTCVideoFrame> f);

// push frame from webrtc
void FlWebrtcVideoFrameListener::OnFrame(scoped_refptr<RTCVideoFrame> f) {
  fl_webrtc_video_texture_on_frame(video_texture, f);
  return;
}

FlWebrtcVideoFrameListener::FlWebrtcVideoFrameListener(FlWebrtcVideoTexture *video_texture) {
  this->video_texture = video_texture;
}

FlWebrtcVideoFrameListener::~FlWebrtcVideoFrameListener() {
}

struct frame_info {
  uint32_t width;
  uint32_t height;
  uint8_t *buf;
};

// c context, all c++ use pointer here
struct _FlWebrtcVideoTexture {
  FlPixelBufferTexture parent_instance;
  FlWebrtcContext *context;

  // c++ context
  FlWebrtcVideoFrameListener *rtc_frame;
  FlEventChannel* channel;
  GMutex lock;
  bool rendered;
  bool has_pending;
  struct frame_info pending;
  struct frame_info rendering;
};

G_DEFINE_TYPE(FlWebrtcVideoTexture,
              fl_webrtc_video_texture,
              fl_pixel_buffer_texture_get_type())

static void fl_webrtc_video_texture_on_frame(FlWebrtcVideoTexture *video_texture, scoped_refptr<RTCVideoFrame> f) {
  FL_WEBRTC_LOGI("Enter");
  g_autoptr(GMutexLocker) locker = g_mutex_locker_new(&video_texture->lock);
  (void)locker;  // make compiler happy

  struct frame_info *fp = &video_texture->pending;
  if (!fp->buf || (fp->height != f->height()) || (fp->width != f->width())) {
      size_t buffer_size = (f->width() * f->height()) * (32 >> 3);
      if (fp->buf) g_clear_pointer(&fp->buf, g_free);

      fp->buf = static_cast<uint8_t *>(g_malloc0(buffer_size));
      g_assert(fp->buf != nullptr);
      fp->height = f->height();
      fp->width = f->width();
  }

  f->ConvertToARGB(RTCVideoFrame::Type::kABGR, fp->buf, 0, f->width(), f->height());
  video_texture->has_pending = true;
  return;
}

// This method is called on Render Thread. Be careful with your
// cross-thread operation.
// @width and @height are initially stored the canvas size in Flutter.
static gboolean
fl_webrtc_video_texture_copy_pixels (FlPixelBufferTexture* texture,
                                                  const uint8_t** out_buffer,
                                                  uint32_t* width,
                                                  uint32_t* height,
                                                  GError** error) {
  FlWebrtcVideoTexture *video_texture = FL_WEBRTC_VIDEO_TEXTURE(texture);

  {
    g_autoptr(GMutexLocker) locker = g_mutex_locker_new(&video_texture->lock);
    (void)locker;

    if (video_texture->has_pending) {
      // Directly return pointer to your pixel buffer here.
      // Flutter takes content of your pixel buffer after this function
      // is finished. So you must make the buffer live long enough until
      // next tick of Render Thread.
      // If it is hard to manage lifetime of your pixel buffer, you should
      // take look into #FlTextureGL.

      video_texture->has_pending = false;
      struct frame_info f = video_texture->rendering;
      video_texture->rendering = video_texture->pending;
      video_texture->pending = f;
    }
  }

  if (!video_texture->rendering.buf) {
    g_error("shouldn't rgbbuf_rendering nullptr\n");
    return FALSE;
  }

  if (!video_texture->rendered) {
    video_texture->rendered = true;
  }

  // keep last frame
  *out_buffer = video_texture->rendering.buf;
  *width = video_texture->rendering.width;
  *height = video_texture->rendering.height;
  return TRUE;
}

static void fl_webrtc_video_texture_dispose(GObject *object) {
  FlWebrtcVideoTexture *self = FL_WEBRTC_VIDEO_TEXTURE(object);
  FL_WEBRTC_LOGI("Enter:%p", fl_webrtc_context_texture_registrar(self->context));
  fl_texture_registrar_unregister_texture(fl_webrtc_context_texture_registrar(self->context), FL_TEXTURE(self));

  G_OBJECT_CLASS(fl_webrtc_video_texture_parent_class)->dispose(object);
  return;
}

static void fl_webrtc_video_texture_finalize(GObject *object) {
  FL_WEBRTC_LOGI("Enter");
  FlWebrtcVideoTexture *thiz = FL_WEBRTC_VIDEO_TEXTURE(object);
  g_clear_pointer(&thiz->pending.buf, g_free);
  g_clear_pointer(&thiz->rendering.buf, g_free);
  g_mutex_clear(&thiz->lock);

  delete thiz->rtc_frame;
  thiz->rtc_frame = nullptr;

  g_object_unref(thiz->channel);
  thiz->channel = nullptr;

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
  FL_WEBRTC_LOGI("Enter");
  return;
}

static FlMethodErrorResponse*
texture_channel_listen_cb(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FL_WEBRTC_LOGI("value:%s", fl_value_to_string(args));
  return nullptr;
}

static FlMethodErrorResponse*
texture_channel_cancel_cb(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FL_WEBRTC_LOGI("value:%s", fl_value_to_string(args));
  return nullptr;
}

FlWebrtcVideoTexture *fl_webrtc_video_texture_new(FlWebrtcContext *context) {
  FlWebrtcVideoTexture *video_texture;
  video_texture = FL_WEBRTC_VIDEO_TEXTURE((g_object_new(fl_webrtc_video_texture_get_type(), NULL)));
  g_assert(video_texture != NULL);

  video_texture->context = context;
  video_texture->rtc_frame = new FlWebrtcVideoFrameListener(video_texture);

  g_mutex_init(&video_texture->lock);
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  gchar *channel_name = g_strdup_printf("FlutterWebRTC/Texture%" PRIi64, reinterpret_cast<int64_t>(video_texture));
  FL_WEBRTC_LOGI("video_texture channel:%s", channel_name);

  video_texture->channel = fl_event_channel_new(fl_webrtc_context_messenger(context), channel_name, FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(video_texture->channel,
      texture_channel_listen_cb, texture_channel_cancel_cb, video_texture, nullptr);
  fl_texture_registrar_register_texture(fl_webrtc_context_texture_registrar(context), FL_TEXTURE(video_texture));
  return video_texture;
}

int64_t fl_webrtc_video_texture_id(FlWebrtcVideoTexture *video_texture) {
  return reinterpret_cast<int64_t>(video_texture);
}
