#include <string.h>

#include "lwip/netif.h"
#include "lwip/etharp.h"
#include "lwip/dhcp.h"

#include "ethernet/ethernet_if.h"

#include "enc624j600/enc624j600_driver.h"
#include "utils/flags.h"

enum {
	FLAG_LINK_LAST_STATE = 0
};

static uint16_t flags = 0;
static struct netif eth_if;

static uint8_t frame_buffer[1536];
static uint16_t frame_length;

static void low_level_input() {

	enc624j600_receive(frame_buffer, &frame_length);
	
	struct pbuf *p;
	u16_t len = frame_length;

#if ETH_PAD_SIZE
	len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

	/* We allocate a pbuf chain of pbufs from the pool. */
	p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	if (p == NULL) {
		return;
	}

#if ETH_PAD_SIZE
	pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif

	memcpy(p->payload, frame_buffer, frame_length);
		
#if ETH_PAD_SIZE
	  pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
	  
	/* pass all packets to tcpip_input(), which decides what packets it supports */
	if (eth_if.input(p, &eth_if) != ERR_OK) {
		LWIP_DEBUGF(NETIF_DEBUG, ("tcpip_input(): IP input error\n"));
		pbuf_free(p);
		p = NULL;
	}
	  
	debug_print("\r\n Receive frame: len = %d.\r\n", p->len);
}

static err_t low_level_output(struct netif *netif, struct pbuf *p) {

#if ETH_PAD_SIZE
	pbuf_remove_header(p, ETH_PAD_SIZE); /* drop the padding word */
#endif
	
	if (p->next != NULL) {
		
		struct pbuf *q = pbuf_clone(PBUF_RAW, PBUF_POOL, p);
		
		if (q == NULL) {
			return ERR_MEM;
		}
		
		p = q;
	}
	
	enc624j600_transmit_result result = ENC_TRANSMIT_FAILED;
	
	if (enc624j600_link_status()) {
		result = enc624j600_transmit(p->payload, p->len);
		
		debug_print("\r\n Transmit frame: len = %d.\r\n", p->len);
	}
	
#if ETH_PAD_SIZE
	pbuf_add_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

	if (result != ENC_TRANSMIT_SUCCEEDED) {
		return ERR_IF;
	}
	
	return ERR_OK;
}

static err_t eth_netif_init(struct netif *netif) {

	LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
	/* Initialize interface hostname */
	netif->hostname = "enc624j600";
#endif /* LWIP_NETIF_HOSTNAME */

	netif->state = NULL;
	netif->name[0] = 'e';
	netif->name[1] = 't';
	netif->num = 1;
	
#if LWIP_IPV4
	netif->output = etharp_output;
#endif /* LWIP_IPV4 */
	
	netif->linkoutput = low_level_output;

	/* set MAC hardware address length */
	netif->hwaddr_len = ETHARP_HWADDR_LEN;

	/* set MAC hardware address */ // TODO:
//	if (enc_config->mac_address != NULL) {
//		netif->hwaddr[0] = enc_config->mac_address[0];
//		netif->hwaddr[1] = enc_config->mac_address[1];
//		netif->hwaddr[2] = enc_config->mac_address[2];
//		netif->hwaddr[3] = enc_config->mac_address[3];
//		netif->hwaddr[4] = enc_config->mac_address[4];
//		netif->hwaddr[5] = enc_config->mac_address[5];
//	} else {
		netif->hwaddr[0] = 0xD8;
		netif->hwaddr[1] = 0x80;
		netif->hwaddr[2] = 0x39;
		netif->hwaddr[3] = 0x7A;
		netif->hwaddr[4] = 0x46;
		netif->hwaddr[5] = 0x1A;
//	}

	/* maximum transfer unit -->  data + headers */
	netif->mtu = 1514;  // TODO: VLAN

	/* device capabilities */
	/* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
	netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

	/* Do whatever else is needed to initialize interface. */
	
	return ERR_OK;
}

void ethernet_if_init(void) {
	
	netif_add(&eth_if, IP4_ADDR_ANY, IP4_ADDR_ANY, IP4_ADDR_ANY, NULL, eth_netif_init, netif_input);
	netif_set_default(&eth_if);
	netif_set_up(&eth_if);
}

void ethernet_if_pump(void) {
	
	if (enc624j600_link_status()) {
		
		if (!get_flag(&flags, FLAG_LINK_LAST_STATE)) {
			
			debug_print("\r\n DHCP starting! \r\n");
			
			// TODO: dhcp ???
			dhcp_start(&eth_if);
			netif_set_link_up(&eth_if);
			set_flag(&flags, FLAG_LINK_LAST_STATE);
			
			return;
		}
		
	} else {
		
		if (get_flag(&flags, FLAG_LINK_LAST_STATE)) {
			// TODO: dhcp ???
			
			debug_print("\r\n DHCP stopping! \r\n");
			
			dhcp_stop(&eth_if);
			netif_set_link_down(&eth_if);
			reset_flag(&flags, FLAG_LINK_LAST_STATE);
		}
		
		return;
	}
	
	if (enc624j600_pending_frame()) {
		low_level_input();
	}
}