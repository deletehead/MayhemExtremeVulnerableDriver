#pragma once

#include <ntifs.h>
#include <ntddk.h>

#define IOCTL_MEMCPY				CTL_CODE(FILE_DEVICE_UNKNOWN, 0, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_ELEVATE_PROCESS		CTL_CODE(FILE_DEVICE_UNKNOWN, 1, METHOD_BUFFERED, FILE_ANY_ACCESS)

typedef struct _IOCTL_MEMCPY_BUFFER {
	void* pSource;
	void* pDestination;
	size_t Size;
} IOCTL_MEMCPY_BUFFER, * PIOCTL_MEMCPY_BUFFER;