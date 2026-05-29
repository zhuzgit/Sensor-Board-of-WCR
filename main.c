// #pragma float64 // 启用双精度浮点数支持
#include <REG251.H>  //包含STC32G的头文件
#include <stdio.h>
#include "string.h"
#include "math.h"
#include "intrins.h"

typedef unsigned char u8;
typedef unsigned int u16;
typedef unsigned long u32;

typedef unsigned char uint8_t;
typedef unsigned int uint16_t;

#define xs 0

#define K1 P33
#define K2 P13
#define K3 P03
#define K4 P02
#define Baddr 0x0a
#define WHO_REG 0x75
#define skip 18  //sensor error 若有一路传感器故障，可以用此来屏蔽该传感器，无故障则设定为大于13的值
#define s11 0    //11 sensor set 1 兼容原有11路传感器板，11路为1，13路为0
// 全局变量
float pitch = 0.0, roll = 0.0, yaw = 0.0;
unsigned char ss;
/*
#define M_PI 3.14159265358979323846

// MPU6050 地址
#define MPU6050_ADDR_W 0xD0
#define MPU6050_ADDR_R 0xD1


// MPU6050寄存器
#define PWR_MGMT_1 0x6B
#define SMPLRT_DIV 0x19
#define CONFIG 0x1A
#define GYRO_CONFIG 0x1B
#define ACCEL_CONFIG 0x1C
#define ACCEL_XOUT_H 0x3B
#define GYRO_XOUT_H 0x43
#define WHO_AM_I 0x75  // 芯片ID寄存器





// FIFO缓冲区
unsigned char fifoBuffer[64];
long q[4];  // 四元数

// CAN 
#define TSG1 2  // 0~15
#define TSG2 1  // 1~7 (TSG2 不能设置为0)
#define BRP 3   // 0~63

#define SJW 0  // 重新同步跳跃宽度
#define SAM 0  // 总线电平采样次数： 0:采样1次; 1:采样3次

u16 CAN1_ID;
u8 RX1_BUF[8];
u8 TX1_BUF[8];

bit B_1ms;       // 1ms标志
bit B_Can1Read;  // CAN 收到数据标志
bit B_Can1Send;  // CAN 发生数据标志
u16 msecond;
u16 second;

// CAN函数
u8 CanReadReg(u8 addr) {
  u8 dat;
  CANAR = addr;
  dat = CANDR;
  return dat;
}
void CanWriteReg(u8 addr, u8 dat) {
  CANAR = addr;
  CANDR = dat;
}
void CanReadFifo(u8 *pdat) {
  pdat[0] = CanReadReg(RX_BUF0);
  pdat[1] = CanReadReg(RX_BUF1);
  pdat[2] = CanReadReg(RX_BUF2);
  pdat[3] = CanReadReg(RX_BUF3);

  pdat[4] = CanReadReg(RX_BUF0);
  pdat[5] = CanReadReg(RX_BUF1);
  pdat[6] = CanReadReg(RX_BUF2);
  pdat[7] = CanReadReg(RX_BUF3);

  pdat[8] = CanReadReg(RX_BUF0);
  pdat[9] = CanReadReg(RX_BUF1);
  pdat[10] = CanReadReg(RX_BUF2);
  pdat[11] = CanReadReg(RX_BUF3);

  pdat[12] = CanReadReg(RX_BUF0);
  pdat[13] = CanReadReg(RX_BUF1);
  pdat[14] = CanReadReg(RX_BUF2);
  pdat[15] = CanReadReg(RX_BUF3);
}
u16 CanReadMsg(u8 *pdat) {
  u8 i;
  u16 CanID;
  u8 buffer[16];

  CanReadFifo(buffer);
  CanID = ((buffer[1] << 8) + buffer[2]) >> 5;
  for (i = 0; i < 8; i++) {
    pdat[i] = buffer[i + 3];
  }
  return CanID;
}
void CanSendMsg(u16 canid, u8 *pdat) {
  u16 CanID;

  CanID = canid << 5;
  CanWriteReg(TX_BUF0, 0x08);
  CanWriteReg(TX_BUF1, (u8)(CanID >> 8));
  CanWriteReg(TX_BUF2, (u8)CanID);
  CanWriteReg(TX_BUF3, pdat[0]);

  CanWriteReg(TX_BUF0, pdat[1]);
  CanWriteReg(TX_BUF1, pdat[2]);
  CanWriteReg(TX_BUF2, pdat[3]);
  CanWriteReg(TX_BUF3, pdat[4]);

  CanWriteReg(TX_BUF0, pdat[5]);
  CanWriteReg(TX_BUF1, pdat[6]);
  CanWriteReg(TX_BUF2, pdat[7]);

  CanWriteReg(TX_BUF3, 0x00);
  CanWriteReg(CMR, 0x04);  // 发起一次帧传输
}
void CANSetBaudrate() {
  CanWriteReg(BTR0, (SJW << 6) + BRP);
  CanWriteReg(BTR1, (SAM << 7) + (TSG2 << 4) + TSG1);
}
void WaitCan1Send(u8 i) {
  while ((--i) && (B_Can1Send))
    ;
}

void CANBUS1_Interrupt(void) interrupt CAN1_VECTOR {
  u8 isr;
  u8 store;
  u8 arTemp;

  arTemp = CANAR;
  store = AUXR2;

  AUXR2 &= ~0x08;  // 选择CAN1模块
  isr = CanReadReg(ISR);
  CANAR = ISR;
  CANDR = isr;  // 写1清除标志位

  if ((isr & 0x04) == 0x04)  // TI
  {
    B_Can1Send = 0;
  }
  if ((isr & 0x08) == 0x08)  // RI
  {
    B_Can1Read = 1;
  }
  if ((isr & 0x40) == 0x40)  // ALI
  {
  }
  if ((isr & 0x20) == 0x20)  // EWI
  {
    CANAR = MR;
    CANDR &= ~0x04;  // 清除 Reset Mode, 从BUS-OFF状态退出
  }
  if ((isr & 0x10) == 0x10)  // EPI
  {
  }
  if ((isr & 0x02) == 0x02)  // BEI
  {
  }
  if ((isr & 0x01) == 0x01)  // DOI
  {
  }

  AUXR2 = store;
  CANAR = arTemp;
}

void CANInit() {
  CANEN = 1;
  CANSEL = 0;
  P_SW1 &= ~0x30;

  CanWriteReg(MR, 0x04);
  CANSetBaudrate();

  CanWriteReg(ACR0, 0x00);
  CanWriteReg(ACR1, 0x00);
  CanWriteReg(ACR2, 0x00);
  CanWriteReg(ACR3, 0x00);
  CanWriteReg(AMR0, 0xFF);
  CanWriteReg(AMR1, 0xFF);
  CanWriteReg(AMR2, 0xFF);
  CanWriteReg(AMR3, 0xFF);

  CanWriteReg(IMR, 0xff);
  CanWriteReg(ISR, 0xff);
  CanWriteReg(MR, 0x00);
  CANICR = 0x02;
}
*/
/*****************************************************************************/
//---------------- UART config ----------------//
#define MAIN_Fosc 24000000L  // 定义主时钟
#define Baudrate (65536 - MAIN_Fosc / 115200 / 4)
//#define UART1_BUF_LENGTH 16
bit B_TX1_Busy;
u8 TX1_Cnt;
u8 RX1_Cnt;
//u8 RX1_Buffer[UART1_BUF_LENGTH];



