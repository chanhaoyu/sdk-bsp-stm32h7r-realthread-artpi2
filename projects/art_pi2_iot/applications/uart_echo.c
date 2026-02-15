#include <rtthread.h>
#include <rtdevice.h>
#include "drv_common.h"

#define LED_PIN GET_PIN(O, 5)

static rt_device_t uart_dev = RT_NULL;
static rt_uint8_t s_buf[] = "uart1: artpi 2\r\n";
static rt_uint8_t r_buf[256];

static rt_sem_t uart_rx_sem = RT_NULL;

static rt_err_t uart_rx_int_cb(rt_device_t dev, rt_size_t size)
{
    rt_sem_release(uart_rx_sem);
}

void uart_echo_entry(void *parameter)
{
    RT_UNUSED(parameter);
    rt_uint32_t read_len = 0;
    
    rt_pin_mode(LED_PIN, PIN_MODE_OUTPUT);
    rt_pin_write(LED_PIN, PIN_HIGH);

    uart_dev = rt_device_find("uart1");
    if (uart_dev) 
    {
        rt_kprintf("uart1 find!\r\n");
        rt_device_open(uart_dev, RT_DEVICE_FLAG_TX_NON_BLOCKING | RT_DEVICE_OFLAG_RDWR);
        uart_rx_sem = rt_sem_create("uart_rx_sem", 0, RT_IPC_FLAG_FIFO);
        rt_device_set_rx_indicate(uart_dev, uart_rx_int_cb);
        rt_device_write(uart_dev, 0, s_buf, rt_strlen(s_buf));
    }
        
    while(1)
    {
        rt_sem_take(uart_rx_sem, RT_WAITING_FOREVER);
        rt_pin_write(LED_PIN, !rt_pin_read(LED_PIN));
        read_len = rt_device_read(uart_dev, 0, r_buf, 256);
        rt_device_write(uart_dev, 0, r_buf, read_len);
    }
}