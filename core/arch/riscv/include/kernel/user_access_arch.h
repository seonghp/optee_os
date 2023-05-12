// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2023, Amazon.com Inc. or its affiliates. All rights Reserved.
 */

#ifndef KERNEL_USER_ACCESS_ARCH_H
#define KERNEL_USER_ACCESS_ARCH_H

static inline void enter_user_access(void) {}
static inline void exit_user_access(void) {}

#define GET_USER(_x, _p) ({ (_x) = *(_p); TEE_SUCCESS; })
#define PUT_USER(_x, _p) ({ *(_p) = (_x); TEE_SUCCESS; })

#endif /* KERNEL_USER_ACCESS_ARCH_H */
