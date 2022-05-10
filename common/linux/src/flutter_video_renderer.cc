#include "flutter_video_renderer.h"
#include "flutter_glog.h"
#include <stdint.h>
#include <inttypes.h>

namespace flutter_webrtc_plugin {

FlMethodErrorResponse*
FlutterVideoRenderer::FlEventChannelListenCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterVideoRenderer *thiz = reinterpret_cast<FlutterVideoRenderer *>(user_data);
  thiz->event_sink_ = true;
  return nullptr;
}

FlMethodErrorResponse*
FlutterVideoRenderer::FlEventChannelCancelCB(FlEventChannel* channel, FlValue* args, gpointer user_data) {
  FlutterVideoRenderer *thiz = reinterpret_cast<FlutterVideoRenderer *>(user_data);
  thiz->event_sink_ = false;
  return nullptr;
}

FlutterVideoRenderer::FlutterVideoRenderer(FlTextureRegistrar *registrar,
                                           FlBinaryMessenger *messenger)
    : registrar_(registrar), unregistered(false) {

  texture_ = fl_webrtc_video_texture_new();
  texture_id_ = fl_webrtc_video_texture_id(texture_);
  fl_webrtc_video_texture_set_handler(texture_, CopyPixelCB, this, nullptr);
  fl_texture_registrar_register_texture(registrar_, FL_TEXTURE(texture_));

  event_sink_ = false;
  std::string name = "FlutterWebRTC/Texture" + std::to_string(texture_id_);
  g_autoptr(FlStandardMethodCodec) codec = fl_standard_method_codec_new();
  event_channel_ = fl_event_channel_new(messenger, name.c_str(), FL_METHOD_CODEC(codec));
  fl_event_channel_set_stream_handlers(event_channel_, FlEventChannelListenCB, FlEventChannelCancelCB, this, nullptr);

  FL_LOGI("Texture Channel:%s\n", name.c_str());
}

FlutterVideoRenderer::~FlutterVideoRenderer() {
  g_object_unref(texture_);
  g_object_unref(event_channel_);
}

gboolean FlutterVideoRenderer::CopyPixelCB(FlWebrtcVideoTexture* texture,
                              const uint8_t** out_buffer,
                              uint32_t* width,
                              uint32_t* height,
                              GError** error,
                              gpointer user_data) {
  FlutterVideoRenderer *thiz = reinterpret_cast<FlutterVideoRenderer *>(user_data);
  //FL_LOGI("texture_id:%" PRIi64 " %p to flutter", thiz->texture_id_, thiz);
  thiz->mutex_.lock();
  if (thiz->pixel_buffer_.get() && thiz->frame_.get()) {
    if (thiz->pixel_buffer_->width != thiz->frame_->width() ||
        thiz->pixel_buffer_->height != thiz->frame_->height()) {
      size_t buffer_size = (thiz->frame_->width() * thiz->frame_->height()) * (32 >> 3);
      thiz->rgb_buffer_.reset(new uint8_t[buffer_size]);
      thiz->pixel_buffer_->width = thiz->frame_->width();
      thiz->pixel_buffer_->height = thiz->frame_->height();
    }

    thiz->frame_->ConvertToARGB(
        RTCVideoFrame::Type::kABGR, thiz->rgb_buffer_.get(), 0,
        (int)thiz->pixel_buffer_->width, (int)thiz->pixel_buffer_->height);

    thiz->pixel_buffer_->buffer = thiz->rgb_buffer_.get();

    *out_buffer = thiz->pixel_buffer_->buffer;
    *width = thiz->pixel_buffer_->width;
    *height = thiz->pixel_buffer_->height;
    thiz->mutex_.unlock();
    return true;
  }
  thiz->mutex_.unlock();
  return true;
}

