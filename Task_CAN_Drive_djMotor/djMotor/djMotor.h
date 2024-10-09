#ifndef __DJMOTOR_H__
#define __DJMOTOR_H__

#include "can.h"

// 枚举类型定义电机ID
typedef enum MotorId
{
	  MOTOR_ID_1 = 1,  // 电机1
    MOTOR_ID_2 = 2,  // 电机2
    MOTOR_ID_3 = 3,  // 电机3
    MOTOR_ID_4 = 4,  // 电机4
    MOTOR_ID_5 = 5,  // 电机5
    MOTOR_ID_6 = 6,  // 电机6
    MOTOR_ID_7 = 7,  // 电机7
  
} MotorId_t;


void DriveDjMotor(CAN_HandleTypeDef* hcan,MotorId_t motorId,double angle);

#endif
