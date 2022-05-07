#ifndef __FL_WEBRTC_VIDEO_TEXTURE_H__
#define __FL_WEBRTC_VIDEO_TEXTURE_H__

#include <flutter_linux/flutter_linux.h>
#include "fl_webrtc_context.h"

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_VIDEO_TEXTURE fl_webrtc_video_texture_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcVideoTexture,
                     fl_webrtc_video_texture,
                     FL,
                     WEBRTC_VIDEO_TEXTURE,
                     FlPixelBufferTexture)

FlWebrtcVideoTexture *fl_webrtc_video_texture_new(FlWebrtcContext *context);
int64_t fl_webrtc_video_texture_id(FlWebrtcVideoTexture *video_texture);

G_END_DECLS

#endif
