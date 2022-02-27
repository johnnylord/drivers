#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>


/*
 * Reference:
 *  - https://elixir.bootlin.com/linux/v5.4/source/include/linux/netfilter.h#L79
 */
static struct nf_hook_ops *nfhoIN = NULL;

/*
 * [> Responses from hook functions. <]
 * #define NF_DROP 0
 * #define NF_ACCEPT 1
 * #define NF_STOLEN 2
 * #define NF_QUEUE 3
 * #define NF_REPEAT 4
 * #define NF_STOP 5
 * #define NF_MAX_VERDICT NF_STOP
 */
static unsigned int
hfuncIN(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct net_device *ndIn = state->in;
    struct iphdr *iphdr = NULL;
    struct tcphdr *tcphdr = NULL;
    unsigned int action = NF_ACCEPT;
    unsigned int port = 0;

    /* Parse the packet & drop packets go to 80 port */
    iphdr = ip_hdr(skb);
    if (iphdr->protocol == IPPROTO_TCP) {
        tcphdr = tcp_hdr(skb);
        port = ntohs(tcphdr->dest);
        if (port == 80) {
            action = NF_DROP;
            pr_info("Drop TCP packet target for port %d from interface %s\n", port, ndIn->name);
        } else {
            pr_info("TCP packet from interface %s which goes to port %d\n", ndIn->name, port);
        }
    }

    return action;
}

static int __init mymodule_init(void)
{
    /* Allocate space for netfilter hook initialization */
    nfhoIN = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
    if (nfhoIN == NULL) {
        goto fail;
    }

    /*
     * Initialize netfilter hook
     *  - pf => protocol family
     *  - hook => hook function
     *  - hooknum => which hookpoint should the hook function should attach to
     *  - priority => priority of the hook function
     */
    nfhoIN->pf = PF_INET;                   // IPv4
    nfhoIN->hook = (nf_hookfn*)hfuncIN;     // hook function
    nfhoIN->hooknum = NF_INET_LOCAL_IN;     // received packets
    nfhoIN->priority = NF_IP_PRI_FIRST;     // max hook priority

    /* Register the netfilter hook on specific network namespace */
    nf_register_net_hook(&init_net, nfhoIN);
    return 0;

fail:
    return -1;
}

static void __exit mymodule_exit(void)
{
    nf_unregister_net_hook(&init_net, nfhoIN);
    kfree(nfhoIN);
}

module_init(mymodule_init);
module_exit(mymodule_exit);
