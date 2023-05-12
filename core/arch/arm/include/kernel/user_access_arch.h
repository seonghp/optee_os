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

#endif /* KERNEL_USER_ACCESS_ARCH_H */
