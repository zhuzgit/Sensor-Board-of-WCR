# Sensor-Board-of-WCR
1. init
2. sensor data to status
3. recevie CMD and send status to control board
4. 灯显示状态


## 接收并响应命令字
CMD is AA AB AC AD 问询ABCD四个足端状态，低四位为地址，只有地址正确才应答

## 返回状态
1. 非接触：FA FB FC FD （）
2. 加载（接触）：EA EB EC ED （Enable）
3. 卸载（接触）：DA DB DC DD （Disable）
4. 未知状态：AA AB AC AD （need Ask）
  
注意：
1. 不返数，不在此程序中处理，NUC收到状态：BA BB BC BD （Bad）
2. 不考虑GAP

## 工作灯
闪动证明工作
<img width="1106" height="1076" alt="image" src="https://github.com/user-attachments/assets/c6f8fcf7-07a8-4570-abbc-a0b098e99043" />

## 非接触状态
<img width="1074" height="754" alt="image" src="https://github.com/user-attachments/assets/9c5b12dc-9853-4ecb-a763-604a29d4b6cb" />

## 接触且处于加载状态
<img width="736" height="630" alt="image" src="https://github.com/user-attachments/assets/885c3f39-f12a-4431-b884-68d1d40bb4ba" />

## 接触且处于卸载状态

<img width="1168" height="1166" alt="image" src="https://github.com/user-attachments/assets/1e5cb1ca-42d2-4079-b3aa-229a1a2d7e3f" />



