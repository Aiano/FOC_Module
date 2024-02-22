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
        .StdId = 0x601,
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

uint8_t CAN_txData[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
uint32_t CAN_txMailbox = CAN_TX_MAILBOX0;

void FOC_comm_init(){
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
    } else if (strcmp(seperated_str[0], "mode") == 0) // 设置模式
    {
        if (strcmp(seperated_str[1], "current") == 0) // 电流环
        {
            FOC_mode = FOC_MODE_CURRENT;
            FOC_target_current = 0;
//            CDC_printf("Mode: current.\n");
        } else if (strcmp(seperated_str[1], "velocity") == 0) // 速度环
        {
            FOC_mode = FOC_MODE_VELOCITY;
            FOC_target_velocity = 0;
//            CDC_printf("Mode: velocity\n");
        } else if (strcmp(seperated_str[1], "position") == 0) // 位置环
        {
            FOC_mode = FOC_MODE_POSITION;
            FOC_target_position = 0;
//            CDC_printf("Mode: position\n");
        }
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rxHeader;
    uint8_t rxData[8];

    if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rxHeader, rxData) == HAL_OK)
    {
//        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);

        memcpy(CAN_txData, rxData, sizeof(CAN_txData));

        if(HAL_CAN_AddTxMessage(&hcan1, &CAN_txHeader, CAN_txData, &CAN_txMailbox)){
            Error_Handler();
        }
    }
}

/**
 * @brief 定时器中断回调函数
 * @details 产生1kHz的时基，用于发送CAN消息
 * @param htim
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM4){
        if(HAL_CAN_AddTxMessage(&hcan1, &CAN_txHeader, CAN_txData, &CAN_txMailbox)){
            Error_Handler();
        }


    }
}