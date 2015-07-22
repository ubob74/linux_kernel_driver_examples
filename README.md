Linux kernel driver examples.

1. waitqueue

The driver creates misc device. Test application opens the device
and tries to readi a data. Driver function puts the process to
waitqueue and starts timer. Timer elapsed in 2 seconds and wakes up
the process.

2. workqueue

The driver creates a list and allocates and puts to the list several
entries. The number of entries in the list can be specified
as a module parameter.
Function test_wq_submit puts each entry from the list to the workqueue
and each work will be delayed in specified jiffies.

3. platform

The driver creates platform device with attribute "id".
User can read and modify the attribute:

 $ cat /sys/devices/platform/plat_test_driver/id
 $ echo 3 > /sys/devices/platform/plat_test_driver/id
