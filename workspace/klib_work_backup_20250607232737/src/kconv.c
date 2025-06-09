/* minix/lib/klib/src/kconv.c - KLIB String to Number Conversion */
#include "klib.h"
#ifndef INT_MAX
#define INT_MAX ((kint_t)0x7FFFFFFF)
#endif
#ifndef INT_MIN
#define K_INT_MIN_MAGNITUDE ((ku32_t)INT_MAX + 1)
#define INT_MIN ((kint_t)0x80000000)
#endif
#ifndef ULONG_MAX
#define ULONG_MAX ((ku64_t)0xFFFFFFFFFFFFFFFFULL)
#endif
static kbool_t is_space(char c) { return (c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r'); }
static kbool_t is_digit(char c) { return (c >= '0' && c <= '9'); }
kint_t katoi(const char* str) {
    KASSERT(str != KNULL); ku32_t r = 0; kint_t s = 1; const char* p = str;
    while (is_space(*p)) p++;
    if (*p == '-') { s = -1; p++; } else if (*p == '+') p++;
    while (is_digit(*p)) {
        kint_t d = *p - '0';
        if (s == 1) { if (r > ((ku32_t)INT_MAX - d) / 10) { kpanic("katoi: overflow"); return INT_MAX; }}
        else { if (r > K_INT_MIN_MAGNITUDE / 10 || (r == K_INT_MIN_MAGNITUDE / 10 && (ku32_t)d > K_INT_MIN_MAGNITUDE % 10)) { kpanic("katoi: underflow"); return INT_MIN; }}
        r = r * 10 + d; p++;
    }
    return (s == 1) ? (kint_t)r : -(kint_t)r;
}
ku64_t kstrtoul(const char* str, char** e, kint_t b) {
    KASSERT(str!=KNULL); KASSERT(b==0||(b>=2&&b<=36)); ku64_t r=0; kint_t s=1; const char*p=str; const char*sp;
    while(is_space(*p))p++; sp=p;
    if(*p=='-'){s=-1;p++;}else if(*p=='+')p++;
    if(b==0){if(*p=='0'){if(p[1]=='x'||p[1]=='X'){if((p[2]>='0'&&p[2]<='9')||(p[2]>='a'&&p[2]<='f')||(p[2]>='A'&&p[2]<='F')){b=16;p+=2;}else b=10;}else if(p[1]>='0'&&p[1]<='7')b=8;else b=10;}else b=10;}
    else if(b==16){if(*p=='0'&&(p[1]=='x'||p[1]=='X')){if((p[2]>='0'&&p[2]<='9')||(p[2]>='a'&&p[2]<='f')||(p[2]>='A'&&p[2]<='F'))p+=2;}}
    const char*dsp=p;kint_t d;
    while(*p){if(is_digit(*p))d=*p-'0';else if(*p>='a'&&*p<='z')d=*p-'a'+10;else if(*p>='A'&&*p<='Z')d=*p-'A'+10;else break;if(d>=b)break;if(r>(ULONG_MAX-(ku64_t)d)/(ku64_t)b){if(e)*e=(char*)p;kpanic("kstrtoul: overflow");return ULONG_MAX;}r=r*b+d;p++;}
    if(e){if(p==dsp)*e=(char*)sp;else *e=(char*)p;}
    if(s==-1&&r!=0)return(~r+1);return r;
}
