/*++

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tpipv6.h

Abstract:

    This module contains IPv6-specific extensions, and address family
    independent extensions to Winsock for the IPv6 Technology Preview.

--*/

#ifndef _TPIPV6_
#define _TPIPV6_

#ifdef _MSC_VER
#define TPIPV6_INLINE __inline
#else
#define TPIPV6_INLINE extern inline /* GNU style */
#endif

#ifdef __cplusplus
#define TPIPV6_EXTERN extern "C"
#else
#define TPIPV6_EXTERN extern
#endif

#ifdef _WINSOCK2API_ 
/* This section gets included if winsock2.h is included */

#ifndef IPPROTO_IPV6

#define IPPROTO_IPV6 41

typedef unsigned __int64 u_int64;

//
// Portable socket structure.
//

//
// Desired design of maximum size and alignment.
// These are implementation specific.
//
#define _SS_MAXSIZE 128                  // Maximum size.
#define _SS_ALIGNSIZE (sizeof(__int64))  // Desired alignment. 

//
// Definitions used for sockaddr_storage structure paddings design.
//
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof (short))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof (short) + _SS_PAD1SIZE \
                                                    + _SS_ALIGNSIZE))

struct sockaddr_storage {
    short ss_family;               // Address family.
    char __ss_pad1[_SS_PAD1SIZE];  // 6 byte pad, this is to make
                                   // implementation specific pad up to
                                   // alignment field that follows explicit
                                   // in the data structure.
    __int64 __ss_align;            // Field to force desired structure.
    char __ss_pad2[_SS_PAD2SIZE];  // 112 byte pad to achieve desired size;
                                   // _SS_MAXSIZE value minus size of
                                   // ss_family, __ss_pad1, and
                                   // __ss_align fields is 112.
};

typedef struct sockaddr_storage SOCKADDR_STORAGE;
typedef struct sockaddr_storage *PSOCKADDR_STORAGE;
typedef struct sockaddr_storage FAR *LPSOCKADDR_STORAGE;

#endif /* !IPPROTO_IPV6 */
#endif /* _WINSOCK2API_ */

#ifdef _WS2TCPIP_H_ 
/* This section gets included if ws2tcpip.h is included */

#ifndef IPV6_JOIN_GROUP

#define in6_addr in_addr6

// Macro that works for both IPv4 and IPv6
#define SS_PORT(ssp) (((struct sockaddr_in*)(ssp))->sin_port)

#define IN6ADDR_ANY_INIT        { 0 }
#define IN6ADDR_LOOPBACK_INIT   { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 }

TPIPV6_EXTERN const struct in6_addr in6addr_any;
TPIPV6_EXTERN const struct in6_addr in6addr_loopback;

TPIPV6_INLINE int
IN6_ADDR_EQUAL(const struct in6_addr *a, const struct in6_addr *b)
{
    return (memcmp(a, b, sizeof(struct in6_addr)) == 0);
}

TPIPV6_INLINE int
IN6_IS_ADDR_UNSPECIFIED(const struct in6_addr *a)
{
    return IN6_ADDR_EQUAL(a, &in6addr_any);
}

TPIPV6_INLINE int
IN6_IS_ADDR_LOOPBACK(const struct in6_addr *a)
{
    return IN6_ADDR_EQUAL(a, &in6addr_loopback);
}

TPIPV6_INLINE int
IN6_IS_ADDR_MULTICAST(const struct in6_addr *a)
{
    return (a->s6_addr[0] == 0xff);
}

TPIPV6_INLINE int
IN6_IS_ADDR_LINKLOCAL(const struct in6_addr *a)
{
    return ((a->s6_addr[0] == 0xfe) &&
            ((a->s6_addr[1] & 0xc0) == 0x80));
}

TPIPV6_INLINE int
IN6_IS_ADDR_SITELOCAL(const struct in6_addr *a)
{
    return ((a->s6_addr[0] == 0xfe) &&
            ((a->s6_addr[1] & 0xc0) == 0xc0));
}

TPIPV6_INLINE int
IN6_IS_ADDR_V4MAPPED(const struct in6_addr *a)
{
    return ((a->s6_addr[0] == 0) &&
            (a->s6_addr[1] == 0) &&
            (a->s6_addr[2] == 0) &&
            (a->s6_addr[3] == 0) &&
            (a->s6_addr[4] == 0) &&
            (a->s6_addr[5] == 0) &&
            (a->s6_addr[6] == 0) &&
            (a->s6_addr[7] == 0) &&
            (a->s6_addr[8] == 0) &&
            (a->s6_addr[9] == 0) &&
            (a->s6_addr[10] == 0xff) &&
            (a->s6_addr[11] == 0xff));
}

