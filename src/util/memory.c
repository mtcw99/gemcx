#include "memory.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct util_Memory
{
	void		**ptrs;
	uint64_t	*allocSize;

	uint64_t	length;
	uint64_t	alloc;

	uint64_t	total;
	bool		debug;
};

static const uint64_t UTIL_MEMORY_EXPAND = 64;
static struct util_Memory mems = {
	.ptrs = NULL,
	.allocSize = NULL,
	.length = 0,
	.alloc = 0,
	.total = 0,
	.debug = false
};

static void
util_memory__expand(void)
{
	if (mems.alloc == 0)
	{
		mems.alloc = UTIL_MEMORY_EXPAND;
		mems.length = 0;
		mems.ptrs = calloc(sizeof(void *), mems.alloc);
		mems.allocSize = calloc(sizeof(uint64_t), mems.alloc);
	}
	else if (mems.length == (mems.alloc - 1))
	{
		mems.alloc += UTIL_MEMORY_EXPAND;
		mems.ptrs = realloc(mems.ptrs,
				sizeof(void *) * mems.alloc);
		mems.allocSize = realloc(mems.allocSize,
				sizeof(uint64_t) * mems.alloc);
	}
}

static void
util_memory__set(const uint64_t index, void *ptr, const size_t size)
{
	mems.ptrs[index] = ptr;
	mems.allocSize[index] = size;
}

static void
util_memory__add(void *ptr, const size_t size)
{
	util_memory__expand();
	util_memory__set(mems.length++, ptr, size);
	mems.total += size;
}

static uint64_t
util_memory__getIndex(void *ptr, bool *found)
{
	for (uint64_t i = 0; i < mems.length; ++i)
	{
		if (mems.ptrs[i] == ptr)
		{
			*found = true;
			return i;
		}
	}

	*found = false;
	return 0;
}

void *
util_memory_alloc(size_t size)
{
	void *ptr = malloc(size);
	util_memory__add(ptr, size);
	return ptr;
}

void *
util_memory_calloc(size_t nmemb, size_t size)
{
	void *ptr = calloc(nmemb, size);
	util_memory__add(ptr, nmemb * size);
	return ptr;
}

void *
util_memory_realloc(void *ptr, size_t size)
{
	bool found = false;
	const uint64_t index = util_memory__getIndex(ptr, &found);
	void *nptr = realloc(ptr, size);
	(void) found;
	//printf("[%ld]: %p => %p\n", index, ptr, nptr);
	mems.total = mems.total - mems.allocSize[index] + size;
	util_memory__set(index, nptr, size);
	return nptr;
}

void
util_memory_free(void *ptr)
{
	bool found = false;
	const uint64_t index = util_memory__getIndex(ptr, &found);
	free(ptr);
	ptr = NULL;
	if (found)
	{
		mems.total -= mems.allocSize[index];
		mems.allocSize[index] = 0;
	}
}

void
util_memory_freeAll(void)
{
	if (mems.debug)
	{
		printf("Total pre-freeAll: %ld\n", mems.total);
	}

	if (mems.total)
	{
		for (uint64_t i = 0; i < mems.length; ++i)
		{
			//printf("[%ld]: %ld | %p | %ld\n", i, mems.total, mems.ptrs[i], mems.allocSize[i]);
			if ((mems.ptrs[i] != NULL) && (mems.allocSize[i] > 0))
			{
				free(mems.ptrs[i]);
				mems.ptrs[i] = NULL;
			}
			mems.total -= mems.allocSize[i];
		}
	}

	if (mems.debug)
	{
		printf("Total post-freeAll: %ld\n", mems.total);
	}

	free(mems.ptrs);
	free(mems.allocSize);
	mems.ptrs = NULL;
	mems.allocSize = NULL;
}

void
util_memory_enableDebug(void)
{
	mems.debug = true;
}


