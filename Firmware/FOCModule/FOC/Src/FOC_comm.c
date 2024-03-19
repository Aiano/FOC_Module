/**
 * @file FOC_comm.c
 * @brief FOC通信
 * @details
 * @author Mo
 * @email Aiano_czm@163.com
 */

#include "FOC_comm.h"
#include "can.h"
#include "tim.h"
#include "string.h"
#include "usbd_cdc_if.h"
#include "FOC_setting.h"
#include "FOC_utility.h"

#define SEPERATED_STR_NUM 5
#define BUF_LEN 10

// USB CDC
char seperated_str[SEPERATED_STR_NUM][BUF_LEN];

// CAN
CAN_TxHeaderTypeDef CAN_txHeader = {
        .RTR = CAN_RTR_DATA, // Remote Transmission Request
        .IDE = CAN_ID_STD, // Identifier Extension
        .StdId = 0x201,
        .DLC = 8, // Data Length Code (Byte)
        .TransmitGlobalTime = DISABLE
};

// 不配置滤波器无法接收数据
// 以下是可以通过任意ID的滤波器
CAN_FilterTypeDef CAN_filterConfig = {
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDMASK,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .FilterIdHigh = 0x0000,
        .FilterIdLow = 0x0000,
        .FilterMaskIdHigh = 0x0000,
        .FilterMaskIdLow = 0x0000,
        .FilterFIFOAssignment = CAN_RX_FIFO0,
        .FilterActivation = ENABLE,
        .SlaveStartFilterBank = 14
};

uint8_t  CAN_txData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t CAN_txMailbox = CAN_TX_MAILBOX0;

