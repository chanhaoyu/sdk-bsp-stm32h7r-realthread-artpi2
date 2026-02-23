#include <rtthread.h>
#include <wlan_mgnt.h>
#include <wlan_prot.h>
#include <wlan_cfg.h>

#define WLAN_SSID       "886"
#define WLAN_PASSWORD   "12345678"

rt_sem_t wifi_recon_sem = RT_NULL;

#define MAX_HANDLERS 10
typedef void (*wifi_ready_handler_t)(void);
static wifi_ready_handler_t handlers[MAX_HANDLERS];
static int handler_cnt = 0;

int wifi_ready_register(wifi_ready_handler_t handler)
{
    if (handler_cnt >= MAX_HANDLERS) {
        return -RT_ENOMEM;
    }
    handlers[handler_cnt++] = handler;
    return RT_EOK;
}

static void wifi_ready_callback(int event, struct rt_wlan_buff *buff, void *parameter)
{
    for (int i = 0; i < handler_cnt; i++) {
        if (handlers[i]) handlers[i]();
    }
}

static void wifi_connect_callback(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_kprintf("[EVENT] === WiFi CONNECTED ===\n");
}

static void wifi_connect_fail_callback(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_kprintf("[EVENT] WiFi CONNECT FAILED\n");
#ifndef RT_WLAN_AUTO_CONNECT_ENABLE
    if (wifi_recon_sem)
        rt_sem_release(wifi_recon_sem);
#endif
}

static void wifi_disconnect_callback(int event, struct rt_wlan_buff *buff, void *parameter)
{
    rt_kprintf("[EVENT] WiFi DISCONNECTED\n");
#ifndef RT_WLAN_AUTO_CONNECT_ENABLE
    if (wifi_recon_sem)
        rt_sem_release(wifi_recon_sem);
#endif
}

static void wifi_connect_thread(void *parameter)
{
    wifi_recon_sem = rt_sem_create("wifi_recon_sem", 0, RT_IPC_FLAG_FIFO);

    if (!wifi_recon_sem) {
        rt_kprintf("connect sem create failed\n");
        RT_ASSERT(0);
    } else {
        rt_sem_release(wifi_recon_sem);
    }

    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED, wifi_connect_callback, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_CONNECTED_FAIL, wifi_connect_fail_callback, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_STA_DISCONNECTED, wifi_disconnect_callback, RT_NULL);
    rt_wlan_register_event_handler(RT_WLAN_EVT_READY, wifi_ready_callback, RT_NULL);

    rt_sem_take(wifi_recon_sem, RT_WAITING_FOREVER);

    while(1) {
        rt_kprintf("Starting to connect to AP: %s ...\n", WLAN_SSID);
        if (RT_EOK != rt_wlan_connect(WLAN_SSID, WLAN_PASSWORD)) {
            rt_sem_release(wifi_recon_sem);
        }
        rt_sem_take(wifi_recon_sem, RT_WAITING_FOREVER);
        rt_thread_mdelay(10 * 1000);
    }
}

int wifi_connect_task_init(void)
{
    rt_thread_t tid = rt_thread_create("wifi_connect",
                                      wifi_connect_thread,
                                      RT_NULL,
                                      4096,
                                      RT_THREAD_PRIORITY_MAX / 3,
                                      20);
    if (tid)
        rt_thread_startup(tid);
    return 0;
}
INIT_APP_EXPORT(wifi_connect_task_init);
