#include "../inc.h"

const uint8_t magic_cookie[4] = {0x63, 0x82, 0x53, 0x63};

uint8_t *add_msg_type(uint8_t *opt, uint8_t type)
{
	  *opt++ = DHCP_OPT_DHCPMSGTYPE;
	  *opt++ = 1;
	  *opt++ = type;
	  return opt;
}
uint8_t *add_req_opions(uint8_t *opt)
{
	  *opt++ = DHCP_OPT_PARAMREQLIST;
	  *opt++ = 3;
	  *opt++ = DHCP_OPT_NETMASK;
	  *opt++ = DHCP_OPT_ROUTERS;
	  *opt++ = DHCP_OPT_DNSSERVERS;
	  return opt;
}
uint8_t *add_sever_id(uint8_t *opt,uint8_t type, uint8_t *id)
{
	  *opt++ = type;
	  *opt++ = 4;
	  memcpy(opt, id, 4);
	  return opt + 4;
}
uint8_t *add_end(uint8_t *opt)
{
	  *opt++ = DHCP_OPT_END;
	  return opt;
}

uint8_t parse_opions(uint8_t *opt, int len)
{
	uint8_t *end = opt +len;
	uint8_t type =0;
	while(opt < end)
	{
		switch(*opt)
		{
		case DHCP_OPT_NETMASK:
			memcpy(s.nestmask,opt+2,4);
			break;
		case DHCP_OPT_ROUTERS:
			memcpy(s.default_router,opt+2,4);
			break;
		case DHCP_OPT_DNSSERVERS:
			memcpy(s.dnsaddr,opt+2,4);
			break;
		case DHCP_OPT_DHCPMSGTYPE:
			type = *(opt+2);
			break;
		case DHCP_OPT_SERVERID:
			memcpy(s.server_id,opt+2,4);
			break;
		case DHCP_OPT_LEASETIME:
			memcpy(s.lease_time,opt+2,4);
			break;
		case DHCP_OPT_END:
			return type;
		}
		opt += opt[1]+2;
	}
	return type;
}

void dhcpInit(void)
{
	enc28j60GetMac(macaddr);
	memcpy(DhcpTransactID,macaddr,sizeof(DhcpTransactID));
	DhcpLeaseTime =0;
	DhcpTimeout = 1;
	DhcpRetries = DHCP_RETRIES;

	//UARTprintf("\nDHCP Transaction ID.....: %x.%x.%x.%x.\n",DhcpTransactID[0],DhcpTransactID[1],DhcpTransactID[2],DhcpTransactID[3]);
}

void dhcpDiscover(void)
{
	uint8_t* opt;
	packet = (struct DHCPHeader*)(ethGetBuffer() + ETH_HEADER_LEN + IP_HEADER_LEN + 8);   // 8 = udp header legnht
	packet->bootp.opcode = BOOTP_OP_BOOTREQUEST;
	packet->bootp.Htype = BOOTP_HTYPE_ETHERNET;
	packet->bootp.Hlen = BOOTP_HLEN_ETHERNET;
	packet->bootp.Hops =0;
	memcpy(packet->bootp.Xid,DhcpTransactID,sizeof(packet->bootp.Xid));
	packet->bootp.Secs =0;
	packet->bootp.Flag = HTONS(1);
	memcpy(packet->bootp.CIAddr, GetInfo()->ipAddr, sizeof(packet->bootp.CIAddr));
	memset(packet->bootp.YIAddr, 0, sizeof(packet->bootp.YIAddr));
	memset(packet->bootp.SIAddr, 0, sizeof(packet->bootp.SIAddr));
	memset(packet->bootp.GIAddr, 0, sizeof(packet->bootp.GIAddr));
	enc28j60GetMac(&packet->bootp.CHAddr[0]);
	memset(packet->bootp.SName, 0, sizeof(packet->bootp.SName));
	memset(packet->bootp.File, 0, sizeof(packet->bootp.File));

	memcpy(packet->cookie, magic_cookie, sizeof(magic_cookie));
	opt = add_msg_type(packet->options, DHCP_MSG_DHCPDISCOVER);
	opt = add_req_opions(opt);
	opt = add_end(opt);

	udpSend(0xFFFFFFFF, DHCP_UDP_SERVER_PORT, DHCP_UDP_CLIENT_PORT, DHCP_HEADER_LEN+3+1+8, (uint8_t*)packet);

}

void dhcpRequest(struct DHCPHeader* packet, uint8_t *severid)
{
	uint8_t* opt;
	packet->bootp.opcode = BOOTP_OP_BOOTREQUEST;
	memset(packet->bootp.CIAddr, 0, sizeof(packet->bootp.CIAddr));
	memset(packet->bootp.GIAddr, 0, sizeof(packet->bootp.GIAddr));
	opt = add_msg_type(packet->options, DHCP_MSG_DHCPREQUEST);
	opt = add_sever_id(opt, DHCP_OPT_REQUESTEDIP, packet->bootp.YIAddr);
	opt = add_sever_id(opt, DHCP_OPT_SERVERID, severid);
	memset(packet->bootp.YIAddr, 0, sizeof(packet->bootp.YIAddr));
	udpSend(0xFFFFFFFF, DHCP_UDP_SERVER_PORT, DHCP_UDP_CLIENT_PORT, DHCP_HEADER_LEN+3+6+6+6+8+1, (uint8_t*)packet);

}

void dhcpIn(int len, struct DHCPHeader* packet)
{
	uint8_t type;
	if(u32con(packet->bootp.Xid)!=u32con(DhcpTransactID))
		return;

	type=parse_opions(packet->options,len);
	if(type==DHCP_MSG_DHCPOFFER)
		dhcpRequest(packet, s.server_id);
	if(type == DHCP_MSG_DHCPACK)
		{
			ipConfig(packet->bootp.YIAddr, s.nestmask, s.default_router, GetInfo()->port);
			DhcpLeaseTime = u32con(s.lease_time);
			arpSetAddress(macaddr, GetInfo()->ipAddr);
			UARTprintf("Recived IP!\n");
		}
}

void dhcpService(void)
{
	if(DhcpRetries && DhcpTimeout == 0 &&  u32con(GetInfo()->ipAddr)==0x00000000){ //&& (DhcpTimeout == 0
		DhcpRetries--;
		DhcpTimeout = DHCP_TIMEOUT;
		dhcpDiscover();
	}
}

void dhcpTimer(void)
{
	if(DhcpLeaseTime)
		DhcpLeaseTime--;
	if(DhcpTimeout){
		DhcpTimeout--;
	}
}