void FlutterVideoRenderer::OnFrame(scoped_refptr<RTCVideoFrame> frame) {
  //FL_LOGI("texture_id:%" PRIi64 " %p frome rtc", texture_id_, this);
  if (!first_frame_rendered) {
    if (event_sink_) {
      //FL_LOGI("texture_id:%" PRIi64 " notify didFirstFrameRendered", texture_id_);
      g_autoptr(FlValue) params = fl_value_new_map();
      fl_value_set_string_take(params, "event", fl_value_new_string("didFirstFrameRendered"));
      fl_value_set_string_take(params, "id", fl_value_new_int(texture_id_));
      fl_event_channel_send(event_channel_, params, nullptr, nullptr);
    }
    pixel_buffer_.reset(new PixelBuffer());
    first_frame_rendered = true;
  }
  if (rotation_ != frame->rotation()) {
    if (event_sink_) {
      g_autoptr(FlValue) params = fl_value_new_map();
      fl_value_set_string_take(params, "event", fl_value_new_string("didTextureChangeRotation"));
      fl_value_set_string_take(params, "id", fl_value_new_int(texture_id_));
      fl_value_set_string_take(params, "rotation", fl_value_new_int(frame->rotation()));
      fl_event_channel_send(event_channel_, params, nullptr, nullptr);
    }
    rotation_ = frame->rotation();
  }
  if (last_frame_size_.width != frame->width() ||
      last_frame_size_.height != frame->height()) {
    if (event_sink_) {
      g_autoptr(FlValue) params = fl_value_new_map();
      fl_value_set_string_take(params, "event", fl_value_new_string("didTextureChangeVideoSize"));
      fl_value_set_string_take(params, "id", fl_value_new_int(texture_id_));
      fl_value_set_string_take(params, "width", fl_value_new_int(frame->width()));
      fl_value_set_string_take(params, "height", fl_value_new_int(frame->height()));
      fl_event_channel_send(event_channel_, params, nullptr, nullptr);
    }
    last_frame_size_ = {(size_t)frame->width(), (size_t)frame->height()};
  }
  mutex_.lock();
  frame_ = frame;
  mutex_.unlock();
  fl_texture_registrar_mark_texture_frame_available(registrar_, FL_TEXTURE(texture_));
}

void FlutterVideoRenderer::SetVideoTrack(scoped_refptr<RTCVideoTrack> track) {
  if (track_ != track) {
    if (track_) track_->RemoveRenderer(this);
    track_ = track;
    last_frame_size_ = {0, 0};
    first_frame_rendered = false;
    if (track_) track_->AddRenderer(this);
  }
}


bool FlutterVideoRenderer::CheckMediaStream(std::string mediaId) {
  if (0 == mediaId.size() || 0 == media_stream_id.size()) {
    return false;
  }
  return mediaId == media_stream_id;
}

bool FlutterVideoRenderer::CheckVideoTrack(std::string mediaId) {
  if (0 == mediaId.size() || !track_) {
    return false;
  }
  return mediaId == track_->id().std_string();
}

void FlutterVideoRenderer::Dispose() {
  if (!unregistered) {
    fl_texture_registrar_unregister_texture(registrar_, FL_TEXTURE(texture_));
    unregistered = true;
  }
}

FlutterVideoRendererManager::FlutterVideoRendererManager(
    FlutterWebRTCBase *base)
    : base_(base) {}

void FlutterVideoRendererManager::CreateVideoRendererTexture(FlMethodCall *result) {
  std::unique_ptr<FlutterVideoRenderer> texture(
      new FlutterVideoRenderer(base_->textures_, base_->messenger_));
  int64_t texture_id = texture->texture_id();
  renderers_[texture_id] = std::move(texture);

  g_autoptr(FlValue) params = fl_value_new_map();
  fl_value_set_string_take(params, "textureId", fl_value_new_int(texture_id));
  fl_method_call_respond_success(result, params, nullptr);
}

void FlutterVideoRendererManager::SetMediaStream(int64_t texture_id,
                                                 const std::string& stream_id) {
  scoped_refptr<RTCMediaStream> stream =
      base_->MediaStreamForId(stream_id);
  auto it = renderers_.find(texture_id);
  if (it != renderers_.end()) {
    FlutterVideoRenderer *renderer = it->second.get();
    if (stream.get()) {
      auto video_tracks = stream->video_tracks();
      if (video_tracks.size() > 0) {
        renderer->SetVideoTrack(video_tracks[0]);
        renderer->media_stream_id = stream_id;
      }
    } else {
      renderer->SetVideoTrack(nullptr);
    }
  }
}

void FlutterVideoRendererManager::VideoRendererDispose(
    int64_t texture_id, FlMethodCall *result) {
  auto it = renderers_.find(texture_id);
  if (it != renderers_.end()) {
    it->second->Dispose();
    renderers_.erase(it);
    fl_method_call_respond_success(result, nullptr, nullptr);
    return;
  }
  fl_method_call_respond_error(result, "VideoRendererDisposeFailed",
                               "VideoRendererDispose() texture not found!",
                               nullptr, nullptr);
}


}  // namespace flutter_webrtc_plugin
