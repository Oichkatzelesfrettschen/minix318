/* minix/lib/klib/src/kstring.c - KLIB String Manipulation Functions */

#include "klib.h"

/*
 * kstrlen - Calculate length of a string
 */
ksize_t kstrlen(const char* s) {
    KASSERT(s != KNULL);
    const char* p = s;
    while (*p) {
        p++;
    }
    return (ksize_t)(p - s);
}

/*
 * kstrcmp - Compare two strings
 */
kint_t kstrcmp(const char* s1, const char* s2) {
    KASSERT(s1 != KNULL);
    KASSERT(s2 != KNULL);
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const ku8_t*)s1 - *(const ku8_t*)s2;
}

/*
 * kstrncmp - Compare strings with length limit
 */
kint_t kstrncmp(const char* s1, const char* s2, ksize_t n) {
    KASSERT(s1 != KNULL);
    KASSERT(s2 != KNULL);

    if (n == 0) return 0;

    while (n > 0 && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }

    if (n == 0) return 0;
    return *(const ku8_t*)s1 - *(const ku8_t*)s2;
}

/*
 * kstrcpy - Copy a string
 */
char* kstrcpy(char* dest, const char* src) {
    KASSERT(dest != KNULL);
    KASSERT(src != KNULL);
    char* d = dest;
    while ((*d++ = *src++))
        ;
    return dest;
}

/*
 * kstrncpy - Copy a string with length limit (traditional behavior)
 * Note: This can leave dest non-null-terminated if src is too long.
 * kstrlcpy is generally preferred for safety.
 */
char* kstrncpy(char* dest, const char* src, ksize_t n) {
    KASSERT(dest != KNULL);
    KASSERT(src != KNULL);
    char* d = dest;
    while (n > 0 && *src) {
        *d++ = *src++;
        n--;
    }
    while (n > 0) {
        *d++ = '\0';
        n--;
    }
    return dest;
}

/*
 * kstrlcpy - Length-limited string copy with guaranteed null termination
 */
ksize_t kstrlcpy(char* dst, const char* src, ksize_t size) {
    KASSERT(dst != KNULL);
    KASSERT(src != KNULL);
    KASSERT(size > 0); /* Must have room for at least null terminator */

    ksize_t src_len = kstrlen(src);
    ksize_t copy_len;

    if (src_len < size) {
        copy_len = src_len;
    } else {
        copy_len = size - 1;
    }

    kmemcpy(dst, src, copy_len); /* Relies on kmemcpy being available */
    dst[copy_len] = '\0';

    return src_len; /* Return total length of source string, as per convention */
}

/*
 * kmemcmp - Compare memory blocks
 */
kint_t kmemcmp(const void* s1, const void* s2, ksize_t n) {
    KASSERT(s1 != KNULL);
    KASSERT(s2 != KNULL);

    const ku8_t* p1 = (const ku8_t*)s1;
    const ku8_t* p2 = (const ku8_t*)s2;

    while (n-- > 0) {
        if (*p1 != *p2) {
            return *p1 - *p2;
        }
        p1++;
        p2++;
    }
    return 0;
}

/*
 * kstrcat - Concatenate strings with buffer size checking
 * Safer than standard strcat because it includes buffer size parameter.
 */
char* kstrcat(char* dst, const char* src, ksize_t dst_size) {
    KASSERT(dst != KNULL);
    KASSERT(src != KNULL);
    KASSERT(dst_size > 0);

    ksize_t dst_len = kstrlen(dst);
    ksize_t src_len = kstrlen(src);

    KASSERT(dst_size > dst_len); // Ensure there's space for at least a null terminator

    if (dst_len + src_len >= dst_size) {
        // Not enough space for the whole string + null terminator.
        // Truncate src string.
        kstrlcpy(dst + dst_len, src, dst_size - dst_len);
        // kpanic("kstrcat: buffer overflow would occur"); // Alternative: panic
    } else {
        kmemcpy(dst + dst_len, src, src_len + 1);  /* +1 for null terminator */
    }
    return dst;
}

/*
 * kstrchr - Find first occurrence of character in string
 */
const char* kstrchr(const char* s, kint_t c) {
    KASSERT(s != KNULL);
    char ch = (char)c;
    while (*s != ch) {
        if (*s == '\0') {
            return KNULL;
        }
        s++;
    }
    return s;
}

/*
 * kstrrchr - Find last occurrence of character in string
 */
const char* kstrrchr(const char* s, kint_t c) {
    KASSERT(s != KNULL);
    const char* last = KNULL;
    char ch = (char)c;
    while (*s) {
        if (*s == ch) {
            last = s;
        }
        s++;
    }
    if (ch == '\0') { /* Special case: match null terminator */
        return s;
    }
    return last;
}
