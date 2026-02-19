#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"
#include "mpu6xxx.h"

#ifndef MPU6XXX_DEVICE_NAME
#define MPU6XXX_DEVICE_NAME "i2c1"
#endif

void mpu_monitor_entry(void *parameter)
{
    struct mpu6xxx_device *dev;
    struct mpu6xxx_3axes accel, gyro;
    int i;

    /* Initialize mpu6xxx, The parameter is RT_NULL, means auto probing for i2c*/
    dev = mpu6xxx_init(MPU6XXX_DEVICE_NAME, RT_NULL);

    if (dev == RT_NULL)
    {
        rt_kprintf("mpu6xxx init failed\n");
        return;
    }
    rt_kprintf("mpu6xxx init succeed\n");

    while(1)
    {
        mpu6xxx_get_accel(dev, &accel);
        mpu6xxx_get_gyro(dev, &gyro);

        rt_kprintf("accel.x = %3d, accel.y = %3d, accel.z = %3d ", accel.x, accel.y, accel.z);
        rt_kprintf("gyro.x = %3d gyro.y = %3d, gyro.z = %3d\n", gyro.x, gyro.y, gyro.z);

        rt_thread_mdelay(500);
    }

    mpu6xxx_deinit(dev);

    return;
}

static int mpu_monitor_init()
{
    rt_thread_t tid;

#ifdef RT_USING_HEAP
    tid = rt_thread_create("mpu_monitor", mpu_monitor_entry, RT_NULL,
                           1024, 20, 20);
    RT_ASSERT(tid != RT_NULL);
#endif /* RT_USING_HEAP */
    rt_thread_startup(tid);
}

//INIT_APP_EXPORT(mpu_monitor_init);
MSH_CMD_EXPORT(mpu_monitor_init, mpu monitor thread init);