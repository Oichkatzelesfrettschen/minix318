/* minix/lib/klib/src/kmemory.c - KLIB Memory Manipulation Functions */
#include "klib.h"

kcpu_features_t g_kcpu_features = { kfalse, kfalse, kfalse, kfalse, kfalse, 64, 0, 0, 0, 0 };

static void* kmemcpy_scalar(void* dest, const void* src, ksize_t n) {
    KASSERT(dest != KNULL || n == 0); KASSERT(src != KNULL || n == 0);
    char* d = (char*)dest; const char* s = (const char*)src;
    for (ksize_t i = 0; i < n; i++) d[i] = s[i];
    return dest;
}

void* kmemcpy(void* dest, const void* src, ksize_t n) {
    KASSERT(dest != KNULL || n == 0); KASSERT(src != KNULL || n == 0);
    if (dest == src || n == 0) return dest;
    return kmemcpy_scalar(dest, src, n);
}

void* kmemcpy_fast(void* dest, const void* src, ksize_t n) { return kmemcpy(dest, src, n); }
void* kmemcpy_aligned(void* dest, const void* src, ksize_t n) {
    KASSERT(((kuintptr_t)dest % sizeof(void*)) == 0); KASSERT(((kuintptr_t)src % sizeof(void*)) == 0);
    return kmemcpy(dest, src, n);
}

static void* kmemset_scalar(void* s, kint_t c, ksize_t n) {
    KASSERT(s != KNULL || n == 0);
    unsigned char* p = (unsigned char*)s; unsigned char uc = (unsigned char)c;
    for (ksize_t i = 0; i < n; i++) p[i] = uc;
    return s;
}

void* kmemset(void* s, kint_t c, ksize_t n) {
    KASSERT(s != KNULL || n == 0);
    return kmemset_scalar(s, c, n);
}

void* kmemset_aligned(void* s, kint_t c, ksize_t n) {
    KASSERT(((kuintptr_t)s % sizeof(void*)) == 0);
    return kmemset(s, c, n);
}


void* kmemmove(void* dest, const void* src, ksize_t n) {
    KASSERT(dest != KNULL || n == 0); KASSERT(src != KNULL || n == 0);
    char* d = (char*)dest;
    const char* s = (const char*)src;
    if (d == s || n == 0) return dest;
    if (d < s) { // Source is ahead of destination, or no overlap where s is before d
        for (ksize_t i = 0; i < n; i++) {
            d[i] = s[i];
        }
    } else { // Destination is ahead of source (d > s)
             // Must copy backwards to avoid overwriting source before it's read
        for (ksize_t i = n; i > 0; i--) {
            d[i-1] = s[i-1];
        }
    }
    return dest;
}
