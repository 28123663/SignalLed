#include "signal_led.h"
#include "stdlib.h"
#include "math.h"
#include "drv_gpio.h"

static led *first_handle = NULL;
static void led_get_blinkArr(led *handle);

//��ʼ���źŵƶ���
//handle:�źŵƾ��
//pin_index:�źŵ����ź�
//switch_on():���ƺ���
//switch_off():�صƺ���
void led_init(led *handle, rt_base_t pin_index, void (*switch_on)(void), void (*switch_off)(void))
{
    rt_pin_mode(pin_index, PIN_MODE_OUTPUT);
    memset(handle, 0, sizeof(led));

    handle->switch_on = switch_on;
    handle->switch_off = switch_off;
}

//�����źŵƵĹ���ģʽ
//loop:         ѭ������ (0xff ����ѭ��)
//blinkMode:    ��˸��ʽ
void led_set_mode(led *handle, uint8_t loop, char *blinkMode)
{
    handle->loop = loop;
    handle->blinkMode = blinkMode;
    led_get_blinkArr(handle);
}

//����źŵ�һ�������ڵ���˸ʱ������
static void led_get_blinkArr(led *handle)
{
    char *blinkModeTemp = NULL;
    uint8_t blinkCnt = 0;
    uint8_t blinkCntNum = 0;

    //��ȡ���鳤��
    for (blinkModeTemp = handle->blinkMode; *blinkModeTemp != '\0'; blinkModeTemp++)
    {
        if (*blinkModeTemp == ',')
        {
            if (*(blinkModeTemp + 1) != '\0')
                handle->modeCnt++;
        }
    }
    if (handle->modeCnt)
        handle->modeCnt++;

    //��������
    handle->blinkArr = malloc(sizeof(uint16_t) * handle->modeCnt);

    //�洢����
    for (blinkModeTemp = handle->blinkMode; *blinkModeTemp != '\0'; blinkModeTemp++)
    {
        if (*blinkModeTemp == ',')
        {
            for (; blinkCnt > 1; blinkCnt--)
            {
                handle->blinkArr[blinkCntNum] +=
                    (*(blinkModeTemp - blinkCnt) - '0') * pow(10, blinkCnt - 1);
            }

            //���������������ֵ�������źŵƶ�ʱ����ʱʱ�䣩
            handle->blinkArr[blinkCntNum] /= LED_TICK_TIME;

            blinkCntNum++;
        }
        else
        {
            blinkCnt++;
        }
    }
}

//�źŵ�״̬��ת
void led_switch(led *led_handle)
{
    if (led_handle->state)
    {
        led_handle->switch_off();
        led_handle->state = LED_OFF;
    }
    else
    {
        led_handle->switch_on();
        led_handle->state = LED_ON;
    }
}

//�źŵƹ�������
void led_handle(led *crt_handle)
{
    if (crt_handle->loopTemp)
    {
        crt_handle->handleCnt++;
        if ((crt_handle->handleCnt - 1) == crt_handle->blinkPoint)
        {
            if (crt_handle->modePointer < crt_handle->modeCnt)
            {
                led_switch(crt_handle);
                crt_handle->blinkPoint += crt_handle->blinkArr[crt_handle->modePointer];
                crt_handle->modePointer++;
            }
            else
            {
                crt_handle->modePointer = 0;
                crt_handle->blinkPoint = 0;
                crt_handle->handleCnt = 0;
                if (crt_handle->loopTemp != LOOP_PERMANENT)
                    crt_handle->loopTemp--;
            }
        }
    }
}

uint8_t led_start(led *led_handle)
{
    led_handle->loopTemp = led_handle->loop; //����ʱ��������

    led *temp_handle = first_handle;
    while (temp_handle)
    {
        if (temp_handle == led_handle)
            return HANDLE_EXIST;         //�Ѵ��ڣ����˳�
        temp_handle = temp_handle->next; //��������ǰָ��
    }

    led_handle->next = first_handle;
    first_handle = led_handle;

    return 0;
}

void led_stop(struct led *led_handle)
{
    struct led **handle_pointer;

    led_handle->switch_off(); //�رյ�

    for (handle_pointer = &first_handle; *handle_pointer;)
    {
        led *handle_destory = *handle_pointer;

        if (handle_destory == led_handle)
        {
            *handle_pointer = handle_destory->next;
        }
        else
        {
            handle_pointer = &handle_destory->next;
        }
    }
}

//�źŵƶ�ʱ����LED_TICK_TIME ms ����һ��
void led_ticks(void)
{
    led *current_handle;

    for (current_handle = first_handle; current_handle != NULL; current_handle = current_handle->next)
    {
        led_handle(current_handle);
    }
}