void Uart1_Init(void) {
  TR1 = 0;
  S1BRT = 0;
  T1_CT = 0;
  T1x12 = 1;
  TMOD &= ~0x30;
  TH1 = (u8)(Baudrate / 256);
  TL1 = (u8)(Baudrate % 256);
  ET1 = 0;
  TR1 = 1;
  SCON = (SCON & 0x3f) | 0x40;
  ES = 1;
  REN = 1;
  P_SW1 &= 0x3f;
}

void UART_SendChar(u8 c) {
  SBUF = c;
  while (!TI)
    ;
  TI = 0;
}

void PrintString(u8 *puts) {
  for (; *puts != 0; puts++) {
    SBUF = *puts;
    B_TX1_Busy = 1;
    while (B_TX1_Busy)
      ;
  }
}

//----------------  基本函数 ----------------//
void delay_ms(u8 ms) {
  u16 i;
  do {
    i = MAIN_Fosc / 6000;
    while (--i)
      ;
  } while (--ms);
}

void Delay500ms(void) {
  unsigned long edata i;
  _nop_();
  _nop_();
  i = 2999998UL;
  while (i)
    i--;
}

void IOSET(void) {
  P0M1 = 0x00;
  P0M0 = 0x00;
  P1M1 = 0x00;
  P1M0 = 0x00;
  P2M1 = 0x00;
  P2M0 = 0x00;
  P3M1 = 0x00;
  P3M0 = 0x00;
  P4M1 = 0x00;
  P4M0 = 0x00;
  P5M1 = 0x00;
  P5M0 = 0x00;
  P6M1 = 0x00;
  P6M0 = 0x00;
  P7M1 = 0x00;
  P7M0 = 0x00;

  P_SW1 &= ~0x30;

  P0M0 &= ~0x0c;
  P0M1 |= 0x0c;
  P3M0 &= ~0x08;
  P3M1 &= ~0x08;
  P1M0 &= ~0x08;
  P1M1 |= 0x08;

  P2 = 0xff;
  P34 = 1;
  P35 = 1;
  P36 = 1;
  P10 = 1;
  P11 = 1;
/*
P33 Key1
P13 Key2
P03 Key3
P04 Key4
*/
        P0M0 &= ~0x0c; P0M1 &= ~0x0c; //P02 P03
    P1M0 &= ~0x08; P1M1 &= ~0x08; //P13
    P3M0 &= ~0x08; P3M1 &= ~0x08; //P33

  P37 = 0;//RSTN
  delay_ms(10);
  P37 = 1;
}