void FOC_comm_init() {
    HAL_TIM_Base_Start_IT(&htim4);

    HAL_CAN_ConfigFilter(&hcan1, &CAN_filterConfig);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void FOC_comm_parse_command(uint8_t *buf, uint32_t *len) {
    static uint16_t seperated_str_index; // 字符串分割遍历变量
    static uint16_t seperated_str_num; // 字符串分段数
    static char     *str_ptr; // strtok()返回指针

    /* 字符串分割 */
    seperated_str_index = 0;

    str_ptr = strtok((char *) buf, ",\r\n");
    strcpy(seperated_str[seperated_str_index++], str_ptr);

    while (str_ptr != NULL && seperated_str_index < SEPERATED_STR_NUM) {
        str_ptr = strtok(NULL, ",\r\n");
        strcpy(seperated_str[seperated_str_index++], str_ptr);
    }

    seperated_str_num = seperated_str_index - 1;

    /* 协议解析 */
    if (strcmp(seperated_str[0], "position") == 0) // 设置目标角度
    {
        char  *end;
        float temp;

        temp = strtof(seperated_str[1], &end);
        if (seperated_str[1] != end && temp >= 0 && temp <= 2 * _PI) // 解析成功
        {
            FOC_target_position = temp;
//            CDC_printf("Target position: %.2f\n", FOC_target_position);
        } else { // 解析失败
//            CDC_printf("Invalid input: position must be within [0 - 2Pi] and float type.\n");
        }
    } else if (strcmp(seperated_str[0], "velocity") == 0) { // 设置目标速度
        char  *end;
        float temp;

        temp = strtof(seperated_str[1], &end);
        if (seperated_str[1] != end &&
            _checkRange(temp, FOC_target_velocity_upper_limit, FOC_target_velocity_lower_limit)) // 解析成功
        {
            FOC_target_velocity = temp;
//            CDC_printf("Target velocity: %.2f\n", FOC_target_velocity);
        } else { // 解析失败
//            CDC_printf("Invalid input: velocity must be within [%.2f - %.2f] and float type.\n",
//                       FOC_target_velocity_lower_limit, FOC_target_velocity_upper_limit);
        }
    } else if (strcmp(seperated_str[0], "current") == 0) { // 设置目标电流
        char  *end;
        float temp;

        temp = strtof(seperated_str[1], &end);
        if (seperated_str[1] != end &&
            _checkRange(temp, FOC_target_current_upper_limit, FOC_target_current_lower_limit)) // 解析成功
        {
            FOC_target_current = temp;
//            CDC_printf("Target current: %.2f\n", FOC_target_current);
        } else { // 解析失败
//            CDC_printf("Invalid input: current must be within [%.2f - %.2f] and float type.\n",
//                       FOC_target_current_lower_limit, FOC_target_current_upper_limit);
        }
    } else if (strcmp(seperated_str[0], "voltage") == 0) { // 设置目标电压
        char  *end;
        float temp;

        temp = strtof(seperated_str[1], &end);
        if (seperated_str[1] != end &&
            _checkRange(temp, FOC_target_voltage_upper_limit, FOC_target_voltage_lower_limit)) // 解析成功
        {
            FOC_target_voltage = temp;

        } else { // 解析失败

        }
    } else if (strcmp(seperated_str[0], "mode") == 0) // 设置模式
    {
        if (strcmp(seperated_str[1], "current") == 0) // 电流环
        {
            FOC_mode           = FOC_MODE_CURRENT;
            FOC_target_current = 0;
//            CDC_printf("Mode: current.\n");
        } else if (strcmp(seperated_str[1], "velocity") == 0) // 速度环
        {
            FOC_mode            = FOC_MODE_VELOCITY;
            FOC_target_velocity = 0;
//            CDC_printf("Mode: velocity\n");
        } else if (strcmp(seperated_str[1], "position") == 0) // 位置环
        {
            FOC_mode            = FOC_MODE_POSITION;
            FOC_target_position = 0;
//            CDC_printf("Mode: position\n");
        } else if (strcmp(seperated_str[1], "voltage") == 0) // 电压环
        {
            FOC_mode           = FOC_MODE_VOLTAGE;
            FOC_target_voltage = 0;
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    static CAN_RxHeaderTypeDef rxHeader;
    static uint8_t             rxData[8];
    static int16_t             voltage, current, speed, angle;
    static float               target_voltage, target_current, target_speed, target_angle;

    if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK &&
        rxHeader.DLC == 8 && // 数据长度为8字节
        rxHeader.IDE == 0 && // 标准帧
        rxHeader.RTR == 0) { // 数据帧
        switch (rxHeader.StdId) {
            case 0x1FF: // 电压控制 1~4号驱动器 -25000 ~ 25000 对应 -48V ~ 48V
                if (_checkRange(FOC_CAN_driver_ID, 4, 1)) {
                    voltage        = (int16_t) ((rxData[FOC_CAN_driver_ID * 2 - 2] << 8) |
                                                rxData[FOC_CAN_driver_ID * 2 - 1]);
                    target_voltage = (float) voltage * 48 / 25000;
                    if (_checkRange(target_voltage, FOC_target_voltage_upper_limit,
                                    FOC_target_voltage_lower_limit)) { // 设置的电压不允许超过供电电压
                        FOC_mode           = FOC_MODE_VOLTAGE;
                        FOC_target_voltage = target_voltage;
                    }
                }
                break;
            case 0x1FE: // 电流控制 1~4号驱动器 -10000 ~ 10000 对应 -10A ~ 10A
                if (_checkRange(FOC_CAN_driver_ID, 4, 1)) {
                    current        = (int16_t) ((rxData[FOC_CAN_driver_ID * 2 - 2] << 8) |
                                                rxData[FOC_CAN_driver_ID * 2 - 1]);
                    target_current = (float) current / 1000;
                    if (_checkRange(target_current, FOC_target_current_upper_limit, FOC_target_current_lower_limit)) {
                        FOC_mode           = FOC_MODE_CURRENT;
                        FOC_target_current = target_current;
                    }
                }
                break;
            case 0x1FD: // 速度控制 1~4号驱动器 转速单位 rpm
                if (_checkRange(FOC_CAN_driver_ID, 4, 1)) {
                    speed        = (int16_t) ((rxData[FOC_CAN_driver_ID * 2 - 2] << 8) |
                                              rxData[FOC_CAN_driver_ID * 2 - 1]);
                    target_speed = (float) speed * _2PI / 60;
                    if (_checkRange(target_speed, FOC_target_velocity_upper_limit, FOC_target_velocity_lower_limit)) {
                        FOC_mode           = FOC_MODE_VELOCITY;
                        FOC_target_velocity = target_speed;
                    }
                }
                break;
            case 0x1FC: // 位置控制 1~4号驱动器 机械角度范围 0 ~ 4095
                if (_checkRange(FOC_CAN_driver_ID, 4, 1)) {
                    angle        = (int16_t) ((rxData[FOC_CAN_driver_ID * 2 - 2] << 8) |
                                              rxData[FOC_CAN_driver_ID * 2 - 1]);
                    target_angle = (float) angle * _2PI / 4096;
                    if (_checkRange(target_angle, _2PI, 0)) {
                        FOC_mode           = FOC_MODE_POSITION;
                        FOC_target_position = target_angle;
                    }
                }
                break;
            default:
                break;
        }
    }
}

/**
 * @brief CAN反馈电机报文
 */
void CAN_send_message() {
    static uint16_t angle;
    static int16_t  speed;
    static int16_t  current;
    static int8_t   temperature;
    static int8_t   voltage;

    /* 报文 */

    // 机械角度范围 0 ~ 4095
    // DATA[0] 机械角度高8位
    // DATA[1] 机械角度低8位
    angle = (uint16_t) (FOC_mechanical_angle * 4096 / _2PI);
    CAN_txData[0] = (angle >> 8) & 0xFF;
    CAN_txData[1] = angle & 0xFF;

    // 转速单位 rpm
    // DATA[2] 转速高8位
    // DATA[3] 转速低8位
    speed = (int16_t) (FOC_velocity * 60 / _2PI);
    CAN_txData[2] = (speed >> 8) & 0xFF;
    CAN_txData[3] = speed & 0xFF;

    // 转矩电流 -10000 ~ 10000 对应 -10A ~ 10A
    // DATA[4] 转矩电流高8位
    // DATA[5] 转矩电流低8位
    current = (int16_t) (Iq * 1000);
    CAN_txData[4] = (current >> 8) & 0xFF;
    CAN_txData[5] = current & 0xFF;

    // PCB温度
    temperature = (int8_t) FOC_PCB_temp;
    CAN_txData[6] = temperature & 0xFF;

    // 输入电压
    CAN_txData[7] = 0x00;

    /* ID */
    CAN_txHeader.StdId = 0x200 + FOC_CAN_driver_ID;

    if (HAL_CAN_AddTxMessage(&hcan1, &CAN_txHeader, CAN_txData, &CAN_txMailbox)) {
        Error_Handler();
    }
}

/**
 * @brief 定时器中断回调函数
 * @details 产生1kHz的时基，用于发送CAN消息
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM4) {
        CAN_send_message();
    }
}