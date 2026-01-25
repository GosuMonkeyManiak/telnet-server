#pragma once

/* ### Debuging ### */

/* === Assertion handling to disable in cc.h --> define LWIP_NOASSERT === */

/* === Statistics === */

/* === Debug messages === */

#define LWIP_DEBUG

#define DHCP_DEBUG LWIP_DBG_ON
#define NETIF_DEBUG LWIP_DBG_ON

/* === Performance === */
#define LWIP_PERF 0

/* ### Infrastructure ### */

/* === OS === */
#define NO_SYS	1

/* === Timers === */
#define LWIP_TIMERS 1
#define LWIP_TIMERS_CUSTOM 0

/* === Memcpy look at doxygen doc === */

/* === Core locking and MPU === */
#define LWIP_MPU_COMPATIBLE 0
#define LWIP_TCPIP_CORE_LOCKING 0
#define SYS_LIGHTWEIGHT_PROT 0

/* === Heap and memory pools === */
#define MEMP_MEM_INIT 1
#define MEM_ALIGNMENT 4
#define MEM_SIZE (4 * 1024)
#define MEMP_OVERFLOW_CHECK 2
#define MEMP_SANITY_CHECK 1 //TODO: maybe in debug
#define MEM_OVERFLOW_CHECK 2
#define MEM_SANITY_CHECK 1 //TODO: maybe in debug

/* === Internal memory pools === */
#define MEMP_NUM_PBUF 16
#define MEMP_NUM_RAW_PCB 4
#define MEMP_NUM_UDP_PCB 4
#define MEMP_NUM_REASSDATA 5
#define MEMP_NUM_FRAG_PBUF 15
#define PBUF_POOL_SIZE 10

/* === SNMP MIB2 callbacks === */
#define LWIP_MIB2_CALLBACKS 0

/* === Multicast === */
//#define LWIP_MULTICAST_TX_OPTIONS

/* === Threading === */

/* === Checksums === */

/* === Hooks === */

/* ### Callback-style APIs ### */

/* === RAW === */
#define LWIP_RAW 1

/* === DNS === */
#define LWIP_DNS 0

/* === UDP === */
#define LWIP_UDP 1
#define LWIP_UDPLITE 1

/* === TCP === */
#define LWIP_TCP 0

/* ### Thread-safe APIs ### */

/* === Netconn === */
#define LWIP_NETCONN 0
//LWIP_NETCONN_SEM_PER_THREAD TODO

/* === Sockets === */
#define LWIP_SOCKET 0

/* ### IPv4 ### */
#define LWIP_IPV4 1
#define IP_REASS_MAX_PBUFS 10

/* === ARP === */
#define LWIP_ARP 1
#define ARP_TABLE_SIZE 3

/* === ICMP === */
#define LWIP_ICMP 1

/* === DHCP === */
#define LWIP_DHCP 1

/* === AUTOIP === */
#define LWIP_AUTOIP 0

/* === ACD === */

/* === IGMP === */
#define LWIP_IGMP 0

/* ### PBUF ### */
#define PBUF_POOL_BUFSIZE 1536

/* ### NETIF ### */
#define LWIP_SINGLE_NETIF 1
#define LWIP_NETIF_HOSTNAME 1
#define LWIP_NETIF_TX_SINGLE_PBUF 1

/* === Loopback === */

// STUPID SIO NEEDED BEACUSE NOT BUILD
#define PPP_SUPPORT 0
#define PPPOS_SUPPORT 0
#define SLIPIF 0