void cs(char c) {
  P2 = 0xff;
  P34 = 1;
  P35 = 1;
  P36 = 1;
  P10 = 1;
  P11 = 1;

  if (c == 1) P20 = 0;
  if (c == 2) P21 = 0;
  if (c == 3) P22 = 0;
  if (c == 4) P23 = 0;
  if (c == 5) P24 = 0;
  if (c == 6) P25 = 0;
  if (c == 7) P26 = 0;
  if (c == 8) P27 = 0;
  if (c == 9) P34 = 0;
  if (c == 10) P35 = 0;
  if (c == 11) P36 = 0;
  if (c == 12) P10 = 0;
  if (c == 13) P11 = 0;
}

void Delay10ms(void) {
  unsigned long edata i;
  _nop_();
  _nop_();
  _nop_();
  i = 59998UL;
  while (i)
    i--;
}

void boardset(void) {
  EAXFR = 1;
  CKCON = 0x00;
  WTST = 0x00;
  XOSCCR = 0xc0;
  while (!(XOSCCR & 1))
    ;
  CLKDIV = 0x00;
  CLKSEL = 0x01;

  I2C_S1 = 0;
  I2C_S0 = 0;
  I2CCFG = 0xe0;
  I2CMSST = 0x00;
}

//---------------- I2C 基本函数 ----------------//
sbit SDA = P1 ^ 4;
sbit SCL = P1 ^ 5;
#define AK09911C_ADDR 0x0C
#define SLAW ((AK09911C_ADDR << 1) | 0)
#define SLAR ((AK09911C_ADDR << 1) | 1)

void Wait() {
  while (!(I2CMSST & 0x40))
    ;
  I2CMSST &= ~0x40;
}

void Start() {
  I2CMSCR = 0x01;
  Wait();
}

void SendData(char dat) {
  I2CTXD = dat;
  I2CMSCR = 0x02;
  Wait();
}

void RecvACK() {
  I2CMSCR = 0x03;
  Wait();
}

char RecvData() {
  I2CMSCR = 0x04;
  Wait();
  return I2CRXD;
}

void SendACK() {
  I2CMSST = 0x00;
  I2CMSCR = 0x05;
  Wait();
}

void SendNAK() {
  I2CMSST = 0x01;
  I2CMSCR = 0x05;
  Wait();
}

