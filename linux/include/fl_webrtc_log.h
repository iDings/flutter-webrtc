#ifndef __FL_WEBRTC_LOG_H__
#define __FL_WEBRTC_LOG_H__

#include <flutter_linux/flutter_linux.h>
#include <unistd.h>
#include <sys/syscall.h>

G_BEGIN_DECLS

#define FL_WEBRTC_LOGI(fmt, argv...)  g_message("%ld %s:%d " fmt, syscall(SYS_gettid), __func__, __LINE__, ##argv)
#define FL_WEBRTC_TRACE(fmt, argv...) FL_WEBRTC_LOGI(fmt, ##argv)

G_END_DECLS
#endif
