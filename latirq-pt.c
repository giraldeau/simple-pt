/*
 * latirq-pt.c
 *
 * Copyright (C) 2016 Suchakrapani Sharma <suchakrapani.sharma@polymtl.ca>
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation; only version 2.1 of the License.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */


#include <linux/module.h>
#include "wrapper/tracepoint.h"

static
void probe_core_critical_timing_start(void *ignore, unsigned long ip,
		unsigned long parent_ip, unsigned long flags, int preempt_cnt,
		cycles_t delta_ns)
{
	/*TODO:
	 * TRACE_EN*/
}

static
void probe_core_critical_timing_stop(void *ignore, unsigned long ip,
		unsigned long parent_ip, unsigned long flags, int preempt_cnt,
		cycles_t delta_ns)
{
	/*TODO:
	 * !TRACE_EN*/
}

static
void probe_core_critical_timing_hit(void *ignore, unsigned long ip,
		unsigned long parent_ip, unsigned long flags, int preempt_cnt,
		cycles_t delta_ns)
{
	/*TODO:
	 * save PT buffer
	 */
}


static
int __init latirq_init(void)
{
	int ret;
	/*

	tracker = latency_tracker_create("critical_timings");
	if (!tracker)
		goto error;
	latency_tracker_debugfs_setup_wakeup_pipe(tracker);
	 */
	ret = lttng_wrapper_tracepoint_probe_register("core_critical_timing_hit",
			probe_core_critical_timing_hit, NULL);
	ret = lttng_wrapper_tracepoint_probe_register("core_critical_timing_start",
			probe_core_critical_timing_start, NULL);
	ret = lttng_wrapper_tracepoint_probe_register("core_critical_timing_stop",
			probe_core_critical_timing_stop, NULL);
	WARN_ON(ret);

	/*TODO:
	 * pt_init()
	 * create buffer
	 * start_pt()
	 */

	printk("hello latirq!\n");
	goto end;

error:
	ret = -1;
end:
	return ret;
}
module_init(latirq_init);

static
void __exit latirq_exit(void)
{
	int ret;
	lttng_wrapper_tracepoint_probe_unregister("core_critical_timing_hit",
			probe_core_critical_timing_hit, NULL);
	lttng_wrapper_tracepoint_probe_unregister("core_critical_timing_start",
			probe_core_critical_timing_start, NULL);
	lttng_wrapper_tracepoint_probe_unregister("core_critical_timing_stop",
			probe_core_critical_timing_stop, NULL);

	/*TODO:
	 * stop_pt()
	 * pt_teardown()
	 * destroy buffer
	 */

	/*
	lttng_wrapper_tracepoint_probe_unregister("core_critical_timing_hit",
			probe_core_critical_timing_hit, NULL);
	tracepoint_synchronize_unregister();
	latency_tracker_destroy(tracker);
	*/
	printk("bye cruel world (from latirq)\n");
}
module_exit(latirq_exit);

MODULE_AUTHOR("Suchakrapani Sharma <suchakrapani.sharma@polymtl.ca>");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");