void Stop() {
  I2CMSCR = 0x06;
  Wait();
}

bit AckOK() {
  return (I2CMSST & 0x01) ? 0 : 1;
}

void WriteNbyte(u8 addr, u8 *p, u8 number) {
  Start();
  SendData(SLAW);
  RecvACK();
  SendData(addr);
  RecvACK();
  do {
    SendData(*p++);
    RecvACK();
  } while (--number);
  Stop();
}

void ReadNbyte(u8 addr, u8 *p, u8 number) {
  Start();
  SendData(SLAW);
  RecvACK();
  SendData(addr);
  RecvACK();
  Start();
  SendData(SLAR);
  RecvACK();
  do {
    *p = RecvData();
    p++;
    if (number != 1)
      SendACK();
  } while (--number);
  SendNAK();
  Stop();
}

// AK09911C 寄存器定义
#define AK09911C_WIA1 0x00
#define AK09911C_WIA2 0x01
#define AK09911C_ST1 0x10
#define AK09911C_HXL 0x11
#define AK09911C_HXH 0x12
#define AK09911C_HYL 0x13
#define AK09911C_HYH 0x14
#define AK09911C_HZL 0x15
#define AK09911C_HZH 0x16
#define AK09911C_ST2 0x18
#define AK09911C_CNTL1 0x30
#define AK09911C_CNTL2 0x31
#define AK09911C_CNTL3 0x32

#define REG_ASAX 0x60
#define REG_ASAY 0x61
#define REG_ASAZ 0x62

#define MODE_POWER_DOWN 0x00
#define MODE_SINGLE_MEASURE 0x01
#define MODE_CONT_MEASURE1 0x02  // 10Hz
#define MODE_CONT_MEASURE2 0x04  // 20Hz
#define MODE_CONT_MEASURE3 0x06  // 50Hz
#define MODE_CONT_MEASURE4 0x08  // 100Hz

//--------------------------------//
void AK09911C_Init(void) {
  unsigned char st1, st2;
  unsigned char mode = MODE_CONT_MEASURE3;  // 0x06 连续测量模式 3

  // 【修正点】：操作模式必须写入 CNTL2 寄存器，而不是 CNTL1
  WriteNbyte(AK09911C_CNTL2, &mode, 1);

  do {
    delay_ms(10);
    ReadNbyte(AK09911C_ST1, &st1, 1);
    ReadNbyte(AK09911C_ST2, &st2, 1);  // 必须读 ST2
  } while (!(st1 & 0x01));
}

//--------------------------------//
void AK09911C_ReadMag(char addr, int *x, int *y, int *z) {
  unsigned char buf[6];
  ReadNbyte(addr, buf, 6);

  // 【修正点】：强制转换为 short，保证 16 位补码转 int 时符号位正确扩展
  *x = (short)((buf[1] << 8) | buf[0]);
  *y = (short)((buf[3] << 8) | buf[2]);
  *z = (short)((buf[5] << 8) | buf[4]);
}

char putchar(char ch) {
  SBUF = ch;
  while (!TI)
    ;
  TI = 0;
  return ch;
}

union {
  float f;
  unsigned char c[4];
} u;