TPIPV6_INLINE int
IN6_IS_ADDR_V4COMPAT(const struct in6_addr *a)
{
    return ((a->s6_addr[0] == 0) &&
            (a->s6_addr[1] == 0) &&
            (a->s6_addr[2] == 0) &&
            (a->s6_addr[3] == 0) &&
            (a->s6_addr[4] == 0) &&
            (a->s6_addr[5] == 0) &&
            (a->s6_addr[6] == 0) &&
            (a->s6_addr[7] == 0) &&
            (a->s6_addr[8] == 0) &&
            (a->s6_addr[9] == 0) &&
            (a->s6_addr[10] == 0) &&
            (a->s6_addr[11] == 0) &&
            !((a->s6_addr[12] == 0) &&
              (a->s6_addr[13] == 0) &&
              (a->s6_addr[14] == 0) &&
              ((a->s6_addr[15] == 0) ||
               (a->s6_addr[15] == 1))));
}

TPIPV6_INLINE int
IN6_IS_ADDR_MC_NODELOCAL(const struct in6_addr *a)
{
    return IN6_IS_ADDR_MULTICAST(a) && ((a->s6_addr[1] & 0xf) == 1);
}

TPIPV6_INLINE int
IN6_IS_ADDR_MC_LINKLOCAL(const struct in6_addr *a)
{
    return IN6_IS_ADDR_MULTICAST(a) && ((a->s6_addr[1] & 0xf) == 2);
}

TPIPV6_INLINE int
IN6_IS_ADDR_MC_SITELOCAL(const struct in6_addr *a)
{
    return IN6_IS_ADDR_MULTICAST(a) && ((a->s6_addr[1] & 0xf) == 5);
}

TPIPV6_INLINE int
IN6_IS_ADDR_MC_ORGLOCAL(const struct in6_addr *a)
{
    return IN6_IS_ADDR_MULTICAST(a) && ((a->s6_addr[1] & 0xf) == 8);
}

TPIPV6_INLINE int
IN6_IS_ADDR_MC_GLOBAL(const struct in6_addr *a)
{
    return IN6_IS_ADDR_MULTICAST(a) && ((a->s6_addr[1] & 0xf) == 0xe);
}

/* Argument structure for IPV6_JOIN_GROUP and IPV6_LEAVE_GROUP */

typedef struct ipv6_mreq {
    struct in6_addr ipv6mr_multiaddr;  // IPv6 multicast address.
    unsigned int    ipv6mr_interface;  // Interface index.
} IPV6_MREQ;

//
// Socket options at the IPPROTO_IPV6 level.
//
#define IPV6_UNICAST_HOPS       4  // Set/get IP unicast hop limit.
#define IPV6_MULTICAST_IF       9  // Set/get IP multicast interface.
#define IPV6_MULTICAST_HOPS     10 // Set/get IP multicast ttl.
#define IPV6_MULTICAST_LOOP     11 // Set/get IP multicast loopback.
#define IPV6_ADD_MEMBERSHIP     12 // Add an IP group membership.
#define IPV6_DROP_MEMBERSHIP    13 // Drop an IP group membership.
#define IPV6_JOIN_GROUP         IPV6_ADD_MEMBERSHIP
#define IPV6_LEAVE_GROUP        IPV6_DROP_MEMBERSHIP

//
// Socket options at the IPPROTO_UDP level.
//
#define UDP_CHECKSUM_COVERAGE   20  // Set/get UDP-Lite checksum coverage.

//
// Error codes from getaddrinfo().
//
#define EAI_AGAIN       WSATRY_AGAIN
#define EAI_BADFLAGS    WSAEINVAL
#define EAI_FAIL        WSANO_RECOVERY
#define EAI_FAMILY      WSAEAFNOSUPPORT
#define EAI_MEMORY      WSA_NOT_ENOUGH_MEMORY
#define EAI_NODATA      WSANO_DATA
#define EAI_NONAME      WSAHOST_NOT_FOUND
#define EAI_SERVICE     WSATYPE_NOT_FOUND
#define EAI_SOCKTYPE    WSAESOCKTNOSUPPORT

//
// Structure used in getaddrinfo() call.
//
typedef struct addrinfo {
    int ai_flags;              // AI_PASSIVE, AI_CANONNAME, AI_NUMERICHOST.
    int ai_family;             // PF_xxx.
    int ai_socktype;           // SOCK_xxx.
    int ai_protocol;           // 0 or IPPROTO_xxx for IPv4 and IPv6.
    size_t ai_addrlen;         // Length of ai_addr.
    char *ai_canonname;        // Canonical name for nodename.
    struct sockaddr *ai_addr;  // Binary address.
    struct addrinfo *ai_next;  // Next structure in linked list.
} ADDRINFO, FAR * LPADDRINFO;

