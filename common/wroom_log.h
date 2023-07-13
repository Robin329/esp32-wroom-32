#ifndef __WROOM_H__
#define __WROOM_H__
#include <stdio.h>

#define WROOM_TAG "WROOM"

#define WLOG(fmt, ...)                                                         \
	printf("%s [%s:%d] " fmt "\n", WROOM_TAG, __FUNCTION__, __LINE__,      \
	       ##__VA_ARGS__)

#endif // __WROOM_H__