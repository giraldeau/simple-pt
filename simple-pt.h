#ifndef _SIMPLE_PT
#define _SIMPLE_PT

#include <linux/percpu.h>
#include <linux/types.h>

#define SIMPLE_PT_SET_CPU 9901 /* arg: cpu number */
#define SIMPLE_PT_GET_SIZE 9902 /* arg: pointer to int, output for buffer size */
#define SIMPLE_PT_GET_OFFSET 9903 /* arg: pointer to int, last write offset in buffer */
			     /* must be stopped */

#define MSR_IA32_RTIT_OUTPUT_BASE	0x00000560
#define MSR_IA32_RTIT_OUTPUT_MASK_PTRS	0x00000561
#define MSR_IA32_RTIT_CTL		0x00000570
#define TRACE_EN	BIT_ULL(0)
#define CYC_EN		BIT_ULL(1)
#define CTL_OS		BIT_ULL(2)
#define CTL_USER	BIT_ULL(3)
#define PT_ERROR	BIT_ULL(4)
#define CR3_FILTER	BIT_ULL(7)
#define TO_PA		BIT_ULL(8)
#define MTC_EN		BIT_ULL(9)
#define TSC_EN		BIT_ULL(10)
#define DIS_RETC	BIT_ULL(11)
#define BRANCH_EN	BIT_ULL(13)
#define MTC_MASK	(0xf << 14)
#define CYC_MASK	(0xf << 19)
#define PSB_MASK	(0xf << 24)
#define ADDR0_SHIFT	32
#define ADDR1_SHIFT	32
#define ADDR0_MASK	(0xfULL << ADDR0_SHIFT)
#define ADDR1_MASK	(0xfULL << ADDR1_SHIFT)
#define MSR_IA32_RTIT_STATUS		0x00000571
#define MSR_IA32_CR3_MATCH		0x00000572
#define TOPA_STOP	BIT_ULL(4)
#define TOPA_INT	BIT_ULL(2)
#define TOPA_END	BIT_ULL(0)
#define TOPA_SIZE_SHIFT 6
#define MSR_IA32_ADDR0_START		0x00000580
#define MSR_IA32_ADDR0_END		0x00000581
#define MSR_IA32_ADDR1_START		0x00000582
#define MSR_IA32_ADDR1_END		0x00000583

DECLARE_PER_CPU(bool, pt_running);

#endif //_SIMPLE_PT
