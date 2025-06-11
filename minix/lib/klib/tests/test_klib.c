#include "klib.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#undef kprintf
#define kprintf(...) test_print_func(__VA_ARGS__)
#undef KASSERT
#define KASSERT(expr) \
    do { if (!(expr)) { test_print_func("TEST ASSERT FAILED: %s at %s:%d\n", #expr, __FILE__, (kint_t)__LINE__); test_fail_count++; } else { test_pass_count++; } } while(kfalse)
static kint_t test_pass_count = 0; static kint_t test_fail_count = 0;
void test_print_func(const char* fmt, ...) { va_list args; va_start(args, fmt); vprintf(fmt, args); va_end(args); }
void test_kmemcpy(void){char s[]="Hello, KLIB!";char d[20];char dov[20];kprintf("Testing kmemcpy...\n");kmemset(d,0,sizeof(d));kmemcpy(d,s,kstrlen(s)+1);KASSERT(kstrcmp(d,s)==0);
kprintf("Testing kmemmove (forward, overlapping, d > s)...\n");kstrcpy(dov,"abcdefghijkl");kmemmove(dov+2,dov,10);KASSERT(kmemcmp(dov,"ababcdefghij",12)==0);
kprintf("Testing kmemmove (backward/forward, overlapping, d < s)...\n");kstrcpy(dov,"abcdefghijkl");kmemmove(dov,dov+2,10);KASSERT(kmemcmp(dov,"cdefghijlk",10)==0);KASSERT(dov[10]=='k');KASSERT(dov[11]=='l');kprintf("kmemcpy/kmemmove tests done.\n");}
void test_kmemset(void){char b[32];kprintf("Testing kmemset...\n");kmemset(b,'A',sizeof(b));for(ksize_t i=0;i<sizeof(b);i++){KASSERT(b[i]=='A');}kmemset(b,0,5);KASSERT(b[0]==0&&b[4]==0&&b[5]=='A');kprintf("kmemset tests done.\n");}
void test_kstring(void){char sb[50];const char*ts="Test String123";kprintf("Testing kstrlen...\n");KASSERT(kstrlen(ts)==14);KASSERT(kstrlen("")==0);
kprintf("Testing kstrcmp...\n");KASSERT(kstrcmp(ts,"Test String123")==0);KASSERT(kstrcmp(ts,"Test String124")<0);KASSERT(kstrcmp(ts,"Test String12")>0);
kprintf("Testing kstrncmp...\n");KASSERT(kstrncmp(ts,"Test StringXYZ",11)==0);KASSERT(kstrncmp(ts,"Test StringXYZ",12)!=0);
kprintf("Testing kstrcpy...\n");kstrcpy(sb,ts);KASSERT(kstrcmp(sb,ts)==0);
kprintf("Testing kstrncpy...\n");kmemset(sb,'X',sizeof(sb));kstrncpy(sb,"short",10);KASSERT(kstrcmp(sb,"short")==0&&sb[5]=='\0'&&sb[9]=='\0');
kprintf("Testing kstrlcpy...\n");kmemset(sb,'X',sizeof(sb));ksize_t l=kstrlcpy(sb,"short",10);KASSERT(kstrcmp(sb,"short")==0&&l==5&&sb[5]=='\0');
kmemset(sb,'X',sizeof(sb));l=kstrlcpy(sb,"thisiswaytoolongforthisbuffer",10);KASSERT(kmemcmp(sb,"thisisway",9)==0&&sb[9]=='\0'&&l==29);
kprintf("Testing kstrcat...\n");kstrcpy(sb,"First");kstrcat(sb,"Second",sizeof(sb));KASSERT(kstrcmp(sb,"FirstSecond")==0);
kstrcpy(sb,"Test");kstrcat(sb,"ConcatenationTestMaxOutTheBuffer01234567890123456789",50);KASSERT(kstrlen(sb)==49&&sb[49]=='\0');
kprintf("Testing kstrchr...\n");KASSERT(kstrchr("find me here",'m')==&("find me here"[5]));KASSERT(kstrchr("find me here",'z')==KNULL);KASSERT(kstrchr("find me here",'\0')==&("find me here"[12]));
kprintf("Testing kstrrchr...\n");KASSERT(kstrrchr("abracadabra",'a')==&("abracadabra"[10]));KASSERT(kstrrchr("abracadabra",'z')==KNULL);KASSERT(kstrrchr("abracadabra",'\0')==&("abracadabra"[11]));kprintf("kstring tests done.\n");}
void test_kconv(void){kprintf("Testing kconv functions (katoi, kstrtoul)...\n");char*ep;
KASSERT(katoi("123")==123);KASSERT(katoi("-456")==-456);KASSERT(katoi("   +789")==789);KASSERT(katoi("0")==0);KASSERT(katoi("  -0  ")==0);
KASSERT(kstrtoul("  FF",&ep,16)==0xFF&&*ep=='\0');KASSERT(kstrtoul("0xABCD",&ep,0)==0xABCD&&*ep=='\0');
KASSERT(kstrtoul("0123",&ep,0)==0123&&*ep=='\0');KASSERT(kstrtoul("123xyz",&ep,0)==123&&strcmp(ep,"xyz")==0);
KASSERT(kstrtoul("0x",&ep,0)==0&&strcmp(ep,"0x")==0);KASSERT(kstrtoul("0xG",&ep,0)==0&&strcmp(ep,"0xG")==0);
KASSERT(kstrtoul("0",&ep,0)==0&&*ep=='\0');ku64_t nuo=(ku64_t)-1;KASSERT(kstrtoul("-1",&ep,0)==nuo&&*ep=='\0');
kprintf("kconv tests done.\n");}
void test_kformat(void){char b[128];kprintf("Testing ksnprintf...\n");kint_t w=ksnprintf(b,sizeof(b),"Hello %s","World");KASSERT(kstrcmp(b,"Hello World")==0&&w==11);
w=ksnprintf(b,sizeof(b),"Num %d, hex %x, HEX %X",-123,0xabc,0xDEF);KASSERT(kstrcmp(b,"Num -123, hex abc, HEX DEF")==0&&w==29);
void*tp=(void*)0xdeadbeef;char es[30];sprintf(es,"%p",tp);w=ksnprintf(b,sizeof(b),"%p",tp);KASSERT(kstrcmp(b,es)==0&&w==(kint_t)strlen(es));
w=ksnprintf(b,5,"longstring");KASSERT(kstrcmp(b,"long")==0&&w==10);kprintf("ksnprintf tests done.\n");}
int main(void){test_pass_count=0;test_fail_count=0;kprintf("--- Starting KLIB Test Suite ---\n");
test_kmemcpy();test_kmemset();test_kstring();test_kconv();test_kformat();
kprintf("--- KLIB Test Suite Complete ---\n");kprintf("PASSED: %d, FAILED: %d\n",test_pass_count,test_fail_count);
return(test_fail_count==0)?0:1;}
