#include <rtthread.h>
#include <wlan_mgnt.h>
#include <ntp.h>

#define DBG_TAG "ntp task"
#define DBG_LVL DBG_INFO
#include <rtdbg.h> 

#define NTP_SYNC_INTERVAL (2 * 60 * 1000)

rt_sem_t ntp_sem = RT_NULL;

extern int wifi_ready_register(void *handler);

static void ntp_on_wifi_ready(void)
{
    if (ntp_sem) {
        rt_sem_release(ntp_sem);
    }
}

static void ntp_sync(void)
{
    time_t cur_time = 0;
    struct timespec ts;

    cur_time = ntp_get_time(RT_NULL); /*after v4.0.3, RT-Thread takes over the timezone management*/

    if (!cur_time) {
        LOG_E("NTP get time failed");
        return;
    }
        
    ts.tv_sec = cur_time;
    ts.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &ts);

    LOG_I("System time set to %s", ctime(&cur_time));
}

static void ntp_thread_entry(void *param)
{
    wifi_ready_register(ntp_on_wifi_ready);
    ntp_sem = rt_sem_create("ntp_sem", 0, RT_IPC_FLAG_FIFO);

    while (1)
    {
        rt_sem_take(ntp_sem, RT_WAITING_FOREVER);
        ntp_sync();
        rt_thread_mdelay(NTP_SYNC_INTERVAL);
        ntp_on_wifi_ready();
    }
}

int ntp_task_init(void)
{
    rt_thread_t tid;

    tid = rt_thread_create("ntp",
                           ntp_thread_entry,
                           RT_NULL,
                           2048,
                           RT_THREAD_PRIORITY_MAX * 2 / 3,
                           20);
    if (tid == RT_NULL) {
        LOG_E("create ntp thread failed");
        return -RT_ENOMEM;
    }
    rt_thread_startup(tid);
    LOG_I("NTP task initialized");
    return RT_EOK;
}

INIT_APP_EXPORT(ntp_task_init);

