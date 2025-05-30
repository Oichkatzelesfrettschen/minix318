/*
 * Copyright (c) 1987, 1997, 2006,
 * Vrije Universiteit, Amsterdam, The Netherlands.
 * All rights reserved. Redistribution and use of the MINIX 3 operating system
 * in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the Vrije Universiteit nor the names of the software
 *     authors or contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *   * Any deviations from these conditions require written permission from the
 *     copyright holder in advance
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS, AUTHORS, AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PRENTICE HALL OR ANY
 * AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributions by Mihai Chelaru.
 */

#ifndef _NETMPLS_MPLS_H_
#define _NETMPLS_MPLS_H_

#include <sys/param.h>
#include <sys/proc.h>
#include <sys/queue.h>
#include <sys/time.h>

#include <net/if.h>
#include <net/if_dl.h>

#define MPLS_LABEL_IPV4NULL 0 /* IPv4 Explicit NULL Label	*/
#define MPLS_LABEL_RTALERT 1  /* Router Alert Label		*/
#define MPLS_LABEL_IPV6NULL 2 /* IPv6 Explicit NULL Label	*/
#define MPLS_LABEL_IMPLNULL 3 /* Implicit NULL Label		*/
#define MPLS_LABEL_RESMAX 15  /* Maximum reserved Label	*/

union mpls_shim {
  uint32_t s_addr; /* the whole shim */
  struct {
#if BYTE_ORDER == LITTLE_ENDIAN
    uint32_t ttl : 8;
    uint32_t bos : 1;
    uint32_t exp : 3;
    uint32_t label : 20;
#else
    uint32_t label : 20;
    uint32_t exp : 3;
    uint32_t bos : 1;
    uint32_t ttl : 8;
#endif
  } shim;
};

struct sockaddr_mpls {
  uint8_t smpls_len;
  uint8_t smpls_family;
  uint8_t smpls_pad[2];
  union mpls_shim smpls_addr;
} __packed;

#endif /* !_NETMPLS_MPLS_H_ */
