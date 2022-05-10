#ifndef FLUTTER_WEBRTC_RTC_VIDEO_RENDERER_HXX
#define FLUTTER_WEBRTC_RTC_VIDEO_RENDERER_HXX

#include <mutex>

#include "flutter_webrtc_base.h"
#include "flutter_video_texture.h"

#include "rtc_video_frame.h"
#include "rtc_video_renderer.h"

namespace flutter_webrtc_plugin {

class FlutterVideoRenderer: public RTCVideoRenderer<scoped_refptr<RTCVideoFrame>> {
 public:
  FlutterVideoRenderer(FlTextureRegistrar *registrar, FlBinaryMessenger *messenger);
  ~FlutterVideoRenderer();
  virtual void OnFrame(scoped_refptr<RTCVideoFrame> frame) override;
  void SetVideoTrack(scoped_refptr<RTCVideoTrack> track);
  int64_t texture_id() { return texture_id_; }
  bool CheckMediaStream(std::string mediaId);
  bool CheckVideoTrack(std::string mediaId);
  void Dispose();

  std::string media_stream_id;
 private:
  struct FrameSize {
    size_t width;
    size_t height;
  };

  struct PixelBuffer {
    PixelBuffer():buffer(nullptr), width(0), height(0){}
    const uint8_t *buffer;
    size_t width;
    size_t height;
  };

  static gboolean CopyPixelCB(FlWebrtcVideoTexture* texture,
                              const uint8_t** out_buffer,
                              uint32_t* width,
                              uint32_t* height,
                              GError** error,
                              gpointer user_data);

  static FlMethodErrorResponse* FlEventChannelListenCB(FlEventChannel* channel,
                                                       FlValue* args,
                                                       gpointer user_data);

  static FlMethodErrorResponse* FlEventChannelCancelCB(FlEventChannel* channel,
                                                       FlValue* args,
                                                       gpointer user_data);

  FrameSize last_frame_size_ = {0, 0};
  bool first_frame_rendered = false;
  FlTextureRegistrar *registrar_ = nullptr;
  bool event_sink_;
  FlEventChannel *event_channel_;
  int64_t texture_id_ = -1;
  scoped_refptr<RTCVideoTrack> track_ = nullptr;
  scoped_refptr<RTCVideoFrame> frame_;
  FlWebrtcVideoTexture *texture_;
  std::shared_ptr<PixelBuffer> pixel_buffer_;
  mutable std::shared_ptr<uint8_t> rgb_buffer_;
  mutable std::mutex mutex_;
  RTCVideoFrame::VideoRotation rotation_ = RTCVideoFrame::kVideoRotation_0;
  bool unregistered;
};

class FlutterVideoRendererManager {
 public:
  FlutterVideoRendererManager(FlutterWebRTCBase *base);

  void CreateVideoRendererTexture(FlMethodCall *result);

  void SetMediaStream(int64_t texture_id,
                      const std::string& stream_id);

  void VideoRendererDispose(int64_t texture_id, FlMethodCall *result);

 private:
  FlutterWebRTCBase *base_;
  std::map<int64_t, std::unique_ptr<FlutterVideoRenderer>> renderers_;
};

}  // namespace flutter_webrtc_plugin

#endif  // !FLUTTER_WEBRTC_RTC_VIDEO_RENDERER_HXX
