
#ifndef ETHERNET_DHCP_H_
#define ETHERNET_DHCP_H_

uint8_t DhcpServerIP[4];
uint8_t DhcpTransactID[4];
uint32_t DhcpLeaseTime;
uint8_t macaddr[6];
struct DHCPHeader *packet;
uint8_t DhcpTimeout;
uint8_t DhcpRetries;

struct BootPHeader {
	uint8_t opcode;
	uint8_t Htype;
	uint8_t Hlen;
	uint8_t Hops;
	uint8_t Xid[4];
	uint16_t Secs;
	uint16_t Flag;
	uint8_t CIAddr[4];
	uint8_t YIAddr[4];
	uint8_t SIAddr[4];
	uint8_t GIAddr[4];
	uint8_t CHAddr[16];
	uint8_t SName[64];
	uint8_t File[128];
};
struct options{
	uint8_t nestmask[4];
	uint8_t default_router[4];
	uint8_t dnsaddr[4];
	uint8_t mss_type;
	uint8_t server_id[4];
	uint8_t lease_time[4];
};
struct options s;
struct DHCPHeader{
	struct BootPHeader bootp;		//BOOTP header
	uint8_t	cookie[4];			//magic cookie value
	uint8_t	options[1];		//DHCP options
};

#define BOOTP_HEADER_LEN		236	//length of BOOTP header not including options
//
#define BOOTP_OP_BOOTREQUEST	1	//BOOTP Request operation (message from client to server)
#define BOOTP_OP_BOOTREPLY		2	//BOOTP Reply operation (message from server to client)
//
#define BOOTP_HTYPE_ETHERNET	1	//Hardware type for ethernet protocol
#define BOOTP_HLEN_ETHERNET		6	//Length of ethernet MAC address
//
#define DHCP_HEADER_LEN			240	//length of DHCP header not including options

//Code for DHCP option field
#define DHCP_OPT_PAD			0	//token padding value (make be skipped)
#define DHCP_OPT_NETMASK		1	//subnet mask client should use (4 byte mask)
#define DHCP_OPT_ROUTERS		3	//routers client should use (IP addr list)
#define DHCP_OPT_TIMESERVERS	4	//time servers client should use (IP addr list)
#define DHCP_OPT_NAMESERVERS	5	//name servers client should use (IP addr list)
#define DHCP_OPT_DNSSERVERS		6	//DNS servers client should use (IP addr list)
#define DHCP_OPT_HOSTNAME		12	//host name client should use (string)
#define DHCP_OPT_DOMAINNAME		15	//domain name client should use (string)
#define DHCP_OPT_REQUESTEDIP	50	//IP address requested by client (IP address)
#define DHCP_OPT_LEASETIME		51	//DHCP Lease Time (uint32 seconds)
#define DHCP_OPT_DHCPMSGTYPE	53	//DHCP message type (1 byte)
#define DHCP_OPT_SERVERID		54	//Server Identifier (IP address)
#define DHCP_OPT_PARAMREQLIST	55	//Paramerter Request List (n OPT codes)
#define DHCP_OPT_RENEWALTIME	58	//DHCP Lease Renewal Time (uint32 seconds)
#define DHCP_OPT_REBINDTIME		59	//DHCP Lease Rebinding Time (uint32 seconds)
#define DHCP_OPT_CLIENTID		61	//DHCP Client Identifier
#define DHCP_OPT_END			255 //token end value (marks end of options list)
//Code for DHCP message type
#define DHCP_MSG_DHCPDISCOVER	1	//DISCOVER is broadcast by client to solicit OFFER from any/all DHCP servers.
#define DHCP_MSG_DHCPOFFER		2	//OFFER(s) are made to client by server to offer IP address and config info.
#define DHCP_MSG_DHCPREQUEST	3	//REQUEST is made my client in response to best/favorite OFFER message.
#define DHCP_MSG_DHCPDECLINE	4	//DECLINE may be sent by client to server to indicate IP already in use.
#define DHCP_MSG_DHCPACK		5	//ACK is sent to client by server in confirmation of REQUEST, contains config and IP.
#define DHCP_MSG_DHCPNAK		6	//NAK is sent to client by server to indicate problem with REQUEST.
#define DHCP_MSG_DHCPRELEASE	7	//RELEASE is sent by client to server to relinquish DHCP lease on IP address, etc.
#define DHCP_MSG_DHCPINFORM		8	//INFORM is sent by client to server to request config info, IP address configured locally.

#define DHCP_UDP_SERVER_PORT	67	//UDP port where DHCP requests should be sent
#define DHCP_UDP_CLIENT_PORT	68	//UDP port clients will receive DHCP replies
//
#define DHCP_TIMEOUT	10
#define DHCP_RETRIES	3

uint8_t *add_msg_type(uint8_t *opt, uint8_t type);
uint8_t *add_req_opions(uint8_t *opt);
uint8_t *add_sever_id(uint8_t *opt,uint8_t type, uint8_t *id);
uint8_t *add_end(uint8_t *opt);
uint8_t parse_opions(uint8_t *opt, int len);

void dhcpInit(void);
void dhcpDiscover(void);
void dhcpRequest(struct DHCPHeader* packet, uint8_t *severid);
void dhcpIn(int len, struct DHCPHeader* packet);
void dhcpService(void);
void dhcpTimer(void);
#endif /* ETHERNET_DHCP_H_ */
