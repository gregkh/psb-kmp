/**************************************************************************
 *
 * Copyright (c) 2006-2007 Tungsten Graphics, Inc., Cedar Park, TX., USA
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
 * THE COPYRIGHT HOLDERS, AUTHORS AND/OR ITS SUPPLIERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE
 * USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **************************************************************************/
/*
 * Authors: Thomas Hellstr�m <thomas-at-tungstengraphics-dot-com>
 */

#include "drmP.h"

static void drm_ttm_ipi_handler(void *null)
{
	flush_agp_cache();
}

void drm_ttm_cache_flush(void)
{
  #if (LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,27))
  if (on_each_cpu(drm_ttm_ipi_handler, NULL, 1) != 0) 
  #else 
  if (on_each_cpu(drm_ttm_ipi_handler, NULL, 1, 1) != 0)
  #endif
		DRM_ERROR("Timed out waiting for drm cache flush.\n");
}
EXPORT_SYMBOL(drm_ttm_cache_flush);

/*
 * Use kmalloc if possible. Otherwise fall back to vmalloc.
 */

static void ttm_alloc_pages(struct drm_ttm *ttm)
{
	unsigned long size = ttm->num_pages * sizeof(*ttm->pages);
	ttm->pages = NULL;

	if (size <= PAGE_SIZE)
		ttm->pages = drm_calloc(1, size, DRM_MEM_TTM);

	if (!ttm->pages) {
		ttm->pages = vmalloc_user(size);
		if (ttm->pages)
			ttm->page_flags |= DRM_TTM_PAGE_VMALLOC;
	}
}

static void ttm_free_pages(struct drm_ttm *ttm)
{
	unsigned long size = ttm->num_pages * sizeof(*ttm->pages);

	if (ttm->page_flags & DRM_TTM_PAGE_VMALLOC) {
		vfree(ttm->pages);
		ttm->page_flags &= ~DRM_TTM_PAGE_VMALLOC;
	} else {
		drm_free(ttm->pages, size, DRM_MEM_TTM);
	}
	ttm->pages = NULL;
}

static struct page *drm_ttm_alloc_page(void)
{
	struct page *page;

	page = alloc_page(GFP_KERNEL | __GFP_ZERO | GFP_DMA32);
	if (!page)
		return NULL;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15))
	SetPageReserved(page);
#endif
	return page;
}

/*
 * Change caching policy for the linear kernel map
 * for range of pages in a ttm.
 */

static int drm_set_caching(struct drm_ttm *ttm, int noncached)
{
	int i;
	struct page **cur_page;
	int do_tlbflush = 0;

	if ((ttm->page_flags & DRM_TTM_PAGE_UNCACHED) == noncached)
		return 0;

	if (noncached)
		drm_ttm_cache_flush();

	for (i = 0; i < ttm->num_pages; ++i) {
		cur_page = ttm->pages + i;
		if (*cur_page) {
			if (!PageHighMem(*cur_page)) {
				if (noncached) {
					map_page_into_agp(*cur_page);
				} else {
					unmap_page_from_agp(*cur_page);
				}
				do_tlbflush = 1;
			}
		}
	}
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,25))
	if (do_tlbflush)
		flush_agp_mappings();
#endif

	DRM_FLAG_MASKED(ttm->page_flags, noncached, DRM_TTM_PAGE_UNCACHED);

	return 0;
}


static void drm_ttm_free_user_pages(struct drm_ttm *ttm)
{
	int write;
	int dirty;
	struct page *page;
	int i;

	BUG_ON(!(ttm->page_flags & DRM_TTM_PAGE_USER));
	write = ((ttm->page_flags & DRM_TTM_PAGE_USER_WRITE) != 0);
	dirty = ((ttm->page_flags & DRM_TTM_PAGE_USER_DIRTY) != 0);

	for (i = 0; i < ttm->num_pages; ++i) {
		page = ttm->pages[i];
		if (page == NULL)
			continue;

		if (page == ttm->dummy_read_page) {
			BUG_ON(write);
			continue;
		}

		if (write && dirty && !PageReserved(page))
			set_page_dirty_lock(page);

		ttm->pages[i] = NULL;
		put_page(page);
	}
}

