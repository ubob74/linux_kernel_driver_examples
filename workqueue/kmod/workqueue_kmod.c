#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/workqueue.h>
#include <asm/uaccess.h>

/* default work delay in seconds */
#define DEF_DELAY 5

static int nr_work = 4;
module_param(nr_work, int, 0);

static DEFINE_MUTEX(test_wq_mutex);
static LIST_HEAD(test_wq_list);

static struct workqueue_struct *workq;

struct test_wq {
	int id;
	unsigned int delay;
	struct list_head link;
	struct delayed_work dwork;
};

static void foo(int id)
{
	pr_info("%s is running, id=%d", __func__, id);
}

static void test_wq_worker(struct work_struct *work_req)
{
	struct delayed_work *dwork = to_delayed_work(work_req);
	struct test_wq *twq = container_of(dwork, struct test_wq, dwork);

	foo(twq->id);
}

static struct test_wq *test_wq_alloc(void)
{
	struct test_wq *twq;

	twq = kzalloc(sizeof(struct test_wq), GFP_KERNEL);
	if (!twq)
		return NULL;

	return twq;
}

static struct test_wq *test_wq_setup(int id, unsigned int delay)
{
	struct test_wq *twq;

	twq = test_wq_alloc();
	if (!twq)
		return NULL;

	twq->id = id;
	twq->delay = delay;

	INIT_DELAYED_WORK(&twq->dwork, test_wq_worker);

	mutex_lock(&test_wq_mutex);
	list_add_tail(&twq->link, &test_wq_list);
	mutex_unlock(&test_wq_mutex);

	return twq;
}

static int test_wq_submit(void)
{
	struct test_wq *twq = NULL;

	mutex_lock(&test_wq_mutex);
	list_for_each_entry(twq, &test_wq_list, link)
		queue_delayed_work(workq, &twq->dwork,
				msecs_to_jiffies(twq->delay * MSEC_PER_SEC));
	mutex_unlock(&test_wq_mutex);

	return 0;
}

static void test_wq_release(void)
{
	struct test_wq *twq, *tmp;

	pr_info("%s: enter\n", __func__);

	mutex_lock(&test_wq_mutex);
	list_for_each_entry_safe(twq, tmp, &test_wq_list, link) {
		list_del_init(&twq->link);
		cancel_delayed_work_sync(&twq->dwork);
		pr_info("%s: work struct #%d is about to be freed\n",
					__func__, twq->id);
		kfree(twq);
	}
	mutex_unlock(&test_wq_mutex);
}

static int __init test_wq_init(void)
{
	int i;
	unsigned int delay = DEF_DELAY;

	workq = create_singlethread_workqueue("wqd");
	if (!workq)
		return -ENOMEM;

	pr_info("%s: enter\n", __func__);

	for (i = 0; i < nr_work; i++)
		test_wq_setup(i, delay * (i + 1));

	test_wq_submit();

	return 0;
}

static void __exit test_wq_exit(void)
{
	test_wq_release();
	flush_workqueue(workq);
	destroy_workqueue(workq);
}

module_init(test_wq_init);
module_exit(test_wq_exit);

MODULE_LICENSE("GPL");
