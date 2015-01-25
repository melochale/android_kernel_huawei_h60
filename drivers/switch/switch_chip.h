

#ifndef __DRIVERS_SWITCH_SMART_CHIP_H__
#define __DRIVERS_SWITCH_SMART_CHIP_H__

/******************************************************************************
* Register addresses
******************************************************************************/
#define FSA9685_REG_DEVICE_ID                 1
#define FSA9685_REG_CONTROL                   2
#define FSA9685_REG_INTERRUPT                 3
#define FSA9685_REG_INTERRUPT_MASK            4
#define FSA9685_REG_ADC                       5
#define FSA9685_REG_TIMING_SET_1              6
#define FSA9685_REG_DETACH_CONTROL            7
#define FSA9685_REG_DEVICE_TYPE_1             8
#define FSA9685_REG_DEVICE_TYPE_2             9
#define FSA9685_REG_DEVICE_TYPE_3            10
#define FSA9685_REG_MANUAL_SW_1              11
#define FSA9685_REG_MANUAL_SW_2              12
#define FSA9685_REG_TIMING_SET_2             13
#define FSA9685_REG_DCD                      0x1f

/* Register FSA9685_REG_CONTROL (02) */
#define FSA9685_INT_MASK             (1<<0)
#define FSA9685_WAIT                 (1<<1)
#define FSA9685_MANUAL_SW            (1<<2)
#define FSA9685_RAW_DATA             (1<<3)
#define FSA9685_SWITCH_OPEN          (1<<4)

/* Register FSA9685_REG_INTERRUPT (03) */
#define FSA9685_ATTACH                 (1<<0)
#define FSA9685_DETACH                 (1<<1)
#define FSA9685_VBUS_CHANGE            (1<<5)
#define FSA9685_RESERVED_ATTACH        (1<<6)
#define FSA9685_ADC_CHANGE             (1<<7)

/* Register FSA9685_REG_DEVICE_TYPE_1 (08)*/
#define FSA9685_FC_USB_DETECTED      (1<<0)
#define FSA9685_FC_RF_DETECTED       (1<<1)
#define FSA9685_USB_DETECTED         (1<<2)
#define FSA9685_UART_DETECTED        (1<<3)
#define FSA9685_MHL_DETECTED         (1<<4)
#define FSA9685_CDP_DETECTED         (1<<5)
#define FSA9685_DCP_DETECTED         (1<<6)
#define FSA9685_USBOTG_DETECTED      (1<<7)

/* Register FSA9685_REG_DEVICE_TYPE_2 (09)*/
#define FSA9685_JIG_UART             (1<<2)
/* Register FSA9685_REG_DEVICE_TYPE_3 (0A)*/
#define FSA9685_CUSTOMER_ACCESSORY5      (1<<5)
#define FSA9685_CUSTOMER_ACCESSORY6      (1<<6)
#define FSA9685_CUSTOMER_ACCESSORY7      (1<<7)
#define FSA9685_CUSTOMER_ACCESSORY_UNAVAILABLE  (0x1e)


/*USB state*/
#define FSA9685_OPEN        0
#define FSA9685_USB1        1
#define FSA9685_USB2        2
#define FSA9685_UART        3
#define FSA9685_MHL         4

/*Register FSA9685_REG_MANUAL_SW_1 value in different USB state*/
#define REG_VAL_FSA9685_OPEN    0
#define REG_VAL_FSA9685_USB1    (2<<0) | (1<<2) | (1<<5)
#define REG_VAL_FSA9685_USB2    (2<<0) | (2<<2) | (2<<5)
#define REG_VAL_FSA9685_UART    (2<<0) | (3<<2) | (3<<5)
#define REG_VAL_FSA9685_MHL     (3<<0) | (4<<2) | (4<<5)

enum err_oprt_reg_num
{
    ERR_FSA9685_REG_MANUAL_SW_1 = 1,
    ERR_FSA9685_READ_REG_CONTROL = 2,
    ERR_FSA9685_WRITE_REG_CONTROL = 3,
};

enum err_oprt_irq_num
{
    ERR_REQUEST_THREADED_IRQ = 50,
    ERR_GPIO_DIRECTION_INPUT = 51,
    ERR_GPIO_REQUEST = 52,
    ERR_GPIO_TO_IRQ = 53,
    ERR_OF_GET_NAME_GPIO = 54,
    ERR_SWITCH_USB_DEV_REGISTER = 55,
    ERR_NO_DEV = 56,
};

extern int get_swstate_value(void);
extern void switch_usb2_access_through_ap(void);
extern void switch_usb_set_state_to_mhl(void);
extern void usb_custom_acc5_event(int pedestal_attach);

#endif /* __DRIVERS_SWITCH_SMART_CHIP_H__ */
