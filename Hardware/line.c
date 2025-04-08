#include "line.h"
#define WINDOW_SIZE 1


float MAX_SPEED_DIFF = 8;

volatile int Line_Arr[8] = {0, 0, 0, 0, 0, 0, 0, 0};      // 当前读数存储
volatile int Last_Line_Arr[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 上一时刻读数存储
volatile int Filtered_Arr[8] = {0, 0, 0, 0, 0, 0, 0, 0}; // 滤波后的值存储
int Temp_Arr[8][WINDOW_SIZE]; // 循环缓冲区，每个传感器一个
int Temp_index = 0;           // 缓冲区索引
int Num_A = 0;
volatile  uint8_t last_sum = 0;
volatile  uint8_t flag_sum = 1;

// 读取传感器并更新缓冲区
void Updata_TempArr() {
  for (int i = 0; i < 8; i++) {
    // Line_Arr[i] = (pinState >> i) & 0x01; // 更新当前读数
    Temp_Arr[i][Temp_index] = Line_Arr[i]; // 更新缓冲区
  }
  Temp_index = (Temp_index + 1) % WINDOW_SIZE; // 更新缓冲区索引
}

// 计算多数决定并更新滤波值
void filterValues() {
  for (int i = 0; i < 8; i++) {
    int sum = 0;
    for (int j = 0; j < WINDOW_SIZE; j++) {
      sum += Temp_Arr[i][j];
    }
    // 多数决定：如果1的数量多于总数的一半，结果为1，否则为0
    Filtered_Arr[i] = (sum > (WINDOW_SIZE / 2)) ? 1 : 0;
  }
}

// 主循环
uint8_t read_lines(void) {
  volatile uint8_t sum = 0;

  Line_Arr[0] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_1_PORT, GPIO_LINE_PIN_Line_1_PIN) > 0;
  Line_Arr[1] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_2_PORT, GPIO_LINE_PIN_Line_2_PIN) > 0;
  Line_Arr[2] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_3_PORT, GPIO_LINE_PIN_Line_3_PIN) > 0;
  Line_Arr[3] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_4_PORT, GPIO_LINE_PIN_Line_4_PIN) > 0;
  Line_Arr[4] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_5_PORT, GPIO_LINE_PIN_Line_5_PIN) > 0;
  Line_Arr[5] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_6_PORT, GPIO_LINE_PIN_Line_6_PIN) > 0;
  Line_Arr[6] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_7_PORT, GPIO_LINE_PIN_Line_7_PIN) > 0;
  Line_Arr[7] =
      DL_GPIO_readPins(GPIO_LINE_PIN_Line_8_PORT, GPIO_LINE_PIN_Line_8_PIN) > 0;

  for (int i = 0; i < 8; i++) {
    // last_sum+=Last_Line_Arr[i];
    sum += Line_Arr[i];
    // Last_Line_Arr[i]=Line_Arr[i];
  }

 if (last_sum >= 1 && sum == 0 || last_sum == 0 && sum >= 1) 
 {
 flag_sum+=1;
 DL_GPIO_clearPins(LED_BUZZER_PORT, LED_BUZZER_PIN_0_PIN);
 DL_GPIO_clearPins(LED_BUZZER_PORT, LED_BUZZER_PIN_1_PIN);
 delay_ms(30); 
 DL_GPIO_setPins(LED_BUZZER_PORT, LED_BUZZER_PIN_0_PIN);
 DL_GPIO_setPins(LED_BUZZER_PORT, LED_BUZZER_PIN_1_PIN);
 }

  last_sum = sum;
  return flag_sum;
  // Updata_TempArr();
  // filterValues();
}

int calculate_speed_difference(volatile int sensor_values[]) {
  int position = 0;
  int weight_sum = 0;
  int active_sensors = 0;

  // 计算权重和活动传感器数量
  for (int i = 0; i < 8; i++) {
    if (sensor_values[i] == 1) {
      if (i < 4) {
        weight_sum += (i - 3);
      } else {
        weight_sum += (i - 4);
      }
      active_sensors++;
    }
  }

  // 计算相对位置
  if (active_sensors != 0) {
    position = weight_sum / active_sensors;
  } else {
    // 如果没有检测到黑线，可以根据需要返回特殊值
    position = 0;
  }

  // 根据相对位置计算速度差值
  int speed_difference = position * MAX_SPEED_DIFF;
  return speed_difference;
}

void Reset_Line(void)
{
    last_sum = 0;
    flag_sum = 1;

}