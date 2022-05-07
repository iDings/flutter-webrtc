#include "flutter_media_stream.h"
#include "flutter_glog.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480
#define DEFAULT_FPS 30

namespace flutter_webrtc_plugin {

void FlutterMediaStream::GetUserMedia(
    const FlValue *constraints,
    FlMethodCall *result) {
  std::string uuid = base_->GenerateUUID();
  scoped_refptr<RTCMediaStream> stream =
      base_->factory_->CreateStream(uuid.c_str());

  g_autoptr(FlValue) params = fl_value_new_map();
  fl_value_set_string_take(params, "streamId", fl_value_new_string(uuid.c_str()));

  FlValue *constraints_ = const_cast<FlValue *>(constraints);

  FlValue *audio_ = fl_value_lookup_string(constraints_, "audio");
  bool has_audio = false;
  if (audio_) {
    has_audio = ((fl_value_get_type(audio_) == FL_VALUE_TYPE_BOOL) &&
        fl_value_get_bool(audio_)) || (fl_value_get_type(audio_) == FL_VALUE_TYPE_MAP);
    if (has_audio) GetUserAudio(constraints, stream, params);
  }
  if (!has_audio) fl_value_set_string_take(params, "audioTracks", fl_value_new_list());

  FlValue *video_ = fl_value_lookup_string(constraints_, "video");
  if (video_) {
    if (fl_value_get_type(video_) == FL_VALUE_TYPE_BOOL) {
      if (fl_value_get_bool(video_) == true) {
        GetUserVideo(constraints, stream, params);
      }
    } else if (fl_value_get_type(video_) == FL_VALUE_TYPE_MAP) {
        GetUserVideo(constraints, stream, params);
    }
  }

  base_->local_streams_[uuid] = stream;
  FL_LOGI("respond user_media:%s\n", fl_value_to_string(params));
  fl_method_call_respond_success(result, params, nullptr);
}

void addDefaultAudioConstraints(
    scoped_refptr<RTCMediaConstraints> audioConstraints) {
  audioConstraints->AddOptionalConstraint("googNoiseSuppression", "true");
  audioConstraints->AddOptionalConstraint("googEchoCancellation", "true");
  audioConstraints->AddOptionalConstraint("echoCancellation", "true");
  audioConstraints->AddOptionalConstraint("googEchoCancellation2", "true");
  audioConstraints->AddOptionalConstraint("googDAEchoCancellation", "true");
}

std::string getSourceIdConstraint(const FlValue *mediaConstraints) {
  FlValue *mediaConstraints_ = const_cast<FlValue *>(mediaConstraints);
  FlValue *optional_ = fl_value_lookup_string(mediaConstraints_, "optional");
  if (optional_ && (fl_value_get_type(optional_) == FL_VALUE_TYPE_LIST)) {
    size_t size = fl_value_get_length(optional_);
    for (size_t i = 0; i < size; i++) {
      FlValue *option = fl_value_get_list_value(optional_, i);
      if (fl_value_get_type(option) == FL_VALUE_TYPE_MAP) {
        FlValue *sid = fl_value_lookup_string(option, "sourceId");
        if (sid) return fl_value_get_string(sid);
      }
    }
  }

  return "";
}

std::string getDeviceIdConstraint(const FlValue *mediaConstraints) {
  FlValue *mediaConstraints_ = const_cast<FlValue *>(mediaConstraints);
  FlValue *deviceId_ = fl_value_lookup_string(mediaConstraints_, "deviceId");
  if (deviceId_) return fl_value_get_string(deviceId_);
  return "";
}

void FlutterMediaStream::GetUserAudio(const FlValue *constraints,
                                      scoped_refptr<RTCMediaStream> stream,
                                      FlValue *params) {
  bool enable_audio = false;
  scoped_refptr<RTCMediaConstraints> audioConstraints;
  std::string sourceId;
  std::string deviceId;

  FlValue *constraints_ = const_cast<FlValue *>(constraints);
  FlValue *audio_ = fl_value_lookup_string(constraints_, "audio");
  if (audio_) {
    FlValueType vtype = fl_value_get_type(audio_);
    if (vtype == FL_VALUE_TYPE_BOOL) {
      audioConstraints = RTCMediaConstraints::Create();
      addDefaultAudioConstraints(audioConstraints);
      enable_audio = fl_value_get_bool(audio_);
      sourceId = "";
      deviceId = "";
    }

    if (vtype == FL_VALUE_TYPE_MAP) {
      sourceId = getSourceIdConstraint(audio_);
      deviceId = getDeviceIdConstraint(audio_);
      audioConstraints = base_->ParseMediaConstraints(audio_);
      enable_audio = true;
    }
  }

  // Selecting audio input device by sourceId and audio output device by deviceId
  if (enable_audio) {
    char strRecordingName[256];
    char strRecordingGuid[256];
    int playout_devices = base_->audio_device_->PlayoutDevices();
    int recording_devices = base_->audio_device_->RecordingDevices();

    for (uint16_t i = 0; i < recording_devices; i++) {
      base_->audio_device_->RecordingDeviceName(i, strRecordingName, strRecordingGuid);
      if (sourceId != "" && sourceId == strRecordingGuid) {
        base_->audio_device_->SetRecordingDevice(i);
      }
    }

    char strPlayoutName[256];
    char strPlayoutGuid[256];
    for (uint16_t i = 0; i < playout_devices; i++) {
      base_->audio_device_->PlayoutDeviceName(i, strPlayoutName, strPlayoutGuid);
      if (deviceId != "" && deviceId == strPlayoutGuid) {
        base_->audio_device_->SetPlayoutDevice(i);
      }
    }

    scoped_refptr<RTCAudioSource> source = base_->factory_->CreateAudioSource("audio_input");
    std::string uuid = base_->GenerateUUID();
    scoped_refptr<RTCAudioTrack> track = base_->factory_->CreateAudioTrack(source, uuid.c_str());
    std::string track_id = track->id().std_string();

    // Ownership taking to params
    FlValue *track_info = fl_value_new_map();
    fl_value_set_string_take(track_info, "id", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(track_info, "label", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(track_info, "kind", fl_value_new_string(track->kind().std_string().c_str()));
    fl_value_set_string_take(track_info, "enabled", fl_value_new_bool(track->enabled()));

    FlValue *audioTracks = fl_value_new_list();
    fl_value_append_take(audioTracks, track_info);
    fl_value_set_string_take(params, "audioTracks", audioTracks);

    stream->AddTrack(track);
    base_->local_tracks_[track->id().std_string()] = track;
  }
}

std::string getFacingMode(const FlValue *mediaConstraints) {
  FlValue *mediaConstraintsV = const_cast<FlValue *>(mediaConstraints);
  FlValue *facingModeV = fl_value_lookup_string(mediaConstraintsV, "facingMode");
  if (facingModeV) return fl_value_get_string(facingModeV);
  return "";
}

// raw, no-ref
static FlValue *getConstrainInt(const FlValue *constraints, const std::string& key) {
  FlValue *constraintsV = const_cast<FlValue *>(constraints);
  FlValue *value = fl_value_lookup_string(constraintsV, key.c_str());
  if(value) {
    FlValueType vtype = fl_value_get_type(value);
    if (vtype == FL_VALUE_TYPE_INT) return value;
    if (vtype == FL_VALUE_TYPE_MAP) {
      FlValue *idealV = fl_value_lookup_string(value, "ideal");
      if (fl_value_get_type(idealV) == FL_VALUE_TYPE_INT) return idealV;
    }
  }

  return nullptr;
}

void FlutterMediaStream::GetUserVideo(const FlValue *constraints,
                                      scoped_refptr<RTCMediaStream> stream,
                                      FlValue *params) {
  FlValue *video_mandatory = nullptr;
  FlValue *constraintsV = const_cast<FlValue *>(constraints);
  FlValue *video_constraints = fl_value_lookup_string(constraintsV, "video");
  if (fl_value_get_type(video_constraints) == FL_VALUE_TYPE_MAP) {
    video_mandatory = fl_value_lookup_string(video_constraints, "mandatory");
  }

  std::string facing_mode = getFacingMode(video_constraints);
  //bool isFacing = facing_mode == "" || facing_mode != "environment";
  std::string sourceId = getSourceIdConstraint(video_constraints);

  FlValue *widthValue = getConstrainInt(video_constraints, "width");

  if (widthValue == nullptr)
    widthValue = findEncodableValue(video_mandatory, "minWidth");

  if (widthValue == nullptr)
    widthValue = findEncodableValue(video_mandatory, "width");

  FlValue *heightValue = getConstrainInt(video_constraints, "height");

  if(heightValue == nullptr)
    heightValue = findEncodableValue(video_mandatory, "minHeight");

  if (heightValue == nullptr)
    heightValue = findEncodableValue(video_mandatory, "height");

  FlValue *fpsValue = getConstrainInt(video_constraints, "frameRate");

  if(fpsValue == nullptr)
    fpsValue = findEncodableValue(video_mandatory, "minFrameRate");

  if (fpsValue == nullptr)
    fpsValue = findEncodableValue(video_mandatory, "frameRate");


  scoped_refptr<RTCVideoCapturer> video_capturer;
  char strNameUTF8[256];
  char strGuidUTF8[256];
  int nb_video_devices = base_->video_device_->NumberOfDevices();

  int32_t width = toInt(widthValue, DEFAULT_WIDTH);
  int32_t height = toInt(heightValue, DEFAULT_HEIGHT);
  int32_t fps = toInt(fpsValue, DEFAULT_FPS);

  for (int i = 0; i < nb_video_devices; i++) {
    base_->video_device_->GetDeviceName(i, strNameUTF8, 256, strGuidUTF8, 256);
    if (sourceId != "" && sourceId == strGuidUTF8) {
      video_capturer = base_->video_device_->Create(strNameUTF8, i, width, height, fps);
      break;
    }
  }

  if (nb_video_devices == 0) return;

  if (!video_capturer.get()) {
    base_->video_device_->GetDeviceName(0, strNameUTF8, 128, strGuidUTF8, 128);
    video_capturer =
        base_->video_device_->Create(strNameUTF8, 0, width, height, fps);
  }

  if (!video_capturer.get()) return;

  const char* video_source_label = "video_input";
  scoped_refptr<RTCVideoSource> source = base_->factory_->CreateVideoSource(
      video_capturer, video_source_label,
      base_->ParseMediaConstraints(video_constraints));

  std::string uuid = base_->GenerateUUID();
  scoped_refptr<RTCVideoTrack> track = base_->factory_->CreateVideoTrack(source, uuid.c_str());

  FlValue *info = fl_value_new_map();
  fl_value_set_string_take(info, "id", fl_value_new_string(track->id().std_string().c_str()));
  fl_value_set_string_take(info, "label", fl_value_new_string(track->id().std_string().c_str()));
  fl_value_set_string_take(info, "kind", fl_value_new_string(track->kind().std_string().c_str()));
  fl_value_set_string_take(info, "enabled", fl_value_new_bool(track->enabled()));
  FlValue *videoTracks = fl_value_new_list();
  fl_value_append_take(videoTracks, info);
  fl_value_set_string_take(params, "videoTracks", videoTracks);

  stream->AddTrack(track);
  base_->local_tracks_[track->id().std_string()] = track;
}

void FlutterMediaStream::GetSources(FlMethodCall *result) {
  FlValue *sources = fl_value_new_list();

  int nb_audio_devices = base_->audio_device_->RecordingDevices();
  char strNameUTF8[128];
  char strGuidUTF8[128];

  for (uint16_t i = 0; i < nb_audio_devices; i++) {
    base_->audio_device_->RecordingDeviceName(i, strNameUTF8, strGuidUTF8);
    FlValue *ai = fl_value_new_map();
    fl_value_set_string_take(ai, "label", fl_value_new_string(strNameUTF8));
    fl_value_set_string_take(ai, "deviceId", fl_value_new_string(strGuidUTF8));
    fl_value_set_string_take(ai, "facing", fl_value_new_string(""));
    fl_value_set_string_take(ai, "kind", fl_value_new_string("audioinput"));
    fl_value_append_take(sources, ai);
  }

  nb_audio_devices = base_->audio_device_->PlayoutDevices();
  for (uint16_t i = 0; i < nb_audio_devices; i++) {
    base_->audio_device_->PlayoutDeviceName(i, strNameUTF8, strGuidUTF8);
    FlValue *ao = fl_value_new_map();
    fl_value_set_string_take(ao, "label", fl_value_new_string(strNameUTF8));
    fl_value_set_string_take(ao, "deviceId", fl_value_new_string(strGuidUTF8));
    fl_value_set_string_take(ao, "facing", fl_value_new_string(""));
    fl_value_set_string_take(ao, "kind", fl_value_new_string("audiooutput"));
    fl_value_append_take(sources, ao);
  }

  int nb_video_devices = base_->video_device_->NumberOfDevices();
  for (int i = 0; i < nb_video_devices; i++) {
    base_->video_device_->GetDeviceName(i, strNameUTF8, 128, strGuidUTF8, 128);
    FlValue *video = fl_value_new_map();
    fl_value_set_string_take(video, "label", fl_value_new_string(strNameUTF8));
    fl_value_set_string_take(video, "deviceId", fl_value_new_string(strGuidUTF8));
    if (i == 1) {
      fl_value_set_string_take(video, "facing", fl_value_new_string("front"));
    } else {
      fl_value_set_string_take(video, "facing", fl_value_new_string("back"));
    }
    fl_value_set_string_take(video, "kind", fl_value_new_string("videoinput"));
    fl_value_append_take(sources, video);
  }

  g_autoptr(FlValue) params = fl_value_new_map();
  fl_value_set_string_take(params, "sources", sources);
  fl_method_call_respond_success(result, params, nullptr);
}

void FlutterMediaStream::MediaStreamGetTracks(const std::string& stream_id, FlMethodCall *result) {
  scoped_refptr<RTCMediaStream> stream = base_->MediaStreamForId(stream_id);
  if (!stream) {
    g_autofree gchar *message = g_strdup_printf("MediaStreamGetTracks() %s is null!", stream_id.c_str());
    fl_method_call_respond_error(result, "MediaStreamGetTracksFailed", message, nullptr, nullptr);
    return;
  }

  g_autoptr(FlValue) params = fl_value_new_map();

  FlValue *audioTracks = fl_value_new_list();
  auto audio_tracks = stream->audio_tracks();
  for (auto track : audio_tracks.std_vector()) {
    base_->local_tracks_[track->id().std_string()] = track;
    FlValue *info = fl_value_new_map();
    fl_value_set_string_take(info, "id", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(info, "label", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(info, "kind", fl_value_new_string(track->kind().std_string().c_str()));
    fl_value_set_string_take(info, "enabled", fl_value_new_bool(track->enabled()));
    fl_value_set_string_take(info, "remote", fl_value_new_bool(true));
    fl_value_set_string_take(info, "readyState", fl_value_new_string("live"));
    fl_value_append_take(audioTracks, info);
  }

  FlValue *videoTracks = fl_value_new_list();
  auto video_tracks = stream->video_tracks();
  for (auto track : video_tracks.std_vector()) {
    base_->local_tracks_[track->id().std_string()] = track;
    FlValue *info = fl_value_new_map();
    fl_value_set_string_take(info, "id", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(info, "label", fl_value_new_string(track->id().std_string().c_str()));
    fl_value_set_string_take(info, "kind", fl_value_new_string(track->kind().std_string().c_str()));
    fl_value_set_string_take(info, "enabled", fl_value_new_bool(track->enabled()));
    fl_value_set_string_take(info, "remote", fl_value_new_bool(true));
    fl_value_set_string_take(info, "readyState", fl_value_new_string("live"));
    fl_value_append_take(videoTracks, info);
  }

  fl_value_set_string_take(params, "audioTracks", audioTracks);
  fl_value_set_string_take(params, "videoTracks", videoTracks);
  fl_method_call_respond_success(result, params, nullptr);
  return;
}

void FlutterMediaStream::MediaStreamDispose(
    const std::string& stream_id, FlMethodCall *result) {
  scoped_refptr<RTCMediaStream> stream = base_->MediaStreamForId(stream_id);
  if (!stream) {
    g_autofree gchar *message = g_strdup_printf("%s not found", stream_id.c_str());
    fl_method_call_respond_error(result, "MediaStreamDisposeFailed", message, nullptr, nullptr);
    return;
  }

  vector<scoped_refptr<RTCAudioTrack>> audio_tracks = stream->audio_tracks();

  for (auto track : audio_tracks.std_vector()) {
    stream->RemoveTrack(track);
    base_->local_tracks_.erase(track->id().std_string());
  }

  vector<scoped_refptr<RTCVideoTrack>> video_tracks = stream->video_tracks();
  for (auto track : video_tracks.std_vector()) {
    stream->RemoveTrack(track);
    base_->local_tracks_.erase(track->id().std_string());
  }

  base_->RemoveStreamForId(stream_id);
  fl_method_call_respond_success(result, nullptr, nullptr);
  return;
}

void FlutterMediaStream::MediaStreamTrackSetEnable(
    const std::string& track_id, FlMethodCall *result) {
  fl_method_call_respond_not_implemented(result, nullptr);
}

void FlutterMediaStream::MediaStreamTrackSwitchCamera(
    const std::string& track_id, FlMethodCall *result) {
  fl_method_call_respond_not_implemented(result, nullptr);
}

void FlutterMediaStream::MediaStreamTrackDispose(
    const std::string& track_id, FlMethodCall *result) {
  fl_method_call_respond_success(result, nullptr, nullptr);
}
}  // namespace flutter_webrtc_plugin