unsigned char WHO_AM_I_Debug(unsigned char addr_w, unsigned char addr_r) {
  unsigned char id;

  printf("\n=== WHO_AM_I Debug: addr_w=0x%02X addr_r=0x%02X ===\n", addr_w, addr_r);

  Start();
  printf("Start\n");
  SendData(addr_w);
  RecvACK();
  printf("Write dev addr ACK=%d\n", AckOK());

  if (!AckOK()) {
    Stop();
    printf("Stop (no ACK on write addr)\n");
    return 0xFF;
  }

  SendData(WHO_REG);
  RecvACK();
  printf("Write reg 0x75 ACK=%d\n", AckOK());
  if (!AckOK()) {
    Stop();
    printf("Stop (no ACK on reg addr)\n");
    return 0xFF;
  }

  Start();
  printf("Repeated Start\n");
  SendData(addr_r);
  RecvACK();
  printf("Read dev addr ACK=%d\n", AckOK());
  if (!AckOK()) {
    Stop();
    printf("Stop (no ACK on read addr)\n");
    return 0xFF;
  }

  id = RecvData();
  printf("Recv data = 0x%02X\n", id);
  SendNAK();
  printf("Send NAK\n");
  Stop();
  printf("Stop\n");

  return id;
}
/*
void MPU6050_Init() {
  Start();
  SendData(MPU6050_ADDR_W);
  RecvACK();
  SendData(0x6B);
  RecvACK();
  SendData(0x00);
  RecvACK();
  Stop();

  Start();
  SendData(MPU6050_ADDR_W);
  RecvACK();
  SendData(0x1A);
  RecvACK();
  SendData(0x01);
  RecvACK();
  Stop();

  Start();
  SendData(MPU6050_ADDR_W);
  RecvACK();
  SendData(0x1B);
  RecvACK();
  SendData(0x00);
  RecvACK();
  Stop();

  Start();
  SendData(MPU6050_ADDR_W);
  RecvACK();
  SendData(0x1C);
  RecvACK();
  SendData(0x00);
  RecvACK();
  Stop();
}

int MPU6050_ReadWord(unsigned char reg) {
  unsigned char H, L;
  Start();
  SendData(MPU6050_ADDR_W);
  RecvACK();
  SendData(reg);
  RecvACK();
  Start();
  SendData(MPU6050_ADDR_R);
  RecvACK();
  H = RecvData();
  SendACK();
  L = RecvData();
  SendNAK();
  Stop();
  return (H << 8) | L;
}

void MPU6050_ReadRawData(int *ax, int *ay, int *az, int *gx, int *gy, int *gz) {
  *ax = MPU6050_ReadWord(0x3B);
  *ay = MPU6050_ReadWord(0x3D);
  *az = MPU6050_ReadWord(0x3F);
  *gx = MPU6050_ReadWord(0x43);
  *gy = MPU6050_ReadWord(0x45);
  *gz = MPU6050_ReadWord(0x47);
}

#define ACC_SENS 16384.0

void ComputeEulerAngles_AccelOnly(int ax, int ay, int az,
                                  float *pitch, float *roll, float *yaw) {
  double axf = (double)ax / ACC_SENS;
  double ayf = (double)ay / ACC_SENS;
  double azf = (double)az / ACC_SENS;

  *pitch = (float)(atan2(ayf, sqrt(axf * axf + azf * azf)) * 180.0 / M_PI);
  *roll = (float)(atan2(-axf, azf) * 180.0 / M_PI);
  *yaw = 0.0f;
}
*/

void UART1_int(void) interrupt 4 {
  if (RI) {
    RI = 0;
if(SBUF==(0xa0|Baddr))UART_SendChar(ss|Baddr);		
 //   RX1_Buffer[RX1_Cnt] = SBUF;
 //  if (++RX1_Cnt >= UART1_BUF_LENGTH)
      RX1_Cnt = 0;
  }

  if (TI) {
    TI = 0;
    B_TX1_Busy = 0;
  }
}


