![simple-pt](http://halobates.de/spt-logo.png)

# Introduction

simple-pt is a simple experimental reference implementation of Intel Processor Trace (PT) on
Linux. PT can trace all branches executed by the CPU at the hardware level
with moderate overhead. A PT decoder then uses sideband trace data to decode the branch
traces. 

PT is supported on Intel 5th generation Core (Broadwell) and 6th generation Core (Skylake) CPUs.

# Example

	% sptcmd  -c tcall taskset -c 0 ./tcall
	cpu   0 offset 1027688,  1003 KB, writing to ptout.0
	...
	Wrote sideband to ptout.sideband
	% sptdecode --sideband ptout.sideband --pt ptout.0 | less
	TIME      DELTA	 INSNs   OPERATION
	frequency 32
	0        [+0]     [+   1] _dl_aux_init+436
	                  [+   6] __libc_start_main+455 -> _dl_discover_osversion
	...
	                  [+  13] __libc_start_main+446 -> main
	                  [+   9]     main+22 -> f1
	                  [+   4]	      f1+9 -> f2
	                  [+   2]	      f1+19 -> f2
	                  [+   5]     main+22 -> f1
	                  [+   4]	      f1+9 -> f2
	                  [+   2]	      f1+19 -> f2
	                  [+   5]     main+22 -> f1
	...

# Overview

simple-pt consists of a
* kernel driver
* sptcmd to collect data from the kernel driver
* sptdecode to decode PT information
* fastdecode to dump raw PT traces

It uses the [libipt](https://github.com/01org/processor-trace) PT decoding library

Note that Linux 4.1 and later has an [integrated PT implementation](http://lwn.net/Articles/648154/) as part 
of Linux perf. gdb 7.10 also supports full debugging on top of PT. [Intel VTune](https://software.intel.com/en-us/intel-vtune-amplifier-xe)
also supports PT.

If you want a full production system please use one of these. simple-pt is an experimental implementation.

Simple PT does *NOT* support:

* It does not support long term tracing of more data than fits in the buffer (no interrupt) (use perf)
* It does not support any sampling (use perf or VTune)
* It requires root rights to collect data (use perf)
* It does not support interactive debugging (use gdb or hardware debuggers)

Simple PT has the following functionality
* set up hardware to processor trace
* supports a ring buffer of branch data, stopped on events
* supports flushing buffer on panic
* does not require patching the kernel (although it cheats a bit using kprobes)
* set up PT filters, such as kernel filter
* start and stop traces at specific kernel addresses
* support tracing multiple processes
* print all function calls in "ftrace" style
* simple driver that could be ported to older kernel releases or other operating systems
* simple code base that is easily changed.
* modular "unix style" design with simple tools that do only one thing

# Installation

Build and install libipt. This currently requires a patched version of libipt.

	git clone -b simple-pt https://github.com/andikleen/processor-trace
	cd processor-trace
	cmake .
	make
	sudo make install
	sudo ldconfig

Install libelf-elf-devel or elfutils-devel or similar depending on your distribution.

Optional install udis86 if you want to see disassembled instructions:

	git clone https://github.com/vmt/udis86.git
	cd udis86
	./configure
	make

Clone simple-pt

	git clone https://github.com/andikleen/simple-pt
	cd simple-pt

Build the kernel module. May require installing kernel includes from your distribution.

	make 

Install the kernel module

	sudo make modules_install

Build the user tools

	make user

If you installed udis86 use

	make user UDIS86=1

Check if your system supports PT

	./ptfeature

Run a trace

	sudo ./sptcmd -c ls ls
	sudo ./sptdecode --sideband ptout.sideband --pt ptout.0 | less

sptcmd loads and configures the kernel driver. It runs a program with trace. It always 
does a global trace. It writes the pt trace data to trace files for each CPU
(ptout.N where N is the CPU number). It also writes side band information needed
to decode the trace into the ptout.sideband file. 

-c sets a command filter, tracing only commands with that name. Otherwise
everything global is traced.

sptdecode then decodes the trace for a CPU using the side band information.
When it should decode kernel code it needs to run as root to be able to
read /proc/kcore. If it's not run as root kernel code will not be shown.

Run test suite

	sudo ./tester

# Design overview

The kernel driver manages the PT hardware and allocates the trace buffers.
It also sets up some custom trace points for the sideband data.

The simple-pt kernel driver is configured using module parameters. Many can be changed
at runtime through /sys/module/simple_pt/parameters. A few need a driver reload

Use
	modinfo simple-pt.ko

to show all allowed parameters. For most parameters sptcmd has options to
set them up. That is the recommended interface.

sptcmd configures the driver, starts the trace and runs the trace command.
The driver sets up a ring buffer and runs the the processor trace
for each CPU until stopped.  Then it calls sptdump to write the buffer
for each CPU to a ptout.N file (N is the number of the CPU)

For the side band information ftrace with some custom trace points defined
by the driver is used. sptsideband converts the ftrace output into
the .sideband files used by the decoder.

sptdecode then reads the PT data, the sideband data, the executables, the kernel code
through /proc/kcore, and uses the libipt decoder to reconstruct the
trace.

# Manpages

* [sptcmd](http://halobates.de/spt-man/sptcmd.html)
* [sptdecode](http://halobates.de/spt-man/sptdecode.html)
* [ptfeature](http://halobates.de/spt-man/ptfeature.html)
* [sptarchive](http://halobates.de/spt-man/sptarchive.html)
* [fastdecode](http://halobates.de/spt-man/fastdecode.html)
* [sptdump](http://halobates.de/spt-man/sptdump.html)

# Changing the PT buffer sizes

To change the PT buffer size the driver needs to be loaded manually. The PT
buffer size can be changed with the pt_buffer_order parameter.

	rmmod simple_pt # if it was loaded
	modprobe simple_pt pt_buffer_order=10

The size is specified in 2^n 4K pages. The default is 9 (2MB). The maximum limit
is the kernel's MAX_ORDER limit, typically 8MB. The allocation may also fail
if the kernel memory is too fragmented. In this case quitting a large process
may help.

When ptfeature shows the "multiple toPA entries" feature it is possible to
allocate multiple PT buffers with the pt_num_buffers parameter. All the buffers
are logically concatenated. The default is one buffer. The maximum is 511
buffers.

# Notes

* To limit the program to one CPU use sptcmd taskset -c CPU ..
* To demangle C++ symbols pipe output through c++filt
* To start/stop around specific user code bracket it with dummy syscalls that you
  can then put a kernel trigger on. The test suite uses personality(12341234) and prctl(12341234).
  This will be improved in the future.
* perf or the BIOS may be already using the PT hardware. If you know it's safe you can take
  over the PT hardware with --force -d.
* When configuring the driver manually you need to manually reset any parameters you do not want anymore.
  sptcmd takes care of that automatically.
* Some Debian kernels are built without CONFIG_KALLSYMS_ALL. When you see an "Cannot find task_lock"
error message load the simple_pt module like this

	insmod simple_pt.ko tasklist_lock_ptr=0x$(grep tasklist_lock /boot/System.map-$(uname -r) | awk ' {print $1}')

# Limitations:

* When kernel tracing is disabled (-K) multiple processes cannot be distinguished by the decoder.

* Enabling/Disabling tracing causes the kernel to modify itself, which can cause the PT decoder
  to lose synchronization. sptcmd disables trace points. Workaround is to keep trace points
  running after the trace ends with -k, or disable kernel tracing. his can sometimes affect the
  test suite. If this happens try "tester -k"

* sptcmd does not continuously save side band data, so events at the beginning
  of a trace may not be saved. For complex workloads it may be needed to increase the trace buffers 
  in /sys/kernel/debug/tracing/buffer_size_kb

* The decoder does not (currently) support reusing the same address region in a process for
  different code (for example after dlclose/dlopen)

* Tracing JITed code is not supported.

* On Skylake the trace time occasionally jumps backwards after frequency changes.

* Decoder loses synchronization in some cases where it shouldn't.

# Contact
simple-pt@halobates.de

For bugs please file a github issue.

Andi Kleen
