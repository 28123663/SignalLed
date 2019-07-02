#ifndef __LED_H_
#define __LED_H_

#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "drv_gpio.h"

#define LED_TICK_TIME 50        //�����������õ�ʱ��������λ:ms��
#define LOOP_PERMANENT 0XFF     //����ѭ��

#define HANDLE_EXIST 1

#define LED_OFF     0           //����״̬
#define LED_ON      1           //����״̬


typedef struct led{
    uint16_t    tickCnt;
    uint8_t     blinkPoint;     //��˸�ڵ�
    uint8_t     activeState;    //�źŵ���ʱ������״̬ (PIN_HIGH/PIN_LOW)
    uint8_t     state;          //�źŵƵ�ǰ��״̬
    uint8_t     modePointer;    //����ָ����ǰ������˸�����еĵڼ�����Ա
    uint8_t     modeCnt;        //һ�������ڵ���˸����
    uint8_t     handleCnt;      //handle��������
    uint8_t     loop;           //����
    uint8_t     loopTemp;       //�洢���ڵ���ʱ������������ledʱ�ᱻ����
    char*       blinkMode;      //�����������������𡭡�����������ע��ʱ��֮���ԡ�,���������������ԡ�,����β��
    uint16_t*   blinkArr;
    void        (*switch_on)(void);
    void        (*switch_off)(void);
    struct led* next;
}led;


void led_ticks (void);
void led_init(led *handle, rt_base_t pin_index, void (*switch_on)(void), void (*switch_off)(void));
void led_set_mode(led* handle,uint8_t loop,char* blinkMode);
uint8_t led_start(led* led_handle);
void led_stop(struct led *led_handle);


#endif
