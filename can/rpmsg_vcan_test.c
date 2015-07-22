#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/rpmsg.h>
#include <linux/errno.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>

#include <linux/can/dev.h>
#include <linux/can/error.h>

struct vcan_priv {
	struct can_priv can;
	struct net_device *ndev;
	spinlock_t vcan_lock;
	struct rpmsg_channel *rpdev;
};

static netdev_tx_t vcan_xmit(struct sk_buff *skb, struct net_device *ndev)
{
	struct vcan_priv *priv = netdev_priv(ndev);
	struct net_device_stats *stats = &ndev->stats;
	struct can_frame *cf = (struct can_frame *)skb->data;
	struct rpmsg_channel *rpdev = priv->rpdev;
	struct device *dev = &rpdev->dev;

	if (can_dropped_invalid_skb(ndev, skb))
		return NETDEV_TX_OK;

	pr_info("%s: %s %d\n", __func__, cf->data, cf->can_dlc);

	/*netif_stop_queue(ndev);*/

	if (rpmsg_send(rpdev, cf->data, cf->can_dlc))
		dev_err(dev, "error sending respond\n");

	stats->tx_bytes += cf->can_dlc;
	stats->tx_packets++;

	return NETDEV_TX_OK;
}

static int vcan_rx(struct net_device *ndev, void *data, int len)
{
	struct net_device_stats *stats = &ndev->stats;
	struct sk_buff *skb;
	struct can_frame *cf;

	pr_info("%s\n", __func__);

	skb = alloc_can_skb(ndev, &cf);
	if (!skb)
		return -ENOMEM;

	cf->can_id = 0x123;
	cf->can_dlc = len;
	memcpy(&cf->data[0], (u8 *)data, len);

	netif_rx_ni(skb);

	stats->rx_bytes += cf->can_dlc;
	stats->rx_packets++;

	return 0;
}

static int vcan_open(struct net_device *ndev)
{
	pr_info("%s\n", __func__);
	netif_start_queue(ndev);
	return 0;
}

static int vcan_close(struct net_device *ndev)
{
	pr_info("%s\n", __func__);
	netif_stop_queue(ndev);
	return 0;
}

static const struct net_device_ops vcan_netdev_ops = {
	.ndo_open = vcan_open,
	.ndo_stop = vcan_close,
	.ndo_start_xmit = vcan_xmit,
};

static int vcan_init(struct rpmsg_channel *rpdev)
{
	struct device *dev = &rpdev->dev;
	struct net_device *ndev;
	struct vcan_priv *priv;
	int err;

	ndev = alloc_candev(sizeof(struct vcan_priv), 1);
	if (!ndev) {
		dev_err(dev, "alloc_candev failed\n");
		err = -ENOMEM;
		goto init_exit;
	}

	priv = netdev_priv(ndev);
	priv->ndev = ndev;
	priv->rpdev = rpdev;

	spin_lock_init(&priv->vcan_lock);

	ndev->flags = IFF_ECHO | IFF_NOARP;
	dev_set_drvdata(dev, ndev);
	SET_NETDEV_DEV(ndev, dev);
	ndev->netdev_ops = &vcan_netdev_ops;

	err = register_candev(ndev);
	if (err) {
		dev_err(dev, "register_candev failed\n");
		goto init_exit;
	}

	return 0;

init_exit:
	return err;
}

static void rpmsg_cb(struct rpmsg_channel *rpdev, void *data, int len,
			void *cb_priv, u32 src)
{
	struct device *dev = &rpdev->dev;
	struct net_device *ndev = dev_get_drvdata(dev);

	dev_info(dev, "message '%s' from %u and len=%d\n", (char *)data, src, len);

	netif_wake_queue(ndev);
	vcan_rx(ndev, data, len);
}

static int rpmsg_probe(struct rpmsg_channel *rpdev)
{
	return vcan_init(rpdev);
}

static void __devexit rpmsg_remove(struct rpmsg_channel *rpdev)
{
	struct device *dev = &rpdev->dev;
	struct net_device *ndev = dev_get_drvdata(dev);

	dev_info(dev, "%s\n", __func__);

	unregister_candev(ndev);
	free_candev(ndev);
	dev_set_drvdata(dev, NULL);
}

static struct rpmsg_device_id rpmsg_id_table[] = {
	{ .name = "fuckit-all", },
	{ },
};
MODULE_DEVICE_TABLE(rpmsg, rpmsg_id_table);

static struct rpmsg_driver rpmsg_driver = {
	.drv.name = KBUILD_MODNAME,
	.drv.owner = THIS_MODULE,
	.id_table = rpmsg_id_table,
	.probe = rpmsg_probe,
	.callback = rpmsg_cb,
	.remove = __devexit_p(rpmsg_remove),
};

static int __init init(void)
{
	return register_rpmsg_driver(&rpmsg_driver);
}
module_init(init);

static void __exit fini(void)
{
	unregister_rpmsg_driver(&rpmsg_driver);
}
module_exit(fini);

MODULE_LICENSE("GPL v2");

