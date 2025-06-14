/* minix/lib/klib/src/kformat.c - KLIB Safe Formatted String Printing */
#include "klib.h"
typedef __builtin_va_list k_va_list;
#define k_va_start(v,l)   __builtin_va_start(v,l)
#define k_va_end(v)       __builtin_va_end(v)
#define k_va_arg(v,l)     __builtin_va_arg(v,l)
static char* ul_to_str(char *b, unsigned long n, kint_t base, ksize_t* l, kbool_t uc){
    char t[sizeof(long)*8+1]; kint_t i=sizeof(t)-1; t[i]='\0';
    if(n==0)t[--i]='0';else while(n>0){kint_t d=n%base; if(d>=10)t[--i]=(uc?'A':'a')+(d-10);else t[--i]='0'+d; n/=base;}
    *l=(sizeof(t)-1)-i; kmemcpy(b,&t[i],*l); return b+*l;
}
kint_t ksnprintf(char*b,ksize_t sz,const char*fmt,...){
    KASSERT(b!=KNULL);KASSERT(sz>0);KASSERT(fmt!=KNULL);
    k_va_list args;k_va_start(args,fmt);
    char*p=b;char*e=b+sz-1;ksize_t twb=0;ksize_t acl;
    while(*fmt){
        if(*fmt=='%'){fmt++;ksize_t nl=0;char nb[24];
            switch(*fmt){
            case 's':{const char*s_arg=k_va_arg(args,const char*);if(!s_arg)s_arg="(null)";ksize_t sl=kstrlen(s_arg);twb+=sl;
                if(p<e){acl=(p+sl>e)?(ksize_t)(e-p):sl;kmemcpy(p,s_arg,acl);p+=acl;}break;}
            case 'd':{kint_t ia=k_va_arg(args,kint_t);unsigned long uv;
                if(ia<0){twb++;if(p<e)*p++='-';uv=(unsigned long)-ia;}else uv=(unsigned long)ia;
                ul_to_str(nb,uv,10,&nl,kfalse);twb+=nl;
                if(p<e){acl=(p+nl>e)?(ksize_t)(e-p):nl;kmemcpy(p,nb,acl);p+=acl;}break;}
            case 'u':{unsigned long ua=k_va_arg(args,unsigned long);ul_to_str(nb,ua,10,&nl,kfalse);twb+=nl;
                if(p<e){acl=(p+nl>e)?(ksize_t)(e-p):nl;kmemcpy(p,nb,acl);p+=acl;}break;}
            case 'x':case 'X':{unsigned long xa=k_va_arg(args,unsigned long);ul_to_str(nb,xa,16,&nl,(*fmt=='X'));twb+=nl;
                if(p<e){acl=(p+nl>e)?(ksize_t)(e-p):nl;kmemcpy(p,nb,acl);p+=acl;}break;}
            case 'p':{void*pta=k_va_arg(args,void*);twb+=2;if(p<e)*p++='0';if(p<e)*p++='x';
                ul_to_str(nb,(kuintptr_t)pta,16,&nl,kfalse);twb+=nl;
                if(p<e){acl=(p+nl>e)?(ksize_t)(e-p):nl;kmemcpy(p,nb,acl);p+=acl;}break;}
            case 'c':{char ca=(char)k_va_arg(args,kint_t);twb++;if(p<e)*p++=ca;break;}
            case '%':twb++;if(p<e)*p++='%';break;
            default:twb++;if(p<e)*p++='%';if(*fmt){twb++;if(p<e)*p++=*fmt;}else fmt--;break;}}
        else{twb++;if(p<e)*p++=*fmt;}
        if(*fmt=='\0')break;fmt++;}
    if(sz>0)*p='\0';k_va_end(args);return(kint_t)twb;
}