static void drm_ttm_free_alloced_pages(struct drm_ttm *ttm)
{
	int i;
	struct drm_buffer_manager *bm = &ttm->dev->bm;
	struct page **cur_page;

	for (i = 0; i < ttm->num_pages; ++i) {
		cur_page = ttm->pages + i;
		if (*cur_page) {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,15))
			ClearPageReserved(*cur_page);
#endif
			if (page_count(*cur_page) != 1)
				DRM_ERROR("Erroneous page count. Leaking pages.\n");
			if (page_mapped(*cur_page))
				DRM_ERROR("Erroneous map count. Leaking page mappings.\n");
			__free_page(*cur_page);
			--bm->cur_pages;
		}
	}
}

/*
 * Free all resources associated with a ttm.
 */

int drm_destroy_ttm(struct drm_ttm *ttm)
{
	struct drm_ttm_backend *be;

	if (!ttm)
		return 0;

	be = ttm->be;
	if (be) {
		be->func->destroy(be);
		ttm->be = NULL;
	}

	if (ttm->pages) {
		if (ttm->page_flags & DRM_TTM_PAGE_UNCACHED)
			drm_set_caching(ttm, 0);

		if (ttm->page_flags & DRM_TTM_PAGE_USER)
			drm_ttm_free_user_pages(ttm);
		else
			drm_ttm_free_alloced_pages(ttm);

		ttm_free_pages(ttm);
	}

	return 0;
}

struct page *drm_ttm_get_page(struct drm_ttm *ttm, int index)
{
	struct page *p;
	struct drm_buffer_manager *bm = &ttm->dev->bm;

	p = ttm->pages[index];
	if (!p) {
		p = drm_ttm_alloc_page();
		if (!p)
			return NULL;
		ttm->pages[index] = p;
		++bm->cur_pages;
	}
	return p;
}
EXPORT_SYMBOL(drm_ttm_get_page);

int drm_ttm_set_user(struct drm_ttm *ttm,
		     struct task_struct *tsk,
		     int write,
		     unsigned long start,
		     unsigned long num_pages,
		     struct page *dummy_read_page)
{
	struct mm_struct *mm = tsk->mm;
	int ret;
	int i;

	BUG_ON(num_pages != ttm->num_pages);

	ttm->dummy_read_page = dummy_read_page;
	ttm->page_flags |= DRM_TTM_PAGE_USER |
		((write) ? DRM_TTM_PAGE_USER_WRITE : 0);


	down_read(&mm->mmap_sem);
	ret = get_user_pages(tsk, mm, start, num_pages,
			     write, 0, ttm->pages, NULL);
	up_read(&mm->mmap_sem);

	if (ret != num_pages && write) {
		drm_ttm_free_user_pages(ttm);
		return -ENOMEM;
	}

	for (i = 0; i < num_pages; ++i) {
		if (ttm->pages[i] == NULL)
			ttm->pages[i] = ttm->dummy_read_page;
	}

	return 0;
}

int drm_ttm_populate(struct drm_ttm *ttm)
{
	struct page *page;
	unsigned long i;
	struct drm_ttm_backend *be;

	if (ttm->state != ttm_unpopulated)
		return 0;

	be = ttm->be;
	for (i = 0; i < ttm->num_pages; ++i) {
		page = drm_ttm_get_page(ttm, i);
		if (!page)
			return -ENOMEM;
	}
	be->func->populate(be, ttm->num_pages, ttm->pages);
	ttm->state = ttm_unbound;
	return 0;
}

/*
 * Calculate the estimated pinned memory usage of a ttm.
 */

