#ifndef UTIL_MEMORY_H
#define UTIL_MEMORY_H

void *util_memory_alloc(size_t size);
void *util_memory_calloc(size_t nmemb, size_t size);
void *util_memory_realloc(void *ptr, size_t size);
void util_memory_free(void *ptr);

void util_memory_freeAll(void);
void util_memory_enableDebug(void);

#endif // UTIL_MEMORY_H

