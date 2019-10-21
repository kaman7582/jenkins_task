#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/udp.h>
#include <linux/ip.h>
#include<linux/inet.h>

#define SKB_SIZE  256
#define UDP_PORT 1234
#define L2_SIZE     sizeof(struct ethhdr)
#define L3_SIZE     sizeof(struct iphdr)
#define UDP_SIZE  sizeof(struct udphdr)
#define HEAD_SIZE  (L2_SIZE+L3_SIZE+UDP_SIZE)
//ETH_HDR_LEN + IP_HDR_LEN
#define TAIL_SIZE 32

#if 0
struct net_device * dev = NULL;
struct sk_buff * skb = NULL;
int nret = 1;
struct ethhdr * ethdr = NULL;
u_char * pdata = NULL;
int16_t tmp = 0;

printk("in dev_xmit_tpcm\n");

dev = dev_get_by_name(&init_net, eth);


#endif


int get_dev_info(char * src_mac)
{
	struct net_device * dev = NULL;
	struct in_device * in_dev;
	dev = dev_get_by_name(&init_net, "eth0");
	if(!dev)
		return -1;
	memcpy(src_mac,dev->dev_addr,6);
	//in_dev=in_dev_get(dev);
	//*src_ip = in_dev->ifa_list->ifa_address;
	//neigh_lookup
	
}


static int __init send_my_pkt(void)
{
	struct iphdr *iph;
	struct sk_buff  *skb;
	struct ethhdr *ethh;
	struct udphdr *uh;
	//D0-7E-35-0C-91-2B
	char dst_m[6]={0xd0,0x7e,0x35,0x0c,0x91,0x2b};
	int data_len = 0;
	int ip_total_len;
	printk("send_pkt_init\n");
	skb = alloc_skb(SKB_SIZE, GFP_KERNEL);
	if (!skb)
		return -ENOMEM;
	/* Reserve head-room for the headers */
	skb_reserve(skb, HEAD_SIZE);
	
	data_len = SKB_SIZE - (HEAD_SIZE + TAIL_SIZE);
	
	memset(skb_put(skb, data_len), 0xf, data_len);
	
	uh = (struct udphdr *)skb_push(skb, UDP_SIZE);
	skb_set_transport_header(skb, 0);
	uh->source = uh->dest = htons(UDP_PORT);
	uh->len = htons(data_len);
	uh->check = 0;
	
	ip_total_len = L3_SIZE + UDP_SIZE + data_len;
	iph = (struct iphdr*)skb_push(skb,L3_SIZE);
	skb_set_network_header(skb, 0);
	iph->ihl = L3_SIZE/4;
	iph->version = 4; 
	iph->tos = 0;
	iph->tot_len=htons(ip_total_len);
	iph->frag_off = 0;
	iph->ttl = 64; 
	iph->protocol = IPPROTO_UDP; 
	iph->check = 0;
	iph->saddr = in_aton("192.168.0.22");
	iph->daddr = in_aton("192.168.0.4");
	//--------------------
	ethh = (struct ethhdr*)skb_push(skb, sizeof (struct ethhdr));
	skb->protocol = ethh->h_proto = htons(ETH_P_IP);
	skb->no_fcs = 1;
	get_dev_info(ethh->h_source);
	//D0-7E-35-0C-91-2B
	//memcpy(ethh->h_dest,dst_m,6);
	memset(ethh->h_dest,0xff,6);
	skb->pkt_type = PACKET_OUTGOING;
	dev_queue_xmit(skb);
	//kfree_skb(skb);
	return 0;
}

static void __exit send_my_pkt_exit(void)
{
	printk("send_my_pkt_exit  terminating.\n");
}


module_init(send_my_pkt);
module_exit(send_my_pkt_exit);

MODULE_AUTHOR("Hu Ke <ke.hu@iopsys.eu>");
MODULE_LICENSE("GPL");


