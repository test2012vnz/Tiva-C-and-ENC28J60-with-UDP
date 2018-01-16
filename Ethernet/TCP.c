#include "../inc.h"

//Ham khoi tao so tuan tu cho mot phien TCP
// Hien tai su dung gia tri 1234 (may tinh thuong dung gia tri thoi gian hien tai)
uint32_t TCPInitSequenceNumber(void)
{
	return (1);
}
//Ham dong mot phien TCP
void TCPCloseSession(uint8_t socketnum)
{
	tcpSessionTable[socketnum].sesState = TCP_STATE_CLOSED;

}
void TCPInit(void)
{
	uint8_t i = 0;
	for(i=0; i<TCP_MAX_SESSION; i++)
		TCPCloseSession(i);
}

//Tim mot session TCP dang roi
uint8_t TCPGetFreeSession(){
	uint8_t i;
	for(i=0; i<TCP_MAX_SESSION; i++){
		if (tcpSessionTable[i].sesState == TCP_STATE_CLOSED)
			return i;
	}
	//no free closed socket fount! -> kick an TIMED_WAIT socket
	for(i=0; i<TCP_MAX_SESSION; i++){
		if (tcpSessionTable[i].sesState == TCP_STATE_TIMED_WAIT){
			TCPCloseSession(i);
			return i;
		}
	}
	//no more free sockets ... return invalid val
	return(TCP_MAX_SESSION);
}
uint8_t  TCPCreateSession(uint16_t sourcePort,uint16_t destPort, uint8_t *dataBuffer,uint16_t dataLen,uint8_t *dstIP)
{
	uint8_t i;
	i = TCPGetFreeSession();
	if(i >= TCP_MAX_SESSION)
		i = 0;	//force session 0
	tcpSessionTable[i].srcPort = sourcePort;
	tcpSessionTable[i].desPort = destPort;
	tcpSessionTable[i].sesState = TCP_STATE_LISTEN;		//Current state of TCP session
	tcpSessionTable[i].srcWin = 8192;//NETSTACK_BUFFERSIZE - ETH_HEADER_LEN - IP_HEADER_LEN - TCP_HEADER_LEN - 16;
	tcpSessionTable[i].desWin = tcpSessionTable[i].srcWin;
	tcpSessionTable[i].timeout = TCP_TIMEOUT;		//Session time out
	tcpSessionTable[i].dataLen = dataLen;
	tcpSessionTable[i].dataBuffer = dataBuffer;
	memcpy(tcpSessionTable[i].desIP,dstIP,sizeof(tcpSessionTable[i].desIP));
	return i;
	//tcpSessionTable[i].pSession = pSession;
}
//Duoc goi moi giay de kiem tra Time out cho cac phien TCP,
// giai phong cac phine TCP bi treo
void TCPCheckTimeOut(void){
	uint8_t i;
	for(i=0; i<TCP_MAX_SESSION; i++){
		//decrement ttl:
		if ((tcpSessionTable[i].sesState != TCP_STATE_CLOSED) && (tcpSessionTable[i].sesState != TCP_STATE_LISTEN)){
			if(tcpSessionTable[i].timeout)
				tcpSessionTable[i].timeout--;

			//if socket TTL count is zero, close this socket!
			if (tcpSessionTable[i].timeout == 0){
				TCPCloseSession(i);
			}
		}
	}
}

