# Sensor-Board-of-WCR

1. init
2. sensor data to status
3. recevie CMD and send status to control board

CMD is AA AB AC AD 问询ABCD四个足端状态


状态定义
1. 非接触：FA FB FC FD （）
2. 加载（接触）：EA EB EC ED （Enable）
3. 卸载（接触）：DA DB DC DD （Disable）
4. 未知状态：AA AB AC AD （need Ask）
  
注意：
1. 不返数，不在此程序中处理，状态：BA BB BC BD （Bad）
2. 不考虑GAP
