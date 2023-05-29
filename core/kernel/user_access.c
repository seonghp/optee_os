// SPDX-License-Identifier: BSD-2-Clause
/*
 * Copyright (c) 2014, STMicroelectronics International N.V.
 * Copyright (c) 2015-2020, 2022 Linaro Limited
 */

#include <initcall.h>
#include <kernel/linker.h>
#include <kernel/user_access.h>
#include <kernel/user_mode_ctx.h>
#include <memtag.h>
#include <mm/vm.h>
#include <string.h>
#include <tee_api_types.h>
#include <types_ext.h>

static TEE_Result check_access(uint32_t flags, const void *uaddr, size_t len)
{
	struct ts_session *s = ts_get_current_session();
	struct user_mode_ctx *uctx = NULL;
	
	if (!is_user_mode_ctx(s->ctx)) {
		/*
		 * check_access may be called within a PTA session, which
		 * doesn't have a user_mode_ctx. Here, try to retrieve the
		 * user_mode_ctx associated with the calling session.
		 *
		 * NOTE: Refer to the definition of ts_get_calling_session()
		 * function.
		 */
		s = TAILQ_NEXT(s, link_tsd);
		if (!s)
			return TEE_ERROR_ACCESS_DENIED;

		uctx = to_user_mode_ctx(s->ctx);
		if (!uctx)
			return TEE_ERROR_ACCESS_DENIED;
	} else {
		uctx = to_user_mode_ctx(s->ctx);
	}

	return vm_check_access_rights(uctx, flags, (vaddr_t)uaddr, len);
}

TEE_Result copy_from_user(void *kaddr, const void *uaddr, size_t len)
{
	uint32_t flags = TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER;
	TEE_Result res = TEE_SUCCESS;

	uaddr = memtag_strip_tag_const(uaddr);
	res = check_access(flags, uaddr, len);
	if (!res) {
		enter_user_access();
		memcpy(kaddr, uaddr, len);
		exit_user_access();
	}

	return res;
}

TEE_Result copy_to_user(void *uaddr, const void *kaddr, size_t len)
{
	uint32_t flags = TEE_MEMORY_ACCESS_WRITE | TEE_MEMORY_ACCESS_ANY_OWNER;
	TEE_Result res = TEE_SUCCESS;

	uaddr = memtag_strip_tag(uaddr);
	res = check_access(flags, uaddr, len);
	if (!res) {
		enter_user_access();
		memcpy(uaddr, kaddr, len);
		exit_user_access();
	}

	return res;
}

TEE_Result copy_from_user_private(void *kaddr, const void *uaddr, size_t len)
{
	uint32_t flags = TEE_MEMORY_ACCESS_READ;
	TEE_Result res = TEE_SUCCESS;

	uaddr = memtag_strip_tag_const(uaddr);
	res = check_access(flags, uaddr, len);
	if (!res) {
		enter_user_access();
		memcpy(kaddr, uaddr, len);
		exit_user_access();
	}

	return res;
}

TEE_Result copy_to_user_private(void *uaddr, const void *kaddr, size_t len)
{
	uint32_t flags = TEE_MEMORY_ACCESS_WRITE;
	TEE_Result res = TEE_SUCCESS;

	uaddr = memtag_strip_tag(uaddr);
	res = check_access(flags, uaddr, len);
	if (!res) {
		enter_user_access();
		memcpy(uaddr, kaddr, len);
		exit_user_access();
	}

	return res;
}

TEE_Result clear_user(void *uaddr, size_t n)
{
	uint32_t flags = TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER;
	TEE_Result res = TEE_SUCCESS;

	uaddr = memtag_strip_tag(uaddr);
	res = check_access(flags, uaddr, n);
	if (res)
		return res;

	enter_user_access();
	memset(uaddr, 0, n);
	exit_user_access();

	return TEE_SUCCESS;
}

size_t strnlen_user(const void *uaddr, size_t len)
{
	uint32_t flags = TEE_MEMORY_ACCESS_READ | TEE_MEMORY_ACCESS_ANY_OWNER;
	TEE_Result res = TEE_SUCCESS;
	size_t n = 0;

	uaddr = memtag_strip_tag_const(uaddr);
	res = check_access(flags, uaddr, len);
	if (!res) {
		enter_user_access();
		n = strnlen(uaddr, len);
		exit_user_access();
	}

	return n;
}

TEE_Result memdup_user(const void *src, size_t len, void **p)
{
	TEE_Result res = TEE_SUCCESS;

	*p = malloc(len);
	if (!*p)
		return TEE_ERROR_OUT_OF_MEMORY;

	res = copy_from_user(*p, src, len);
	if (res) {
		free(*p);
		*p = NULL;
		return res;
	}

	return res;
}

TEE_Result copy_kaddr_to_uref(uint32_t *uref, void *kaddr)
{
	uint32_t ref = kaddr_to_uref(kaddr);

	return copy_to_user_private(uref, &ref, sizeof(ref));
}

uint32_t kaddr_to_uref(void *kaddr)
{
	if (MEMTAG_IS_ENABLED) {
		unsigned int uref_tag_shift = 32 - MEMTAG_TAG_WIDTH;
		vaddr_t uref = memtag_strip_tag_vaddr(kaddr);

		uref -= VCORE_START_VA;
		assert(uref < (UINT32_MAX >> MEMTAG_TAG_WIDTH));
		uref |= memtag_get_tag(kaddr) << uref_tag_shift;
		return uref;
	}

	assert(((vaddr_t)kaddr - VCORE_START_VA) < UINT32_MAX);
	return (vaddr_t)kaddr - VCORE_START_VA;
}

vaddr_t uref_to_vaddr(uint32_t uref)
{
	if (MEMTAG_IS_ENABLED) {
		vaddr_t u = uref & (UINT32_MAX >> MEMTAG_TAG_WIDTH);
		unsigned int uref_tag_shift = 32 - MEMTAG_TAG_WIDTH;
		uint8_t tag = uref >> uref_tag_shift;

		return memtag_insert_tag_vaddr(VCORE_START_VA + u, tag);
	}

	return VCORE_START_VA + uref;
}
