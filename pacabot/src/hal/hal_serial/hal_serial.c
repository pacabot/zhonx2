#include "config/config.h"
#include "config/basetypes.h"
#include "config/errors.h"

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_usart.h"
#include "misc.h"

/* The declarations for this module */
#include "hal/hal_serial.h"

#include "hal/hal_os.h"

#include <stdio.h>
#include <string.h>

#define HAL_SERIAL_MAX_HANDLES  2
#define USART1_POS              0
#define USART2_POS              1


#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
static void NVIC_Configuration(unsigned long irqNum);
#endif
static void GPIO_Configuration(GPIO_TypeDef *port, unsigned short rxPin, unsigned short txPin, unsigned short rxPin_src, unsigned short txPin_src);

typedef struct usart_handle_
{
    USART_InitTypeDef settings;
    USART_TypeDef *usart;
    bool isInitialized;
} usart_handle;

static usart_handle usart_conf[HAL_SERIAL_MAX_HANDLES];


int hal_serial_init(void)
{
    /* Clears the handles array */
    memset(usart_conf, 0, sizeof(usart_handle) * HAL_SERIAL_MAX_HANDLES);

    return HAL_SERIAL_E_SUCCESS;
}


int hal_serial_terminate(void)
{
    unsigned char i;

    for (i = 0; i < HAL_SERIAL_MAX_HANDLES; i++)
    {
        USART_DeInit(usart_conf[i].usart);
        USART_Cmd(usart_conf[i].usart, DISABLE);
    }
    /* Clears the handles array */
    memset(usart_conf, 0, sizeof(usart_handle) * HAL_SERIAL_MAX_HANDLES);

    return HAL_SERIAL_E_SUCCESS;
}


int hal_serial_open(HAL_SERIAL_HANDLE *handle, void *params, unsigned long baudrate)
{
    GPIO_TypeDef   *gpio;
    unsigned short rx_pin;
    unsigned short tx_pin;
    unsigned short rx_pin_src;
    unsigned short tx_pin_src;
    usart_handle   *h;
#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
    unsigned long  irqNum;
#endif
    unsigned char  *port = (unsigned char *)params;

    if (handle == null)
    {
        return HAL_SERIAL_E_BAD_HANDLE;
    }

    switch (*port)
    {
        case 0: // USART1
            gpio       = GPIOA;
            tx_pin     = GPIO_Pin_9;
            tx_pin_src = GPIO_PinSource9;
            rx_pin     = GPIO_Pin_10;
            rx_pin_src = GPIO_PinSource10;
            h          = &usart_conf[USART1_POS];
#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
            irqNum     = USART1_IRQn;
#endif
            h->usart   = USART1;

            if (true == h->isInitialized)
            {
                *handle = (HAL_SERIAL_HANDLE)h;
                return HAL_SERIAL_E_SUCCESS;
            }
            /* Enable USART1 and GPIOA clock */
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
            break;
        case 1: // USART2
            gpio       = GPIOB;
            tx_pin     = GPIO_Pin_2;
            tx_pin_src = GPIO_PinSource2;
            rx_pin     = GPIO_Pin_3;
            rx_pin_src = GPIO_PinSource3;
            h          = &usart_conf[USART2_POS];
#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
            irqNum     = USART2_IRQn;
#endif
            h->usart   = USART2;

            if (true == h->isInitialized)
            {
                *handle = (HAL_SERIAL_HANDLE)h;
                return HAL_SERIAL_E_SUCCESS;
            }
            /* Enable USART2 and GPIOA clock */
            RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
            RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
            break;
        default:
            return HAL_SERIAL_E_BAD_HANDLE;
    }

#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
    /* Configure Interrupts */
    USART_ITConfig(h->usart, USART_IT_RXNE, ENABLE);
    NVIC_Configuration(irqNum);
#endif

    /* Configure the GPIOs */
    GPIO_Configuration(gpio, rx_pin, tx_pin, rx_pin_src, tx_pin_src);

    h->isInitialized                      = true;
    h->settings.USART_WordLength          = USART_WordLength_8b;
    h->settings.USART_StopBits            = USART_StopBits_1;
    h->settings.USART_Parity              = USART_Parity_No;
    h->settings.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    h->settings.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    h->settings.USART_BaudRate            = baudrate;

    USART_Init(h->usart, &(h->settings));

    /* Enable USART port */
    USART_Cmd(h->usart, ENABLE);

    /* Wait until USART is correctly initialized */
    while (USART_GetFlagStatus(h->usart, USART_FLAG_IDLE));

    *handle = (HAL_SERIAL_HANDLE)h;

    return HAL_SERIAL_E_SUCCESS;
}


