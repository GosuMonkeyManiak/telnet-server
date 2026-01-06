#ifndef LWIPOPTS_H
#define LWIPOPTS_H

/* ===============================================================
 * System
 * =============================================================== */
#define NO_SYS                         0
#define SYS_LIGHTWEIGHT_PROT           1

/* No errno */
#define LWIP_PROVIDE_ERRNO             0
#define LWIP_ERRNO_STDINCLUDE          0
#define LWIP_SOCKET_SET_ERRNO          0

/* ===============================================================
 * Memory (CRITICAL)
 * =============================================================== */
#define MEM_ALIGNMENT                  4
#define MEM_SIZE                       (4 * 1024)   /* lwIP heap */

/* Use lwIP heap, not malloc */
#define MEM_LIBC_MALLOC                0
#define MEMP_MEM_MALLOC                0

/* ===============================================================
 * Pbufs
 * =============================================================== */
#define PBUF_POOL_SIZE                 4
#define PBUF_POOL_BUFSIZE              512

/* ===============================================================
 * Pools (VERY SMALL)
 * =============================================================== */
#define MEMP_NUM_PBUF                  4
#define MEMP_NUM_RAW_PCB               2
#define MEMP_NUM_UDP_PCB               2
#define MEMP_NUM_TCP_PCB               2
#define MEMP_NUM_TCP_PCB_LISTEN        1
#define MEMP_NUM_TCP_SEG               4
#define MEMP_NUM_NETBUF                2
#define MEMP_NUM_NETCONN               2
#define MEMP_NUM_SYS_TIMEOUT           4

/* ===============================================================
 * Network
 * =============================================================== */
#define LWIP_NETIF_API                 1
#define LWIP_NETIF_STATUS_CALLBACK     0
#define LWIP_NETIF_LINK_CALLBACK       0

#define LWIP_ARP                       1
#define ARP_TABLE_SIZE                 4
#define ARP_QUEUEING                   0

/* ===============================================================
 * IP
 * =============================================================== */
#define LWIP_IPV4                      1
#define LWIP_IPV6                      0

#define IP_REASSEMBLY                  0
#define IP_FRAG                        0
#define IP_FORWARD                     0

/* ===============================================================
 * ICMP
 * =============================================================== */
#define LWIP_ICMP                      1

/* ===============================================================
 * UDP / TCP
 * =============================================================== */
#define LWIP_UDP                       1
#define LWIP_TCP                       1

#define TCP_MSS                        536
#define TCP_SND_BUF                    (2 * TCP_MSS)
#define TCP_WND                        (2 * TCP_MSS)
#define TCP_SND_QUEUELEN               4

#define TCP_LISTEN_BACKLOG             0
#define LWIP_TCP_KEEPALIVE             0

/* ===============================================================
 * APIs (IMPORTANT)
 * =============================================================== */
#define LWIP_SOCKET                    0   /* sockets are EXPENSIVE */
#define LWIP_NETCONN                   1
#define LWIP_RAW                       1

/* ===============================================================
 * DHCP / DNS (DISABLE)
 * =============================================================== */
#define LWIP_DHCP                      0
#define LWIP_DNS                       0
#define LWIP_AUTOIP                   0

/* ===============================================================
 * Statistics & Debug (DISABLE)
 * =============================================================== */
#define LWIP_STATS                     0
#define LWIP_DEBUG                     0
#define LWIP_NOASSERT                  1

/* ===============================================================
 * Checksums
 * =============================================================== */
#define CHECKSUM_GEN_IP                1
#define CHECKSUM_GEN_UDP               1
#define CHECKSUM_GEN_TCP               1
#define CHECKSUM_CHECK_IP              1
#define CHECKSUM_CHECK_UDP             1
#define CHECKSUM_CHECK_TCP             1

/* ===============================================================
 * FreeRTOS tuning
 * =============================================================== */
#define TCPIP_THREAD_STACKSIZE         512
#define TCPIP_THREAD_PRIO              2

#define DEFAULT_THREAD_STACKSIZE       512
#define DEFAULT_THREAD_PRIO            2

#define DEFAULT_ACCEPTMBOX_SIZE        2
#define DEFAULT_TCP_RECVMBOX_SIZE      2
#define DEFAULT_UDP_RECVMBOX_SIZE      2

#define LWIP_DISABLE_TCP_SANITY_CHECKS 1

#define USE_SLIPIF 0

#define sio_send
#define sio_read
#define sio_open
#define sio_tryread

#endif /* LWIPOPTS_H */
