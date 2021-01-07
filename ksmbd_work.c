// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *   Copyright (C) 2019 Samsung Electronics Co., Ltd.
 */

#include <linux/list.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/workqueue.h>

#include "server.h"
#include "connection.h"
#include "ksmbd_work.h"
#include "buffer_pool.h"
#include "mgmt/ksmbd_ida.h"

/* @FIXME */
#include "ksmbd_server.h"

static struct kmem_cache *work_cache;
static struct workqueue_struct *ksmbd_wq;

struct ksmbd_work *ksmbd_alloc_work_struct(void)
{
	struct ksmbd_work *work = kmem_cache_zalloc(work_cache, GFP_KERNEL);

	if (work) {
		work->compound_fid = KSMBD_NO_FID;
		work->compound_pfid = KSMBD_NO_FID;
		INIT_LIST_HEAD(&work->request_entry);
		INIT_LIST_HEAD(&work->async_request_entry);
		INIT_LIST_HEAD(&work->fp_entry);
		INIT_LIST_HEAD(&work->interim_entry);
	}
	return work;
}

void ksmbd_free_work_struct(struct ksmbd_work *work)
{
	WARN_ON(work->saved_cred != NULL);
	if (server_conf.flags & KSMBD_GLOBAL_FLAG_CACHE_TBUF &&
			work->set_trans_buf)
		ksmbd_release_buffer(RESPONSE_BUF(work));
	else
		ksmbd_free_response(RESPONSE_BUF(work));

	if (server_conf.flags & KSMBD_GLOBAL_FLAG_CACHE_RBUF &&
			work->set_read_buf)
		ksmbd_release_buffer(AUX_PAYLOAD(work));
	else
		ksmbd_free_response(AUX_PAYLOAD(work));

	ksmbd_free_response(TRANSFORM_BUF(work));
	ksmbd_free_request(REQUEST_BUF(work));
	if (work->async_id)
		ksmbd_release_id(work->conn->async_ida, work->async_id);
	kmem_cache_free(work_cache, work);
}

void ksmbd_work_pool_destroy(void)
{
	kmem_cache_destroy(work_cache);
}

int ksmbd_work_pool_init(void)
{
	work_cache = kmem_cache_create("ksmbd_work_cache",
					sizeof(struct ksmbd_work), 0,
					SLAB_HWCACHE_ALIGN, NULL);
	if (!work_cache)
		return -ENOMEM;
	return 0;
}

int ksmbd_workqueue_init(void)
{
	ksmbd_wq = alloc_workqueue("ksmbd-io", 0, 0);
	if (!ksmbd_wq)
		return -ENOMEM;
	return 0;
}

void ksmbd_workqueue_destroy(void)
{
	flush_workqueue(ksmbd_wq);
	destroy_workqueue(ksmbd_wq);
	ksmbd_wq = NULL;
}

bool ksmbd_queue_work(struct ksmbd_work *work)
{
	return queue_work(ksmbd_wq, &work->work);
}
