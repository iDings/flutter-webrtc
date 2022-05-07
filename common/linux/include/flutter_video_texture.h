#ifndef __FL_WEBRTC_VIDEO_TEXTURE_H__
#define __FL_WEBRTC_VIDEO_TEXTURE_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_VIDEO_TEXTURE fl_webrtc_video_texture_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcVideoTexture,
                     fl_webrtc_video_texture,
                     FL,
                     WEBRTC_VIDEO_TEXTURE,
                     FlPixelBufferTexture)

typedef gboolean (*FlWebrtcVideoTextureCopyHandler)(
    FlWebrtcVideoTexture* texture,
    const uint8_t** out_buffer,
    uint32_t* width,
    uint32_t* height,
    GError** error,
    gpointer user_data);

FlWebrtcVideoTexture *fl_webrtc_video_texture_new(void);
int64_t fl_webrtc_video_texture_id(FlWebrtcVideoTexture *video_texture);
void fl_webrtc_video_texture_set_handler(FlWebrtcVideoTexture *self,
    FlWebrtcVideoTextureCopyHandler on_copy, gpointer user_data, GDestroyNotify destroy_notify);

G_END_DECLS

#endif
