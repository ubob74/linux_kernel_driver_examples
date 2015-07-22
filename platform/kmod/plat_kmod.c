#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>

#define DRVNAME "plat_test_driver"

static struct platform_device *pdev;

struct platform_data {
	int id;
	struct device_attribute id_attr;
};

static ssize_t show_id(struct device *dev,
			struct device_attribute *attr, char *buf)
{
	struct platform_data *pdata = dev_get_drvdata(dev);
	return sprintf(buf, "%d\n", pdata->id);
}

static ssize_t store_id(struct device *dev,
			struct device_attribute *attr, const char *buf, size_t count)
{
	struct platform_data *pdata = dev_get_drvdata(dev);
	sscanf(buf, "%du", &pdata->id);
	return count;
}

static int create_name_attr(struct platform_data *pdata,
				struct device *dev)
{
	sysfs_attr_init(&pdata->name_attr.attr);
	pdata->id_attr.attr.name = "id";
	pdata->id_attr.attr.mode = S_IRUGO | S_IWUGO;
	pdata->id_attr.show = show_id;
	pdata->id_attr.store = store_id;
	return device_create_file(dev, &pdata->id_attr);
}

static int plat_test_probe(struct platform_device *pdev)
{
	int err = 0;
	struct platform_data *pdata;

	pr_info("%s: enter\n", __func__);

	pdata = devm_kzalloc(&pdev->dev, sizeof(struct platform_data),
					GFP_KERNEL);
	if (!pdata)
		return -ENOMEM;

	err = create_name_attr(pdata, &pdev->dev);
	if (err)
		goto exit_free;

	pdata->id = 0;

	platform_set_drvdata(pdev, pdata);

	return 0;

exit_free:
	return err;
}

static int plat_test_remove(struct platform_device *pdev)
{
	struct platform_data *pdata = platform_get_drvdata(pdev);

	pr_info("%s: enter, id=%d\n", __func__, pdata->id);

	device_remove_file(&pdev->dev, &pdata->id_attr);
	platform_set_drvdata(pdev, NULL);

	return 0;
}

static int plat_test_device_add(void)
{
	int err;

	pdev = platform_device_alloc(DRVNAME, -1);
	if (!pdev) {
		err = -ENOMEM;
		pr_err("Device allocation failed\n");
		goto exit;
	}

	err = platform_device_add(pdev);
	if (err) {
		pr_err("Device addition failed (%d)\n", err);
		goto exit_device_put;
	}

	return 0;

exit_device_put:
	platform_device_put(pdev);
exit:
	return err;	
}

static struct platform_driver plat_test_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = DRVNAME,
	},
	.probe = plat_test_probe,
	.remove = plat_test_remove,
};

static int __init plat_test_init(void)
{
	int err = 0;

	err = platform_driver_register(&plat_test_driver);
	if (err)
		goto exit;

	err = plat_test_device_add();
	if (err)
		goto exit_driver_unreg;

	return 0;

exit_driver_unreg:
	platform_driver_unregister(&plat_test_driver);
exit:
	return err;
}

static void __exit plat_test_exit(void)
{
	platform_device_unregister(pdev);
	platform_driver_unregister(&plat_test_driver);
}

module_init(plat_test_init);
module_exit(plat_test_exit);

MODULE_LICENSE("GPL");