int hal_serial_close(HAL_SERIAL_HANDLE handle)
{
    UNUSED(handle);

    return HAL_SERIAL_E_SUCCESS;
}


int hal_serial_read(HAL_SERIAL_HANDLE handle, void *data,
                    unsigned int *nbytes, unsigned long timeoutms)
{
    usart_handle    *h = (usart_handle *)handle;
    unsigned int    nbReadBytes = 0;
    unsigned char   *pData = (unsigned char *)data;
    unsigned char   readByte;
    unsigned long   end = (hal_os_get_systicks() + timeoutms);

    if (handle == null)
    {
        return HAL_SERIAL_E_BAD_HANDLE;
    }

    while (nbReadBytes < *nbytes)
    {
        if (RESET != USART_GetFlagStatus(h->usart, USART_FLAG_RXNE))
        {
            readByte = USART_ReceiveData(h->usart);
            if (readByte != 0)
            {
                nbReadBytes++;
                *pData++ = readByte;
            }
        }
        else
        {
            if (timeoutms != FOREVER)
            {
                if (hal_os_get_systicks() >= end)
                {
                    break;
                }
            }
        }
    }
    *nbytes = nbReadBytes;

    return HAL_SERIAL_E_SUCCESS;
}


int hal_serial_write(HAL_SERIAL_HANDLE handle, void *data, unsigned int nbytes)
{
    usart_handle    *h = (usart_handle *)handle;
    unsigned char   *bytes = data;

    if (handle == null)
    {
        return HAL_SERIAL_E_BAD_HANDLE;
    }

    /* Loop while there are more characters to send. */
    while(nbytes--)
    {
        USART_SendData(h->usart, (uint16_t)*bytes++);
        /* Loop until the end of transmission */
        while(USART_GetFlagStatus(h->usart, USART_FLAG_TC) == RESET);
    }

    return HAL_SERIAL_E_SUCCESS;
}



/**
 * @brief  Configures the nested vectored interrupt controller.
 * @param [in] irqNum IRQ source to configure in the NVIC
 * @retval None
 */
#ifdef CONFIG_HAL_SERIAL_ENABLE_INTERRUPTS
static void NVIC_Configuration(unsigned long irqNum)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* Enable the USARTx Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = irqNum;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}
#endif


/**
 * @brief  Configures GPIO pins.
 * @param  None
 * @retval None
 */
static void GPIO_Configuration(GPIO_TypeDef *port, unsigned short rxPin, unsigned short txPin, unsigned short rxPin_src, unsigned short txPin_src)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /* Connect PXx to USARTx_Tx*/
    GPIO_PinAFConfig(port, txPin_src, GPIO_AF_USART1);
    GPIO_PinAFConfig(port, txPin_src, GPIO_AF_USART2);

    /* Connect PXx to USARTx_Rx*/
    GPIO_PinAFConfig(port, rxPin_src, GPIO_AF_USART1);
    GPIO_PinAFConfig(port, rxPin_src, GPIO_AF_USART2);

    /* Configure USART Tx pin as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = txPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(port, &GPIO_InitStructure);

    /* Configure USART Rx pin as input floating */
    GPIO_InitStructure.GPIO_Pin = rxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
    GPIO_Init(port, &GPIO_InitStructure);
}