//
// Flags used in "hints" argument to getaddrinfo().
//
#define AI_PASSIVE     0x1  // Socket address will be used in bind() call.
#define AI_CANONNAME   0x2  // Return canonical name in first ai_canonname.
#define AI_NUMERICHOST 0x4  // Nodename must be a numeric address string.

#ifdef __cplusplus
extern "C" {
#endif

WINSOCK_API_LINKAGE
int
WSAAPI
getaddrinfo(
    IN const char FAR * nodename,
    IN const char FAR * servname,
    IN const struct addrinfo FAR * hints,
    OUT struct addrinfo FAR * FAR * res
    );

#if INCL_WINSOCK_API_TYPEDEFS
typedef
int
(WSAAPI * LPFN_GETADDRINFO)(
    IN const char FAR * nodename,
    IN const char FAR * servname,
    IN const struct addrinfo FAR * hints,
    OUT struct addrinfo FAR * FAR * res
    );
#endif

WINSOCK_API_LINKAGE
void
WSAAPI
freeaddrinfo(
    IN struct addrinfo FAR * ai
    );

#if INCL_WINSOCK_API_TYPEDEFS
typedef
void
(WSAAPI * LPFN_FREEADDRINFO)(
    IN struct addrinfo FAR * ai
    );
#endif

#ifdef UNICODE
#define gai_strerror   gai_strerrorW
#else
#define gai_strerror   gai_strerrorA
#endif  /* UNICODE */

// WARNING: The gai_strerror inline functions below use static buffers, 
// and hence are not thread-safe.  We'll use buffers long enough to hold 
// 1k characters.  Any system error messages longer than this will be 
// returned as empty strings.  However 1k should work for the error codes 
// used by getaddrinfo().
#define GAI_STRERROR_BUFFER_SIZE 1024

TPIPV6_INLINE 
char *
gai_strerrorA(
    IN int ecode)
{
    DWORD dwMsgLen;
    static char buff[GAI_STRERROR_BUFFER_SIZE + 1];

    dwMsgLen = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM
                             |FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              ecode,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPSTR)buff,
                              GAI_STRERROR_BUFFER_SIZE,
                              NULL);

    return buff;
}

TPIPV6_INLINE 
WCHAR *
gai_strerrorW(
    IN int ecode
    )
{
    DWORD dwMsgLen;
    static WCHAR buff[GAI_STRERROR_BUFFER_SIZE + 1];

    dwMsgLen = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM
                             |FORMAT_MESSAGE_IGNORE_INSERTS,
                              NULL,
                              ecode,
                              MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                              (LPWSTR)buff,
                              GAI_STRERROR_BUFFER_SIZE,
                              NULL);

    return buff;
}

typedef int socklen_t;

WINSOCK_API_LINKAGE
int
WSAAPI
getnameinfo(
    IN  const struct sockaddr FAR * sa,
    IN  socklen_t       salen,
    OUT char FAR *      host,
    IN  DWORD           hostlen,
    OUT char FAR *      serv,
    IN  DWORD           servlen,
    IN  int             flags
    );

#if INCL_WINSOCK_API_TYPEDEFS
typedef
int
(WSAAPI * LPFN_GETNAMEINFO)(
    IN  const struct sockaddr FAR * sa,
    IN  socklen_t       salen,
    OUT char FAR *      host,
    IN  DWORD           hostlen,
    OUT char FAR *      serv,
    IN  DWORD           servlen,
    IN  int             flags
    );
#endif

#define NI_MAXHOST  1025  // Max size of a fully-qualified domain name.
#define NI_MAXSERV    32  // Max size of a service name.

//
// Flags for getnameinfo().
//
#define NI_NOFQDN       0x01  // Only return nodename portion for local hosts. 
#define NI_NUMERICHOST  0x02  // Return numeric form of the host's address.
#define NI_NAMEREQD     0x04  // Error if the host's name not in DNS.
#define NI_NUMERICSERV  0x08  // Return numeric form of the service (port #).
#define NI_DGRAM        0x10  // Service is a datagram service.

#ifdef __cplusplus
}
#endif

#endif /* !IPV6_JOIN_GROUP */
#endif /* _WS2TCPIP_H_ */

//
// Unless the build environment is explicitly targeting
// platforms that include built-in getaddrinfo() support,
// include the backwards-compatibility version of the APIs.
//
#if !defined(_WIN32_WINNT) || (_WIN32_WINNT <= 0x0500)
#include <wspiapi.h>
#endif

#endif /* _TPIPV6_ */
