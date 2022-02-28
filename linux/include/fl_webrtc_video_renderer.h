#ifndef __FL_WEBRTC_VIDEO_RENDERER_H__
#define __FL_WEBRTC_VIDEO_RENDERER_H__

#include <flutter_linux/flutter_linux.h>

G_BEGIN_DECLS

#define FL_TYPE_WEBRTC_VIDEO_RENDERER fl_webrtc_video_renderer_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcVideoRenderer, fl_webrtc_video_renderer, FL, WEBRTC_VIDEO_RENDERER, GObject)

#define FL_TYPE_WEBRTC_VIDEO_PIXEL_BUFFER_TEXTURE fl_webrtc_video_pixel_buffer_texture_get_type()
G_DECLARE_FINAL_TYPE(FlWebrtcVideoPixelBufferTexture,
                     fl_webrtc_video_pixel_buffer_texture,
                     FL,
                     WEBRTC_VIDEO_PIXEL_BUFFER_TEXTURE,
                     FlPixelBufferTexture)

#define FL_TYPE_WEBRTC_VIDEO_RENDERER_METHOD fl_webrtc_video_renderer_method_get_type()
G_DECLARE_INTERFACE(FlWebrtcVideoRendererMethod,
                    fl_webrtc_video_renderer_method,
                    FL,
                    WEBRTC_VIDEO_RENDER_METHOD,
                    GObject)
struct _FlWebrtcVideoRendererMethodInterface {
    GTypeInterface g_iface;

    void (*create_texture)(FlWebrtcVideoRenderer* self, FlMethodCall* method_call);
    void (*set_media_stream)(FlWebrtcVideoRenderer* self, FlMethodCall* method_call);
    void (*dispose)(FlWebrtcVideoRenderer* self, FlMethodCall* method_call);
};

G_END_DECLS

#endif