//Tinh checksum cho goi TCP
uint16_t checksum(uint8_t *buffer, uint16_t len, uint32_t csum32)
{
	uint16_t  res16 = 0x0000;
	uint8_t data_hi;
	uint8_t data_lo;
	while(len > 1){
		data_hi = *buffer++;
		data_lo = *buffer++;
		res16 = (((uint16_t)data_hi << 8) + data_lo);
		csum32 = csum32 + res16;
		len -=2;
	}
	if(len > 0){
		data_hi = *buffer;
		res16   = (uint16_t)data_hi<<8;
		csum32 = csum32 + res16;
	}
	while(csum32>>16)
		csum32 = (csum32 & 0xFFFF)+ (csum32 >> 16);
	//csum32 = ((csum32 & 0x0000FFFF)+ ((csum32 & 0xFFFF0000) >> 16));
	res16  =~(csum32 & 0x0000FFFF);
	return (res16);
}
//Gui di mot goi TCP
void TCPPackedSend(struct tcpSession *pSession, uint8_t Flags, uint16_t len, uint8_t *dataBuffer)
{
	uint16_t tmp;
	uint32_t checksum32;
	uint8_t mask[] = {0,0,0,0};
	//Make pointer to TCP header
	struct tcpHeader *tcpHeader;
	struct ipHeader *ipHeader;
	//Neu dang syn thi them option ve MSS

	if(Flags & TCP_SYN_FLAG){
		//Option data
		dataBuffer[0] = 0x02;
		dataBuffer[1] = 0x04;
		dataBuffer[2] = (MAX_SEGMENT_SIZE >> 8) & 0xff;
		dataBuffer[3] = MAX_SEGMENT_SIZE & 0xff;
		dataBuffer[4] = 0x01;
		dataBuffer[5] = 0x03;
		dataBuffer[6] = 0x03;
		dataBuffer[7] = 0x00;
		//Move data pointer to make room for TCP header
	}
	dataBuffer -= TCP_HEADER_LEN;
	tcpHeader = (struct tcpHeader*)dataBuffer;
	//Fill UDP header
	tcpHeader->Srcport = HTONS(pSession->srcPort);
	tcpHeader->DstPort = HTONS(pSession->desPort);
	memcpy(tcpHeader->SeqNumber,pSession->SeqNumber,sizeof(tcpHeader->SeqNumber));
	memcpy(tcpHeader->AckNumber,pSession->AckNumber,sizeof(tcpHeader->AckNumber));

	parseHexIP(u32con(pSession->SeqNumber)+len, mask);
	memcpy(pSession->SeqNumber,mask,sizeof(pSession->SeqNumber));

	if(Flags & (TCP_FIN_FLAG|TCP_SYN_FLAG))
	{
		parseHexIP(u32con(pSession->SeqNumber)+1, mask);
		memcpy(pSession->SeqNumber,mask,sizeof(pSession->SeqNumber));
		//(pSession->SeqNumber)++;
	}

	//tcpHeader->AckNumber = HTONL(pSession->AckNumber);
	if(Flags & TCP_SYN_FLAG){
		tcpHeader->Offset = (0x07<<4);
		len += (TCP_HEADER_LEN + 8);
	}else{
		tcpHeader->Offset = (0x05<<4);
		len += TCP_HEADER_LEN;
	}
	tcpHeader->Flags = Flags;
	tcpHeader->Windown = HTONS(pSession->srcWin);//((NETSTACK_BUFFERSIZE-20-14));
	tcpHeader->Checksum = 0;
	tcpHeader->Urgenpt = 0x0000;
	//Generate checksum
	ipHeader = (struct ipHeader*)(dataBuffer-IP_HEADER_LEN);
	memcpy(ipHeader->srcIPAddr,GetInfo()->ipAddr,sizeof(ipHeader->srcIPAddr));
	memcpy(ipHeader->desIPAddr,pSession->desIP,sizeof(ipHeader->desIPAddr));
	ipHeader->Checksum = HTONS(len);
	ipHeader->TTL = 0x00;
	ipHeader->Protocol = IP_PROTO_TCP;
	checksum32 = 0;
	tmp = len + 12;
	tmp = checksum (((uint8_t *)ipHeader+8), tmp, checksum32);
	tcpHeader->Checksum = HTONS(tmp);
	tcpPKtest = tcpHeader;
	//while(1);
	ipSend(u32con(pSession->desIP), IP_PROTO_TCP, len, (uint8_t *)tcpHeader);
	//while(1);
}

