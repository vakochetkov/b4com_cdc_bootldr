/*
 * retarget_bkpt.c
 *
 *  Created on: 11 февр. 2020 г.
 *      Author: kochetkov
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "retarget_bkpt.h"

ssize_t trace_write (const char* buf, size_t nbyte)
{
	// Since the single character debug channel is quite slow, try to
	// optimise and send a null terminated string, if possible.
	if (buf[nbyte] == '\0')	{
		// send string
		call_host (SEMIHOSTING_SYS_WRITE0, (void*) buf);
	} else {
		// If not, use a local buffer to speed things up
		char tmp[OS_INTEGER_TRACE_TMP_ARRAY_SIZE];
		size_t togo = nbyte;
		while (togo > 0)
		{
			unsigned int n = ((togo < sizeof(tmp)) ? togo : sizeof(tmp));
			unsigned int i = 0;
			for (; i < n; ++i, ++buf) {
				tmp[i] = *buf;
			}
			tmp[i] = '\0';
			call_host (SEMIHOSTING_SYS_WRITE0, (void*) tmp);
			togo -= n;
		}
	}
	// All bytes written
	return (ssize_t) nbyte;
}

#ifdef USE_SEMIHOSTING_VIA_DBG
ssize_t trace_bkpt(const char* fmt, ...)
{
	// выделяем буфер под форматированную строку
	char tmp[OS_INTEGER_TRACE_TMP_ARRAY_SIZE];
	memset(tmp, 0, sizeof tmp);
	// берем список аргументов
	va_list va;
	va_start(va, fmt);
	// безопасно форматируем
	vsnprintf(tmp, OS_INTEGER_TRACE_TMP_ARRAY_SIZE-1, fmt, va);
	va_end(va);
	// выплевываем хосту через отладчик
	const ssize_t nbyte = trace_write(tmp, sizeof tmp);
	return nbyte;
}
#else
void trace_wrapper(const char* fmt, ...) { }
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

