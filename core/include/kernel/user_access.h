/* SPDX-License-Identifier: BSD-2-Clause */
/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * Copyright (c) 2020, Linaro Limited
 */
#ifndef __KERNEL_USER_ACCESS_H
#define __KERNEL_USER_ACCESS_H

#include <assert.h>
#include <kernel/user_access_arch.h>
#include <tee_api_types.h>
#include <types_ext.h>

#ifdef CFG_WITH_USER_TA
TEE_Result copy_from_user_private(void *kaddr, const void *uaddr, size_t len);
TEE_Result copy_from_user(void *kaddr, const void *uaddr, size_t len);
#else
static inline TEE_Result copy_from_user_private(void *kaddr __unused,
						const void *uaddr __unused,
						size_t len __unused)
{
	return TEE_ERROR_NOT_SUPPORTED;
}

static inline TEE_Result copy_from_user(void *kaddr __unused,
					const void *uaddr __unused,
					size_t len __unused)
{
	return TEE_ERROR_NOT_SUPPORTED;
}

#endif

TEE_Result copy_to_user_private(void *uaddr, const void *kaddr, size_t len);
TEE_Result copy_to_user(void *uaddr, const void *kaddr, size_t len);

TEE_Result clear_user(void *uaddr, size_t n);

size_t strnlen_user(const void *s, size_t n);

/*
 * memdup_user() - Duplicate a user-space buffer.
 * @src:    Pointer to the user buffer to be duplicated.
 * @len:    Length of the user buffer to be duplicated.
 * @p:      Holds duplicated kernel buffer on success, or NULL on failure.
 *          Note that the returned buffer is allocated by malloc() and
 *          the caller shall free() the buffer later.
 * Return TEE_SUCCESS on success.
 * Return TEE_ERROR_OUT_OF_MEMORY or TEE_ERROR_ACCESS_DENIEND on error.
 */
TEE_Result memdup_user(const void *src, size_t len, void **p);

/*
 * memdup_user_private() - Duplicate a private user-space buffer.
 * @src:    Pointer to the user buffer to be duplicated. The buffer should
 *          be private to current TA (i.e., !TEE_MEMORY_ACCESS_ANY_OWNER).
 * @len:    Length of the user buffer to be duplicated.
 * @p:      Holds duplicated kernel buffer on success, or NULL on failure.
 *          Note that the returned buffer is allocated by malloc() and
 *          the caller shall free() the buffer later.
 * Return TEE_SUCCESS on success.
 * Return TEE_ERROR_OUT_OF_MEMORY or TEE_ERROR_ACCESS_DENIEND on error.
 */
TEE_Result memdup_user_private(const void *src, size_t len, void **p);

TEE_Result copy_kaddr_to_uref(uint32_t *uref, void *kaddr);

uint32_t kaddr_to_uref(void *kaddr);
vaddr_t uref_to_vaddr(uint32_t uref);
static inline void *uref_to_kaddr(uint32_t uref)
{
	return (void *)uref_to_vaddr(uref);
}

#define GET_USER_SCALAR(_x, _p) ({					\
	TEE_Result __res = TEE_SUCCESS;					\
	__typeof((_x)) __tmp = 0;					\
									\
	static_assert(sizeof(_x) == sizeof(*(_p)));			\
									\
	__res = copy_from_user(&__tmp, (const void *)_p, sizeof(_x));	\
	(_x) = (__tmp);							\
	__res;								\
})

#define PUT_USER_SCALAR(_x, _p) ({					\
	TEE_Result __res = TEE_SUCCESS;					\
	__typeof((_x)) __x = (_x);					\
									\
	static_assert(sizeof(_x) == sizeof(*(_p)));			\
									\
	__res = copy_to_user((void *)_p, &(__x), sizeof(*(_p)));	\
	__res;								\
})

#endif /*__KERNEL_USER_ACCESS_H*/