unsigned long drm_ttm_size(struct drm_device *dev,
			   unsigned long num_pages,
			   int user_bo)
{
	struct drm_bo_driver *bo_driver = dev->driver->bo_driver;
	unsigned long tmp;

	tmp = drm_size_align(sizeof(struct drm_ttm)) +
		drm_size_align(num_pages * sizeof(struct page *)) +
		((user_bo) ? 0 : drm_size_align(num_pages * PAGE_SIZE));

	if (bo_driver->backend_size)
		tmp += bo_driver->backend_size(dev, num_pages);
	else
		tmp += drm_size_align(num_pages * sizeof(struct page *)) +
			3*drm_size_align(sizeof(struct drm_ttm_backend));
	return tmp;
}


/*
 * Initialize a ttm.
 */

struct drm_ttm *drm_ttm_init(struct drm_device *dev, unsigned long size)
{
	struct drm_bo_driver *bo_driver = dev->driver->bo_driver;
	struct drm_ttm *ttm;

	if (!bo_driver)
		return NULL;

	ttm = drm_calloc(1, sizeof(*ttm), DRM_MEM_TTM);
	if (!ttm)
		return NULL;

	ttm->dev = dev;
	atomic_set(&ttm->vma_count, 0);

	ttm->destroy = 0;
	ttm->num_pages = (size + PAGE_SIZE - 1) >> PAGE_SHIFT;

	ttm->page_flags = 0;

	/*
	 * Account also for AGP module memory usage.
	 */

	ttm_alloc_pages(ttm);
	if (!ttm->pages) {
		drm_destroy_ttm(ttm);
		DRM_ERROR("Failed allocating page table\n");
		return NULL;
	}
	ttm->be = bo_driver->create_ttm_backend_entry(dev);
	if (!ttm->be) {
		drm_destroy_ttm(ttm);
		DRM_ERROR("Failed creating ttm backend entry\n");
		return NULL;
	}
	ttm->state = ttm_unpopulated;
	return ttm;
}

/*
 * Unbind a ttm region from the aperture.
 */

void drm_ttm_evict(struct drm_ttm *ttm)
{
	struct drm_ttm_backend *be = ttm->be;
	int ret;

	if (ttm->state == ttm_bound) {
		ret = be->func->unbind(be);
		BUG_ON(ret);
	}

	ttm->state = ttm_evicted;
}

void drm_ttm_fixup_caching(struct drm_ttm *ttm)
{

	if (ttm->state == ttm_evicted) {
		struct drm_ttm_backend *be = ttm->be;
		if (be->func->needs_ub_cache_adjust(be))
			drm_set_caching(ttm, 0);
		ttm->state = ttm_unbound;
	}
}

void drm_ttm_unbind(struct drm_ttm *ttm)
{
	if (ttm->state == ttm_bound)
		drm_ttm_evict(ttm);

	drm_ttm_fixup_caching(ttm);
}

int drm_bind_ttm(struct drm_ttm *ttm, struct drm_bo_mem_reg *bo_mem)
{
	struct drm_bo_driver *bo_driver = ttm->dev->driver->bo_driver;
	int ret = 0;
	struct drm_ttm_backend *be;

	if (!ttm)
		return -EINVAL;
	if (ttm->state == ttm_bound)
		return 0;

	be = ttm->be;

	ret = drm_ttm_populate(ttm);
	if (ret)
		return ret;

	if (ttm->state == ttm_unbound && !(bo_mem->flags & DRM_BO_FLAG_CACHED))
		drm_set_caching(ttm, DRM_TTM_PAGE_UNCACHED);
	else if ((bo_mem->flags & DRM_BO_FLAG_CACHED_MAPPED) &&
		   bo_driver->ttm_cache_flush)
		bo_driver->ttm_cache_flush(ttm);

	ret = be->func->bind(be, bo_mem);
	if (ret) {
		ttm->state = ttm_evicted;
		DRM_ERROR("Couldn't bind backend.\n");
		return ret;
	}

	ttm->state = ttm_bound;
	if (ttm->page_flags & DRM_TTM_PAGE_USER)
		ttm->page_flags |= DRM_TTM_PAGE_USER_DIRTY;
	return 0;
}
EXPORT_SYMBOL(drm_bind_ttm);
