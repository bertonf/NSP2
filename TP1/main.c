#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <string.h>

typedef struct sArpPacket
{
u_int8_t ipBroadcast[6];
u_int8_t addrMacSrc[6];
u_int16_t protoArp[2];

}sArpPacket;
/*
void *ConvertIndian(void *val, int size)
{
  void *tmp = malloc(size);

  
}
*/
void printData(unsigned char *buffer, int size)
{
  int i = 0;

  while (i < size)
    {
      printf("%.2X ", buffer[i]);
      if ((i+1) % 8 == 0)
	printf("\n");
      ++i;
    }
  printf("\n");
}

int InitSocket()
{
  int sock;

  printf("Initialisation du socket.\n");
  sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (-1 == sock)
    {
      perror("socket()");
      return (-1);
    }
  return (sock);
}

int CheckIfr(int sock, struct ifreq *ifr, char *iface)
{
  printf("Verification de l'interface %s.\n", iface);
  strncpy(ifr->ifr_name, iface, IFNAMSIZ);
  if (-1 == ioctl(sock, SIOCGIFINDEX, ifr))
    {
      perror("ioctl");
      return (-1);
    }
   return (0);
}

int BindSocket(int sock, struct sockaddr_ll *sll, struct ifreq *ifr)
{
  sll->sll_family = AF_PACKET;
  sll->sll_ifindex = ifr->ifr_ifindex;
  sll->sll_protocol = htons(ETH_P_ALL);

  printf("Bind du socket sur l'interface.\n");
  if (-1 == bind(sock, (struct sockaddr*)sll, sizeof(*sll)))
    {
      perror("bind");
      return (-1);
    }
  return (0);
}

unsigned char *CreatePacket(struct ifreq *ifr, unsigned int ip, int sock)
{
  struct ethhdr ethhdr;
  struct arphdr arphdr;
  char macsrc[6];
  char ipsrc[4];
  char macdest[6];
  char ipdest[4];
  char *packet;
  long tmp;

  printf("Création du paquet arp.\n");
  memset(ethhdr.h_dest, 0xff, 6);
  if (-1 == ioctl(sock, SIOCGIFHWADDR, ifr))
    {
      perror("CreatePAcket() - ioctl");
      return (NULL);
    }
  memcpy(&tmp, ifr->ifr_ifru.ifru_hwaddr.sa_data, 6);
  tmp = htobe64((tmp<<16));
  //  memcpy(ethhdr.h_source, ifr->ifr_ifru.ifru_hwaddr.sa_data, 6);
  memcpy(ethhdr.h_source, &tmp, 6);
  ethhdr.h_proto = htobe16(ETHERTYPE_ARP);

  arphdr.ar_hrd = htobe16(1);
  arphdr.ar_pro = htobe16(ETHERTYPE_IP); /*0x0800*/
  arphdr.ar_hln = 6;
  arphdr.ar_pln = 4; /*ipv4 = 04, ipv6 = 16*/
  arphdr.ar_op = htobe16(1); /*Demande arp = 1, Reponse arp = 2*/

  //  memcpy(macsrc, ifr->ifr_ifru.ifru_hwaddr.sa_data, 6);
  memcpy(macsrc, &tmp, 6);
  memcpy(ipsrc, 
	 &((struct sockaddr_in *)&(ifr->ifr_ifru.ifru_addr))->sin_addr.s_addr, 4);
  *ipsrc = htobe32(*ipsrc);
  memset(macdest, 0, 6);
  memcpy(ipdest, &ip, 4);
  *ipdest = htobe32(*ipdest);

  packet = (char*)malloc(sizeof(struct ethhdr)+sizeof(struct arphdr)+20);
  memcpy(packet, &ethhdr, sizeof(struct ethhdr));
  memcpy(packet + sizeof(struct ethhdr), &arphdr, sizeof(struct arphdr));
  memcpy(packet + sizeof(struct ethhdr) + sizeof(struct arphdr), &macsrc, 6);
  memcpy(packet + sizeof(struct ethhdr) + sizeof(struct arphdr) + 6, &ipsrc, 4);
  memcpy(packet + sizeof(struct ethhdr) + sizeof(struct arphdr) + 10, &macdest, 6);
  memcpy(packet + sizeof(struct ethhdr) + sizeof(struct arphdr) + 16, &ipdest, 4);
  return (packet);
}

int  IsArpAnswer(char *recvpacket)
{
  struct ethhdr *ethhdr;
  struct arphdr *arphdr;

  ethhdr = (struct ethhdr*)recvpacket;
  arphdr = (struct arphdr*)(recvpacket+sizeof(struct ethhdr));
  
  if (ethhdr->h_proto == 0x0608)
    {
      printf("IS ARP\n");
      printData(recvpacket, sizeof(struct ethhdr) + sizeof(struct arphdr) + 20);
      if (arphdr->ar_op == htobe16(2))
	{
	  printf("IS ANSWER\n");
	  return (0);
	}
    }
  return (-1);
}

void AnalysePacket(char *recvpacket)
{
  char *macaddr;

  memcpy(macaddr, recvpacket + sizeof(struct ethhdr) + sizeof(struct arphdr) + 10, 6);
  
  printf("Mac Addr = %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n", macaddr[0],
	 macaddr[1],macaddr[2],macaddr[3],macaddr[4],macaddr[5]);
}

int main(int ac, char** av)
{
  char *ip;
  int iip;
  char *iface;
  int sock;
  struct sockaddr_ll sll;
  struct ifreq ifr;
  struct in_addr addr;
  unsigned char *pack;
  unsigned char recvpacket[2048];

  memset(&sll, 0, sizeof(sll));
  memset(&ifr, 0, sizeof(ifr));
  
  if (3 > ac)
    {
      printf("Nombre d'argument insufisant\n./getmac <iface> <ip>\n");
      return (-1);
    }
  iface = av[1];
  ip = av[2];
  if (0 >= (iip = inet_aton(ip, &addr)))
    {
      perror("inet_aton");
      return (-1);
    }
  printf("iip = %d\n", iip);
  if (-1 == (sock = InitSocket()))
    return (-1);
  if (-1 == CheckIfr(sock, &ifr, iface))
    return (-1);
  if (-1 == BindSocket(sock, &sll, &ifr))
    return (-1);
  pack = CreatePacket(&ifr, addr.s_addr, sock);

  if (NULL == pack)
    {
      printf("Erreur lors de la création du packet.\n");
    }
  else
    {
      printf("Envoi du paquet arp.\n");
      printData(pack, sizeof(struct ethhdr)+sizeof(struct arphdr)+20);
      send(sock, pack, sizeof(struct ethhdr)+sizeof(struct arphdr)+20, 0);
      printf("Reception du paquet arp.\n");
      while (1)
	{
	  recv(sock, recvpacket, 2048, 0);
	  if (0 == IsArpAnswer(recvpacket))
	    AnalysePacket(recvpacket);
	}
    }
  close(sock);
  return (0);
}
