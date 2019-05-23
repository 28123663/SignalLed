/*
 * @File:   signal_led.c
 * @Author: WKJay
 * @Data:   2019/03/14 20:54
 * 
 * @e-mail: 1931048074@qq.com
 * 
 */

#include "signal_led.h"


static led *first_handle = NULL;
static void led_get_blinkArr(led *handle);


/*@brief   ��ʼ���źŵƶ���
 *@param   handle:     �źŵƾ��
 *@param   pin_index:  �źŵ����ź�
 *@param   switch_on:  ���ƺ������û��Զ��壩
 *@param   switch_off: �صƺ������û��Զ��壩
 */
void led_init(led *handle, rt_base_t pin_index, void (*switch_on)(void), void (*switch_off)(void))
{
    switch_off();
    rt_pin_mode(pin_index, PIN_MODE_OUTPUT);
    memset(handle, 0, sizeof(led));

    handle->switch_on = switch_on;
    handle->switch_off = switch_off;
}

/*@brief    �����źŵƵĹ���ģʽ
 *@param    loop:         ѭ������ (0xff ����ѭ��)
 *@param    blinkMode:    һ�����ڵ���˸��ʽ(�ַ�����ʽ,�磺"200,200,200,2000,"��ʾ��200ms,��200ms,��200ms,��2000ms)
 *@note     blinkMode�ַ���������Ӣ�Ķ��ŷָ����������Ӣ�Ķ��Ž�β
 */
void led_set_mode(led *handle, uint8_t loop, char *blinkMode)
{
    handle->loop = loop;
    handle->blinkMode = blinkMode;
    handle->handleCnt = 0;
    handle->blinkPoint = 0;
    handle->modePointer = 0;
    led_get_blinkArr(handle);
}


/*@brief    ����źŵ�һ����������˸ʱ������飨���ڲ����ã��û������ע��
 *@param    handle:    �źŵƾ��
 * */
static void led_get_blinkArr(led *handle)
{
    char *blinkModeTemp = NULL;
    uint8_t blinkCnt = 0;
    uint8_t blinkCntNum = 0;

    if (handle->blinkArr)
    {
        free(handle->blinkArr);
    }
    
    //��ȡ���鳤��
    for (blinkModeTemp = handle->blinkMode; *blinkModeTemp != '\0'; blinkModeTemp++)
    {
        if (*blinkModeTemp == ',')
        {
            handle->modeCnt++;
        }
    }
    if (*(blinkModeTemp-1) != ',')
        handle->modeCnt++;

    //��������
    handle->blinkArr = malloc(sizeof(uint16_t) * handle->modeCnt);

    //�洢����
    for (blinkModeTemp = handle->blinkMode; *blinkModeTemp != '\0'; blinkModeTemp++)
    {
        handle->blinkArr[blinkCntNum] = atol(blinkModeTemp);
        //���������������ֵ�������źŵƶ�ʱ����ʱʱ�䣩
        handle->blinkArr[blinkCntNum] /= LED_TICK_TIME;
        blinkCntNum++;
        while(*blinkModeTemp != ',')
        {
            if(*blinkModeTemp == '\0')
            {
                blinkModeTemp--;
                break;
            }
            blinkModeTemp++;
        }
    }
}

/*@brief    �źŵ�״̬��ת
 *@param    handle:    �źŵƾ��
 * */
void led_switch(led *led_handle)
{
    if (led_handle->modePointer%2)
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

/*@brief    �źŵ���������
 *@param    crt_handle:    ��ǰ�źŵƾ��
 * */
void led_handle(led *crt_handle)
{
    if (crt_handle->loopTemp)
    {
        crt_handle->handleCnt++;
__repeat:
        if ((crt_handle->handleCnt - 1) == crt_handle->blinkPoint)
        {
            if (crt_handle->modePointer < crt_handle->modeCnt)
            {
                crt_handle->blinkPoint += crt_handle->blinkArr[crt_handle->modePointer];
                if(crt_handle->blinkPoint == 0)	//ʱ��Ϊ0��ֱ������
                {
                    crt_handle->modePointer++;
                    goto __repeat;
                }
                else led_switch(crt_handle);
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
    else
        led_stop(crt_handle);
}

/*@brief    �źŵƿ�������û�е��ôκ��������źŵ����źŵƲ��Ṥ����
 *@param    led_handle:     Ҫ�������źŵƾ��
 *@return   0��             ����
 *          HANDLE_EXIST��  �����ͻ
 * */
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

/*@brief    �źŵƹرգ��ر��źŵƺ��Կɵ��ÿ�������������
 *@param    led_handle:    Ҫ�رյ��źŵƾ��
 * */
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

/*@brief    �źŵ���������
 *@note     ����ѭ�����øú����������źŵƽ����Ṥ����
 *          ���Խ�������̻߳�ʱ���У���֤ÿ��LED_TICK_TIME������ü���
 *          LED_TICK_TIME����"signal_led.h"�ж���ĺ꣬�źŵƵĹ������ڸú�
 *          ��֤�ú��������ʱ��Ϊ�źŵ������������õ����ڣ�
 * */
void led_ticks(void)
{
    led *current_handle;

    for (current_handle = first_handle; current_handle != NULL; current_handle = current_handle->next)
    {
        led_handle(current_handle);
    }
}