void main(void) {
  //  u8 sr;
  //  u8 buffer[16];
  //  int ax, ay, az, gx, gy, gz;
	unsigned char s1,s2,s3,pz,pfx;
  float dt = 0.1;

  unsigned char id, st1, st2;
  unsigned char mode_val;  // 【修正点】：增加用于传递指针的模式变量
  int x, y, z;
  float xt[13], yt[13], zt[13],power[13],pall,pxy[13];
  char i,j,t;
  float ASAX[13], ASAY[13], ASAZ[13];
  IOSET();
  cs(0);
  Uart1_Init();
  boardset();
  Delay500ms();
  /*  CANInit();
  MPU6050_Init();
	  CAN1_ID = 0x0123;
  TX1_BUF[0] = 0x11;
  TX1_BUF[1] = 0x12;
  TX1_BUF[2] = 0x13;
  TX1_BUF[3] = 0x14;
  TX1_BUF[4] = 0x15;
  TX1_BUF[5] = 0x16;
  TX1_BUF[6] = 0x17;
  TX1_BUF[7] = 0x18;
*/
  EA = 1;
  if(xs==1)PrintString("Initializing...\r\n");

  if(xs==1)printf("\n--- Probe AD0=0 (0x68) ---\n");
  id = WHO_AM_I_Debug(0xD0, 0xD1);
  if(xs==1)printf("WHO_AM_I(0x68) = 0x%02X\n", id);

  Delay10ms();
  UART_SendChar(0x55);



  for (i = 1; i <= 13; i++) {
    cs(i);
    delay_ms(80);

    if (i == skip) {
      if(xs==1)printf("--- IC %d Skipped!---\n", i);
    } else {

      if (s11 == 1 && i > 10) {
        if(xs==1)printf("--- (S11) IC %d Skipped!---\n", i);
      } else {
        ReadNbyte(AK09911C_WIA1, &id, 1);
        if (id != 0x48)
          while (1)
            ;

        ReadNbyte(AK09911C_WIA2, &id, 1);
        if (id != 0x05)
          while (1)
            ;

        if(xs==1)printf("--- IC %d OK!---\n", i);
      }
    }

    // 【修正点】：所有控制寄存器必须写 CNTL2，并且使用变量指针
    // 1. 进入掉电模式
    mode_val = MODE_POWER_DOWN;
    WriteNbyte(AK09911C_CNTL2, &mode_val, 1);
    delay_ms(1);

    // 2. 进入 Fuse ROM 访问模式读取工厂校准数据
    mode_val = 0x1F;
    WriteNbyte(AK09911C_CNTL2, &mode_val, 1);
    delay_ms(1);

    ReadNbyte(REG_ASAX, &id, 1);
    ASAX[i - 1] = (float)id / 128.0f + 1.0f;  // 【修正点】：采用正确的灵敏度调整公式

    ReadNbyte(REG_ASAY, &id, 1);
    ASAY[i - 1] = (float)id / 128.0f + 1.0f;

    ReadNbyte(REG_ASAZ, &id, 1);
    ASAZ[i - 1] = (float)id / 128.0f + 1.0f;

    // 3. 退出 Fuse ROM 访问模式，回到掉电模式
    mode_val = MODE_POWER_DOWN;
    WriteNbyte(AK09911C_CNTL2, &mode_val, 1);
    delay_ms(1);

    // 4. 初始化传感器到连续测量状态
    AK09911C_Init();
  }

  if(xs==1)PrintString("Initialization completed.\r\n");
  delay_ms(100);

  while (1) {
    i++;
    i = i % 13;
    if (i == 0) {
      delay_ms(50);
    }
    t = i + 1;
    cs(t);
    delay_ms(1);
    if (i + 1 != skip) {
      ReadNbyte(AK09911C_ST1, &st1, 1);
    } else {
      st1 = 0x01;
    }


    if (st1 & 0x01) {
      if (i + 1 == skip) {
        x = 0;
        y = 0;
        z = 0;
      } else {
        if (s11 == 1 && i > 10) {
          x = 0;
          y = 0;
          z = 0;
        } else {
          AK09911C_ReadMag(AK09911C_HXL, &x, &y, &z);
          ReadNbyte(AK09911C_ST2, &st2, 1);
        }
      }
      // 【修正点】：乘数改为 0.6，与 AK09911C 灵敏度 0.6 μT/LSB 匹配
      xt[i] = (float)x * ASAX[i] * 0.6f;
      yt[i] = (float)y * ASAY[i] * 0.6f;
      zt[i] = (float)z * ASAZ[i] * 0.6f;
			power[i]=sqrt(xt[i]*xt[i]+yt[i]*yt[i]+zt[i]*zt[i]);
			pxy[i]=xt[i]*xt[i]+yt[i]*yt[i];
			
			
			if(i==12)
			{
			pall=0;
			for (j = 3; j <= 10; j++)	pall=pall+power[j];
				pall=pall/8.0;
				s1=2;
				if(pall>24000.0)s1=1;else if(pall<4000.0)s1=0;
				pz=0;
				for (j = 3; j <= 10; j++)if(pxy[j]<6000.0)pz++;
				if(pz>4)s2=1;else s2=0;
				pfx=0;
				if(yt[3]<0)pfx++;
				if(xt[4]<0)pfx++;
				if(xt[5]<0)pfx++;
				if(yt[6]>0)pfx++;
				if(yt[7]>0)pfx++;
				if(xt[8]>0)pfx++;
				if(xt[9]>0)pfx++;
				if(yt[10]<0)pfx++;
				if(pfx>4)s3=1;else s3=0;
				ss=0xA0;//unknow;
				//K1=0;K2=0;K3=0;K4=0;
				if(s1==2 && s2==0 && s3==1){ss=0xF0;K1=1;K2=0;K3=0;}//非接触
				if(s1==0 && s2==0 && s3==0){ss=0xD0;K1=0;K2=1;K3=0;}//Disable				
				if(s1==2 && s2==1 && s3==0){ss=0xE0;K1=0;K2=0;K3=1;}//Enable
				//putchar(ss|Baddr);
				K4=~K4;
				//printf("%x %d%d%d %d %d %.0f %s",ss|Baddr,s1,s2,s3,pz,pfx,pall);
			}

      /*
      printf("%.3f", xt);
      printf(",");
      printf("%.3f", yt);
      printf(",");
      printf("%.3f", zt);
      printf(",");
			
      u.f = xt;
      TX1_BUF[0] = t;
      TX1_BUF[1] = t;
      TX1_BUF[2] = t;
      TX1_BUF[3] = t;

      TX1_BUF[4] = u.c[0];
      TX1_BUF[5] = u.c[1];
      TX1_BUF[6] = u.c[2];
      TX1_BUF[7] = u.c[3];

      CANSEL = 0;
      sr = CanReadReg(SR);
      if (sr & 0x01) {
        CANAR = MR;
        CANDR &= ~0x04;
      } else {
        B_Can1Send = 1;
        CanSendMsg(CAN1_ID, TX1_BUF);
        WaitCan1Send(500);
      }
      delay_ms(1);

      u.f = yt;
      TX1_BUF[0] = u.c[0];
      TX1_BUF[1] = u.c[1];
      TX1_BUF[2] = u.c[2];
      TX1_BUF[3] = u.c[3];
      u.f = zt;
      TX1_BUF[4] = u.c[0];
      TX1_BUF[5] = u.c[1];
      TX1_BUF[6] = u.c[2];
      TX1_BUF[7] = u.c[3];

      CANSEL = 0;
      sr = CanReadReg(SR);
      if (sr & 0x01) {
        CANAR = MR;
        CANDR &= ~0x04;
      } else {
        B_Can1Send = 1;
        CanSendMsg(CAN1_ID, TX1_BUF);
        WaitCan1Send(500);
      }

      if (i != 12) {
      } else {
        if (P33 == 0)
          printf("0");
        else
          printf("1");
        if (P13 == 0)
          printf(",0");
        else
          printf(",1");
        if (P03 == 0)
          printf(",0");
        else
          printf(",1");
        if (P02 == 0)
          printf(",0,");
        else
          printf(",1,");

//        MPU6050_ReadRawData(&ax, &ay, &az, &gx, &gy, &gz);

//        ComputeEulerAngles_AccelOnly(ax, ay, az, &pitch, &roll, &yaw);

        printf("%.3f", pitch);
        printf(",");
        printf("%.3f", roll);
        printf(",");
        printf("%.3f", yaw);
        printf("\r\n");
				
      }
			*/
    }
    /*
    //------------------处理CAN1模块-----------------------
    CANSEL = 0;
    sr = CanReadReg(SR);

    if ((sr & 0x88) == 0x80) {
      CanReadFifo(buffer);
    }

    if (sr & 0x01) {
      CANAR = MR;
      CANDR &= ~0x04;
    } else {
    }

    if (B_Can1Read) {
      B_Can1Read = 0;
      second = 0;

      CANSEL = 0;
      CAN1_ID = CanReadMsg(RX1_BUF);
      CanSendMsg(CAN1_ID + 1, RX1_BUF);
    }
*/
  }
}