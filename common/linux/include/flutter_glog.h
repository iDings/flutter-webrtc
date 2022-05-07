#ifndef __FL_WEBRTC_LOG_H__
#define __FL_WEBRTC_LOG_H__

#include <flutter_linux/flutter_linux.h>
#include <unistd.h>
#include <sys/syscall.h>

G_BEGIN_DECLS

#define FL_LOGD(fmt, argv...)  g_message("%ld %s:%d |D|" fmt, syscall(SYS_gettid), __func__, __LINE__, ##argv)
#define FL_LOGI(fmt, argv...)  g_message("%ld %s:%d |I|" fmt, syscall(SYS_gettid), __func__, __LINE__, ##argv)
#define FL_LOGW(fmt, argv...)  g_message("%ld %s:%d |W|" fmt, syscall(SYS_gettid), __func__, __LINE__, ##argv)

#define FL_TRACE(fmt, argv...) FL_LOGI(fmt, ##argv)

G_END_DECLS
#endif
