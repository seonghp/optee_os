// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2023, Amazon.com Inc. or its affiliates. All rights Reserved.
 */

#ifndef KERNEL_USER_ACCESS_ARCH_H
#define KERNEL_USER_ACCESS_ARCH_H

#include <arm.h>

#ifdef CFG_PAN
static inline void enter_user_access(void)
{
	asm volatile(WRITE_PAN(0));
}

static inline void exit_user_access(void)
{
	asm volatile(WRITE_PAN(1));
}
#else
static inline void enter_user_access(void) {}
static inline void exit_user_access(void) {}
#endif /* CFG_PAN */

#ifdef ARM64
#define GET_USER(_x, _p) ({						\
	TEE_Result __res = TEE_SUCCESS;					\
	__typeof(_x) __x = 0;						\
	static_assert(sizeof(_x) == sizeof(*(_p)));			\
	__res = copy_from_user(&__x, (const void *)_p, sizeof(__x));	\
	(_x) = (__x);							\
	__res;								\
})

#define PUT_USER(_x, _p) ({						\
	TEE_Result __res = TEE_SUCCESS;					\
	__typeof(*(_p)) __x = (_x);					\
	static_assert(sizeof(_x) == sizeof(*(_p)));			\
	__res = copy_to_user((void *)_p, &(__x), sizeof(*(_p)));	\
	__res;								\
})
#else
#define GET_USER(_x, _p) ({ (_x) = *(_p); TEE_SUCCESS; })
#define PUT_USER(_x, _p) ({ *(_p) = (_x); TEE_SUCCESS; })
#endif

#endif /* KERNEL_USER_ACCESS_ARCH_H */
