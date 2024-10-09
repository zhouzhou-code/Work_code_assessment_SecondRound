#include "djMotor.h"

#include "can.h"
/*
* @brief : 通过CAN驱动Dj电机
* @param1: 通过哪个CAN发送控制电机的报文
* @param2: 电机ID 参数可以是下面的几个 @arg MotorId_t
* @param3: 电机转动的角度，范围在-360 ~ +360
*/

void DriveDjMotor(CAN_HandleTypeDef* hcan,MotorId_t motorId,double angle)
{   
	  
	  if(angle>360) angle=360;
	  if(angle<-360) angle=-360;
	  
    int16_t motorVoltage = (int16_t)(angle / 360.0 * 25000);  // 根据角度计算电压值 (-25000 ~ 25000)

    // 获取高 8 位和低 8 位
    uint8_t highByte = (motorVoltage >> 8) & 0xFF; // 右移 8 位并取高 8 位
    uint8_t lowByte = motorVoltage & 0xFF;         // 取低 8 位

    // 初始化 CAN 消息
    CanMessage_t canMessageTx = 
	  {
        .TxHeader = 
			  {
					.StdId = (motorId > 4) ? 0x2FF : 0x1FF,  //电机ID>4,则标识符ID为0x2FF，否则为0x1FF
            .IDE = CAN_ID_STD,        // 设置为标准 ID
            .RTR = CAN_RTR_DATA,      // 数据帧
            .DLC = 8                  // 数据长度
        },
        .TxDataBuf = {0, 0, 0, 0, 0, 0, 0, 0}  // 初始化数据缓冲区
    };
		
			 
    // 根据电机 ID 设置相应的电压值
    switch (motorId) 
		{
        case MOTOR_ID_1:
				case MOTOR_ID_5:
            canMessageTx.TxDataBuf[0] = highByte;
            canMessageTx.TxDataBuf[1] = lowByte;
            break;
				
        case MOTOR_ID_2:
				case MOTOR_ID_6:
            canMessageTx.TxDataBuf[2] = highByte;
            canMessageTx.TxDataBuf[3] = lowByte;
            break;
				
        case MOTOR_ID_3:
				case MOTOR_ID_7:
            canMessageTx.TxDataBuf[4] = highByte;
            canMessageTx.TxDataBuf[5] = lowByte;
            break;
				
        case MOTOR_ID_4:
            canMessageTx.TxDataBuf[6] = highByte;
            canMessageTx.TxDataBuf[7] = lowByte;
            break;
        default:
            return;  // 无效的电机ID
    }

    // 发送 CAN 消息
    SendCanMessage(hcan, &canMessageTx);
}

