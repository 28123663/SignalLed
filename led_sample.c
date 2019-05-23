/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 * 2018-11-19     flybreak     add stm32f429-fire-challenger bsp
 */

#include <rtthread.h>
#include <rtdevice.h>
#include "signal_led.h"

/* defined the LED pin */
#define LED0_PIN    GET_PIN(H, 10)
#define LED1_PIN    GET_PIN(H, 11)

static rt_timer_t led_timer;

//�����źŵƶ���
led led0;
led led1;

/*  �����źŵ�һ�������ڵ���˸ģʽ
 *  ��ʽΪ �������������������� ���������� ���Ȳ���
 *  ע�⣺  �����õ�λΪ���룬�ұ������ ��LED_TICK_TIME�� �꣬��Ϊ����������Ϊ������������ȡ������
 *          ������Ӣ�Ķ���Ϊ���������Ӣ�Ķ��Ž�β���ַ�����ֻ���������ּ����ţ������������ַ�����
 */
char *ledBlinkMode0 = "200,200,200,200,200,1000";
char *ledBlinkMode1 = "0,200";
char *ledBlinkMode2 = "500,500,500";
char *ledBlinkMode3 = "500,500";

//���忪�ƺ���
void led0_switch_on(void)
{
    rt_pin_write(LED0_PIN, PIN_LOW);
}
void led1_switch_on(void)
{
    rt_pin_write(LED1_PIN, PIN_LOW);
}

//����صƺ���
void led0_switch_off(void)
{
    rt_pin_write(LED0_PIN, PIN_HIGH);
}
void led1_switch_off(void)
{
    rt_pin_write(LED1_PIN, PIN_HIGH);
}


static void led_timeout(void *parameter)
{
    led_ticks();
}

int rt_led_timer_init(void)
{
    //��ʼ���źŵƶ���
    led_init(&led0, LED0_PIN, led0_switch_on, led0_switch_off);
    led_init(&led1, LED1_PIN, led1_switch_on, led1_switch_off);
    
    //�����źŵƹ���ģʽ��ѭ��ʮ��
    led_set_mode(&led0, LOOP_PERMANENT, ledBlinkMode0);
    led_set_mode(&led1, LOOP_PERMANENT, ledBlinkMode3);
    
    //�����źŵ�
    led_start(&led0);
    led_start(&led1);
    
    /* ������ʱ��1  ���ڶ�ʱ�� */    
#ifdef RT_USING_TIMER_SOFT
    led_timer = rt_timer_create("led_timer", led_timeout,
                                RT_NULL, LED_TICK_TIME,
                                RT_TIMER_FLAG_PERIODIC|RT_TIMER_FLAG_SOFT_TIMER);
#else
    led_timer = rt_timer_create("led_timer", led_timeout,
                                RT_NULL, LED_TICK_TIME,
                                RT_TIMER_FLAG_PERIODIC);
#endif	
    /* ������ʱ��1 */
    if (led_timer != RT_NULL)
        rt_timer_start(led_timer);
    
//    /* start software timer thread */    
//    rt_thread_t tid = RT_NULL;
//    /* �����߳�1 */
//    tid = rt_thread_create("led_thread",
//                            rt_thread_led_entry, 
//                            RT_NULL,
//                            512,
//                            RT_THREAD_PRIORITY_MAX/2,
//                            100);
//    if (tid != RT_NULL)
//        rt_thread_startup(tid);
}
INIT_APP_EXPORT(rt_led_timer_init);

