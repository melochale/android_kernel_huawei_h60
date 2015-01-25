

#include "bsp_icc.h"
#include "osl_common.h"
#include "uart.h"
#include "gpio.h"


#define VIA_WAKEUP_BALONG_PIN	GPIO_26_3
#define LPm3_UART5_IQR_ENABLE	(0xaa)
#define VIA_WAKEUP_BALONG		(0x01)
#define dm_print_info(fmt, ...)    (printk(""fmt"\n", ##__VA_ARGS__))

/*****************************************************************************
 �� �� ��  : recv_modem_rsg_by_icc
 ��������  : modem����icc��ap�����gpio���ѶԷ�modem
 �������  : 
 �������  : ��
 �� �� ֵ  : void
*****************************************************************************/
int recv_modem_rsg_by_icc(u32 chan_id, u32 len, void* context)
{	
	s32 read_size = 0;
	u8 flag = 0;
	context = context;		//��ֹ����澯

 	read_size = bsp_icc_read((ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART, &flag, len); 
	if ((read_size > (s32)len) && (read_size <= 0))
    {
		return 1;
    }
	if(flag == LPm3_UART5_IQR_ENABLE)
	{
#ifdef UART5_IRQ_NOTIFY_MODEM
		uart5_init();
#endif
		bsp_icc_send((u32)ICC_CPU_MODEM,(ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,&flag,sizeof(flag));
	}
	return 0;
}
/*****************************************************************************
 �� �� ��  : via_wakeup_balong_handler
 ��������  : gpio�жϴ�����������icc��modem
 �������  : 
 �������  : 
 �� �� ֵ  : void
*****************************************************************************/
void via_wakeup_balong_handler(u32 gpio_no)
{
	u8 flag = 0;

	flag = VIA_WAKEUP_BALONG;
	bsp_icc_send((u32)ICC_CPU_MODEM,(ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,&flag,sizeof(flag));
}
/*****************************************************************************
 �� �� ��  : wakeup_modem_init
 ��������  : ap��gpio��ʼ��
 �������  : 
 �������  : 
 �� �� ֵ  : 0:�ɹ���-1:ʧ��
*****************************************************************************/
int wakeup_modem_init(void)
{
	int ret = 0;
	
	gpio_set_direction(VIA_WAKEUP_BALONG_PIN,0);
	ret = gpio_irq_request(VIA_WAKEUP_BALONG_PIN,via_wakeup_balong_handler,IRQ_TYPE_EDGE_FALLING|IRQF_AWAKE);
	if(ret < 0)
	{
		printk("irq fail\n");
		goto err_irq_req;
	}
	
	/* ע��ICC��д�ص� */
    if(0 !=bsp_icc_event_register((ICC_CHN_MCORE_CCORE << 16)|MCORE_CCORE_FUNC_UART,recv_modem_rsg_by_icc , NULL, NULL, NULL))
    {
        printk("reg icc cb fail\n");
    }
return 0;
		
err_irq_req:	
	gpio_free_irq(VIA_WAKEUP_BALONG_PIN);
	
return -1;
}

