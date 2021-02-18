/*
 * retarget_debug.cpp
 *
 * debug output using <bkpt>
 * based on https://github.com/micro-os-plus
 *
 * kochetkov
 */



#ifndef _RETARGET_BKPT_
#define _RETARGET_BKPT_

#ifdef __cplusplus
extern "C" {
#endif

#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


//#define USE_SEMIHOSTING_VIA_DBG // un/define to use retarget

#ifdef USE_SEMIHOSTING_VIA_DBG
#define SHTRACE(fmt,args...) trace_bkpt("%s(%s)-" fmt "\n", __FILE__, __FUNCTION__, ##args)
#warning "SEMIHOSTING enabled! Must be turned off on release!"
#else
#define SHTRACE(fmt,args...) trace_wrapper("%s(%s)-" fmt "\n", __FILE__, __FUNCTION__, ##args)
#endif

// размер буфера для trace_write
#define OS_INTEGER_TRACE_TMP_ARRAY_SIZE  (100)

// Semihosting operations.
enum OperationNumber
{
	// Regular operations
	SEMIHOSTING_EnterSVC = 0x17,
	SEMIHOSTING_ReportException = 0x18,
	SEMIHOSTING_SYS_CLOSE = 0x02,
	SEMIHOSTING_SYS_CLOCK = 0x10,
	SEMIHOSTING_SYS_ELAPSED = 0x30,
	SEMIHOSTING_SYS_ERRNO = 0x13,
	SEMIHOSTING_SYS_FLEN = 0x0C,
	SEMIHOSTING_SYS_GET_CMDLINE = 0x15,
	SEMIHOSTING_SYS_HEAPINFO = 0x16,
	SEMIHOSTING_SYS_ISERROR = 0x08,
	SEMIHOSTING_SYS_ISTTY = 0x09,
	SEMIHOSTING_SYS_OPEN = 0x01,
	SEMIHOSTING_SYS_READ = 0x06,
	SEMIHOSTING_SYS_READC = 0x07,
	SEMIHOSTING_SYS_REMOVE = 0x0E,
	SEMIHOSTING_SYS_RENAME = 0x0F,
	SEMIHOSTING_SYS_SEEK = 0x0A,
	SEMIHOSTING_SYS_SYSTEM = 0x12,
	SEMIHOSTING_SYS_TICKFREQ = 0x31,
	SEMIHOSTING_SYS_TIME = 0x11,
	SEMIHOSTING_SYS_TMPNAM = 0x0D,
	SEMIHOSTING_SYS_WRITE = 0x05,
	SEMIHOSTING_SYS_WRITEC = 0x03,
	SEMIHOSTING_SYS_WRITE0 = 0x04,
	// Codes returned by SEMIHOSTING_ReportException
	ADP_Stopped_ApplicationExit = ((2 << 16) + 38),
	ADP_Stopped_RunTimeError = ((2 << 16) + 35)
};

// SWI numbers and reason codes for RDI (Angel) monitors.
#define AngelSWI_ARM                    0x123456

#ifdef __thumb__ // TODO: вернуть возможность с SWI, чтобы было прям вообще универсально
#define AngelSWI                    0xAB
#else
#error Not a Thumb instruction set is not supported! Try SWI
#endif

// For thumb only architectures use the BKPT instruction instead of SWI.
#define AngelSWIInsn                    "bkpt"
#define AngelSWIAsm                     bkpt

static inline int __attribute__((always_inline)) call_host (int reason, void* arg)
{
	int value;
	asm volatile (
			" mov r0, %[rsn]  \n"
			" mov r1, %[arg]  \n"
			" " AngelSWIInsn " %[swi] \n"
			" mov %[val], r0"
			/* Outputs */
			: [val] "=r" (value)
			/* Inputs */
			: [rsn] "r" (reason), [arg] "r" (arg), [swi] "i" (AngelSWI)
			// Clobbers r0 and r1, and lr if in supervisor mode
			: "r0", "r1", "r2", "r3", "ip", "lr", "memory", "cc"
	);

	// Accordingly to page 13-77 of ARM DUI 0040D other registers
	// can also be clobbered. Some memory positions may also be
	// changed by a system call, so they should not be kept in
	// registers. Note: we are assuming the manual is right and
	// Angel is respecting the APCS.
	return value;
}

static ssize_t trace_write (const char* buf, size_t nbyte);

#ifdef USE_SEMIHOSTING_VIA_DBG
ssize_t trace_bkpt(const char* fmt, ...);
#else
void trace_wrapper(const char* fmt, ...);
#endif

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _RETARGET_BKPT_ */
