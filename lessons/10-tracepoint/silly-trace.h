// Define where all tracepoints within the file will belong to
#undef TRACE_SYSTEM
#define TRACE_SYSTEM silly

// Make sure the magic of kernel trace can include this header file
#if !defined(_SILLY_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _SILLY_TRACE_H

#include <linux/tracepoint.h>

TRACE_EVENT(me_silly,

	TP_PROTO(unsigned long time, unsigned long count),

	TP_ARGS(time, count),

	TP_STRUCT__entry(
		__field(	unsigned long,	time	)
		__field(	unsigned long,	count	)
	),

	TP_fast_assign(
		__entry->time = jiffies;
		__entry->count = count;
	),

	TP_printk("time=%lu count=%lu", __entry->time, __entry->count)
);

#endif /* _SILLY_TRACE_H */

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE silly-trace
#include <trace/define_trace.h>