uint8_t TCPSend(uint32_t desIP, uint8_t Flags,uint8_t *dataBuffer)
{
	int i;
	uint16_t tmp;
	uint32_t checksum32;
	struct tcpHeader *tcpHeader;
	struct ipHeader *ipHeader;
	for(i=0;i<TCP_MAX_SESSION;i++)
	{
		if(desIP==u32con(tcpSessionTable[i].desIP))
			break;
	}
	if(i<TCP_MAX_SESSION)
	{
		dataBuffer -= TCP_HEADER_LEN;
		tcpHeader = (struct tcpHeader*)dataBuffer;
		tcpHeader->Srcport = HTONS(tcpSessionTable[i].srcPort);
		tcpHeader->DstPort = HTONS(tcpSessionTable[i].desPort);
		memcpy(tcpHeader->SeqNumber,tcpSessionTable[i].SeqNumber,sizeof(tcpHeader->SeqNumber));
		memcpy(tcpHeader->AckNumber,tcpSessionTable[i].AckNumber,sizeof(tcpHeader->AckNumber));

		tcpHeader->Flags = Flags;
		tcpHeader->Windown = HTONS(tcpSessionTable[i].srcWin);//((NETSTACK_BUFFERSIZE-20-14));
		tcpHeader->Checksum = 0;
		tcpHeader->Urgenpt = 0x0000;

		ipHeader = (struct ipHeader*)(dataBuffer-IP_HEADER_LEN);
		memcpy(ipHeader->srcIPAddr,GetInfo()->ipAddr,sizeof(ipHeader->srcIPAddr));
		memcpy(ipHeader->desIPAddr,tcpSessionTable[i].desIP,sizeof(ipHeader->desIPAddr));
		ipHeader->Checksum = HTONS(strlen((char *)dataBuffer));
		ipHeader->TTL = 0x00;
		ipHeader->Protocol = IP_PROTO_TCP;
		checksum32 = 0;
		tmp = strlen((char *)dataBuffer) + 12;
		tmp = checksum (((uint8_t *)ipHeader+8), tmp, checksum32);
		tcpHeader->Checksum = HTONS(tmp);
		tcpPKtest = tcpHeader;
		//while(1);
		ipSend(desIP, IP_PROTO_TCP, strlen((char *)dataBuffer), (uint8_t *)tcpHeader);
	}
	return 1;
}
void TCPProcess(uint8_t *buffer, uint16_t len)
{

	uint8_t mask[] = {0,0,0,0};
	uint8_t i,ipHeaderLen,tcpHeaderLen;
	uint16_t dataLen;
	uint8_t tmp[] = {0,0,0,0};
	struct ipHeader* ipHeader;
	struct tcpHeader* tcpHeader;
	uint8_t *tcpData;

	ipHeader = (struct ipHeader*)(buffer);
	ipHeaderLen = ((ipHeader->verHdrLen) & 0x0F) << 2;
	tcpHeader = (struct tcpHeader*)(buffer+IP_HEADER_LEN);
	tcpHeaderLen = ((tcpHeader->Offset) & 0xF0) >> 2;
	tcpPKtest = tcpHeader;								////
	tcpData = (buffer+ipHeaderLen+tcpHeaderLen);
	dataLen = HTONS(ipHeader->Len) - (ipHeaderLen + tcpHeaderLen);

	for(i = 0; i < TCP_MAX_SESSION; i++){	//Check session table
		if(tcpSessionTable[i].sesState != TCP_STATE_CLOSED){		//If not closed session
			if(tcpSessionTable[i].srcPort == HTONS((tcpHeader->DstPort))){	//If matched local port 	//
				if(tcpSessionTable[i].desPort == HTONS((tcpHeader->Srcport))&&(u32con(tcpSessionTable[i].desIP) == u32con((ipHeader->srcIPAddr)))){
					break;	//Thoat khoi vong lap for, luc nay gia tri cua i chinh la chi so cua phien TCP tuong ung
				}
			}
		}
	}
	//UARTprintf("\n%d\n",i);
	if(i == TCP_MAX_SESSION){	//Neu khong co 1 phien TCP dang ton tai cho goi nay
		//Tim 1 phien dang o trang thai LISTEN (doi ket noi) cho local port nay
		for(i=0; i < TCP_MAX_SESSION; i++){
			if(tcpSessionTable[i].sesState == TCP_STATE_LISTEN){	//**********************//
				if(tcpSessionTable[i].srcPort == HTONS((tcpHeader->DstPort))){	//If matched local port
					//Cap nhat remote port va remote IP
					tcpSessionTable[i].desPort = HTONS((tcpHeader->Srcport));
					memcpy(tcpSessionTable[i].desIP,ipHeader->srcIPAddr,sizeof(tcpSessionTable[i].desIP));
					//Dong thoi tao ra 1 session moi de cho ket noi khac den local port nay
					//TCPCreateSession(60000,HTONS(tcpHeader->Srcport), 0,1,ipHeader->srcIPAddr);
					break;
				}
			}
		}
		//Neu khong co phien nao dang o trang thai listen// thi tao 1 phien moi  // :))
	}
	if(i==TCP_MAX_SESSION)
		i = TCPCreateSession(60000,HTONS(tcpHeader->Srcport), 0,1,ipHeader->srcIPAddr);
//	if(i==TCP_MAX_SESSION)
//		UARTprintf("\nKhong tim thay secsion nao");
//	else UARTprintf("\ntim thay session %d",i);
//********************************************************************************************************************//
	//Bat dau xu ly giao thuc
	tcpSessionTable[i].timeout = TCP_TIMEOUT;	//Reset lai gia tri Time out
	//Truong hop nhan duoc yeu cau reset lai ket noi
	if ((tcpHeader->Flags) & TCP_RST_FLAG){
		//Chap nhan dong ket noi
		TCPCloseSession(i);
		return;
	}
	//Kiem tra trang thai hien tai cua phien TCP
	switch(tcpSessionTable[i].sesState)
	{
	case TCP_STATE_LISTEN:
		//Chi xu ly neu co SYN duoc set (yeu cau thiet lap ket noi)
		if((tcpHeader->Flags) == TCP_SYN_FLAG)
		{
			//Chuyen sang trang thai ke tiep la TCP_STATE_SYN_RECEIVED
			tcpSessionTable[i].sesState = TCP_STATE_SYN_RECEIVED;
			//Khoi tao gia tri sequence
			//parseHexIP(TCPInitSequenceNumber(), mask);
			memcpy(tcpSessionTable[i].SeqNumber,tcpHeader->SeqNumber,sizeof(tcpSessionTable[i].SeqNumber));
			//Ack chinh la so tuan tu nhan duoc cong 1
			parseHexIP(u32con(tcpHeader->SeqNumber)+1, mask);
			memcpy(tcpSessionTable[i].AckNumber, mask, sizeof(tcpSessionTable[i].AckNumber));
			tcpSessionTable[i].desWin = HTONS(tcpHeader->Windown);
			//Goi tra xac nhan va co SYN (SYN & ACK)
			TCPPackedSend(&tcpSessionTable[i], (TCP_SYN_FLAG|TCP_ACK_FLAG), 0, tcpData);
		}
		break;
	case TCP_STATE_SYN_RECEIVED:
		//Neu co co ACK (cho ban tin SYN & ACK truoc do)
		if((tcpHeader->Flags) == TCP_ACK_FLAG){
			//Kiem tra ack trong goi tin den, neu dung thi thiet lap ket noi hoan tat
			if(u32con(tcpSessionTable[i].SeqNumber)==u32con(tcpHeader->AckNumber))
			{
				UARTprintf("\nTCP Connection established\n");
				tcpSessionTable[i].sesState = TCP_STATE_ESTABLISHED;
				//Goi tiep theo gui di se co co ACK
				tcpSessionTable[i].nextAck = 1;
			}
		}
		break;
		//Truong hop ket noi da duoc thiet lap
	case TCP_STATE_ESTABLISHED:
		//Neu nhan duoc yeu cau ket thuc ket noi tu client
		if((tcpHeader->Flags) & TCP_FIN_FLAG){
			//Chuyen sang trang thai ke tiep la trang thai cho ACK cuoi
			//Dung ra o day phai chuyen sang trang thai TCP_STATE_CLOSE_WAIT nhung khong can thiet
			//vi o day ta co the dong ket noi ngay ma khong can cho gui xong du lieu
			tcpSessionTable[i].sesState = TCP_STATE_LAST_ACK;
			//Cap nhat ack
			parseHexIP(u32con(tcpHeader->SeqNumber) + dataLen + 1, mask);	//	//Tang 1 doo co FIN
			memcpy(tcpSessionTable[i].AckNumber,mask,sizeof(tcpSessionTable[i].AckNumber));
			//Gui xac nhan ACK cho yeu cau dong ket noi dong thoi thong bao san sang dong ket noi
			TCPPackedSend(&tcpSessionTable[i],TCP_ACK_FLAG,0,tcpData);
			TCPPackedSend(&tcpSessionTable[i],(TCP_FIN_FLAG|TCP_ACK_FLAG),0,tcpData);
			//Dang le truyen o trang thai CLOSE_WAIT nhung ta thuc hien o day luon
			TCPCloseSession(i);
			UARTprintf("\nTCP Disconnection\n");
		} // Neu khong (khi dang truyen du lieu)
		else
		{
			if((tcpHeader->Flags) & TCP_ACK_FLAG)
			{ //Neu co co ACK thi kiem tra gia tri ACK
				memcpy(tcpSessionTable[i].lastRxAck,tcpHeader->AckNumber,sizeof(tcpSessionTable[i].lastRxAck));
				if(u32con(tcpSessionTable[i].SeqNumber)==u32con(tcpHeader->AckNumber))
				{
					//UARTprintf("\nGot ACK");						//*********//
				}
				else
				{	//Phia ben kia khong nhan duoc du thong tin
					//Sua loi o day
					memcpy(tcpSessionTable[i].SeqNumber,tcpHeader->AckNumber,sizeof(tcpSessionTable[i].SeqNumber));
					//UARTprintf("\nMiss ACK");						//*********//
				}
			}
			//Ket thuc kiem tra ACK
			//Kiem tra sequence number
			memcpy(tmp,tcpHeader->SeqNumber,sizeof(tcpHeader->SeqNumber));
			//Neu khong dung goi dang cho nhan
			if(u32con(tmp)!=u32con(tcpSessionTable[i].AckNumber))
			{	//UARTprintf("\nIncorrec sq:");						//*********//
				if(u32con(tmp)<u32con(tcpSessionTable[i].AckNumber))
				{
					//Neu dang doi du lieu bat dau tu byte thu n nhung ta nhan duoc doan du lieu bat dau tu (n-k)
					//Tinh phan du lieu thua (k = n - (n-k))
					parseHexIP(u32con(tcpSessionTable[i].AckNumber)-u32con(tmp), mask);
					memcpy(tmp,mask,4);
					//Neu doan du lieu thua it hon du lieu nhan duoc //Bo di phan du lieu thua, nhan phan con lai
					if(u32con(tmp)<dataLen)
					{
						tcpData += *tmp;
						dataLen = dataLen - (uint16_t)(u32con(tmp));
					}
					else{//Neu tat ca du lieu nhan duoc deu thua
						//Gui lai ACK, bo goi vua nhan duoc
						dataLen = 0;
						TCPPackedSend(&tcpSessionTable[i],(TCP_ACK_FLAG),0,tcpData);
					}
				}
				else{//Neu seq > ack (tuc la co 1 doan du lieu bi mat)
					//tmp > tcp....Yeu cau gui lai
					TCPPackedSend(&tcpSessionTable[i],(TCP_ACK_FLAG),0,tcpData);
					return;
				}
			}
			//Neu thuc thi den day nghia la sequence number == ack number (chinh xac)
			//--Ket thuc kiem tra so tuan tu
			//Kiem tra chieu dai buffer de chac chan la chieu dai du lieu nhan duoc khong qua buffer
			if(tcpData>(buffer + ETHERNET_BUFFER_SIZE))
				tcpData = (buffer + ETHERNET_BUFFER_SIZE);
			if((tcpData + dataLen) > buffer + ETHERNET_BUFFER_SIZE)
				dataLen = (buffer + ETHERNET_BUFFER_SIZE) - tcpData;
			//Cap nhat ack cho lan nhan ke tiep
			parseHexIP(u32con(tcpSessionTable[i].AckNumber) + dataLen, mask);
			memcpy(tcpSessionTable[i].AckNumber,mask,sizeof(tcpSessionTable[i].AckNumber));
			//Goi tiep theo gui di se co co ACK
			tcpSessionTable[i].nextAck = 1;
			//Kiem tra neu co yeu cau gui lai
			if((tcpHeader->Flags)&TCP_PSH_FLAG)
			{
				TCPPackedSend(&tcpSessionTable[i],TCP_ACK_FLAG,0,tcpData);
				//TCPPackedSend(&tcpSessionTable[i],TCP_PSH_FLAG|TCP_NON_FLAG,3,"nam");
			}
			UARTprintf("\n%s",tcpData);
			//Goi ham xu ly lop ung dung
			//if(dataLen!=0)
			///////////////////////////////////////////////
			//--Ket thuc xu ly truong hop dang truyen du lieu
		}
		break;
	}
}


