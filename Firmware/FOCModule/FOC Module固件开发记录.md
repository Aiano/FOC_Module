# FOC Module固件开发记录

## 运行参数

### 变量类型

#### 浮点数变量

> [Using STM32 Hardware Floating Point Support – VisualGDB Tutorials](https://visualgdb.com/tutorials/arm/stm32/fpu/#:~:text=STM32F4 floating-point unit only supports 32-bit floating point,a run-time exception. We will demonstrate it now.)
>
> [STM32F4系列，能支持double型的浮点运算吗-CSDN社区](https://bbs.csdn.net/topics/392629095)

STM32F4系列单片机**只有硬件单精度浮点**，**只能用float类型变量**。如果使用较老版本的GCC编译器，同时使用double变量，则会导致运行时错误，GCC 4.9.1以后修复了这个问题。

为了加快浮点数计算效率，所有浮点数变量全部使用float类型。

## PWM

### Counter Mode 计数模式

> [STM32-一文搞懂通用定时器捕获/比较通道_捕获比较通道-CSDN博客](https://blog.csdn.net/wei348144881/article/details/109091539)
>
> [STM32F103高级定时器死区时间的计算_stm32死区生成-CSDN博客](https://blog.csdn.net/geek_monkey/article/details/82809931)

![image-20230926215739656](_media/image-20230926215739656.png)

## ADC低侧采样

> [foc配置篇——ADC注入组使用定时器触发采样的配置_定时器控制adc采样-CSDN博客](https://blog.csdn.net/jdhfusk/article/details/126201653)
>
> [STM32 进阶教程 17 - ADC注入通道_stm32 adc注入-CSDN博客](https://blog.csdn.net/zhanglifu3601881/article/details/97613959)

## DAC

> [【STM32】HAL库 ——DAC_stm32 hal dac-CSDN博客](https://blog.csdn.net/Qxiaofei_/article/details/119108963)

## SVPWM

> [关于电机dq轴的理解 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/641738156)

## DSP

> [STM32F4+CubeMX+Hal库下使能FPU_cubemax fpu加速-CSDN博客](https://blog.csdn.net/jaysur/article/details/108928490)
>
> [STM32 HAL库 CUBEMX FPU 和 DSP库 使用-CSDN博客](https://blog.csdn.net/yuleitao/article/details/105621492)
>
> [STM32 DSP库的快速添加 基于cubemx 调用，使用DSP库_#include "arm_const_structs.h"怎么添加_四臂西瓜的博客-CSDN博客](https://blog.csdn.net/qq_34022877/article/details/117855263)
>
> [如何使用CLion添加STM32F4xx的DSP库 - 哔哩哔哩 (bilibili.com)](https://www.bilibili.com/read/cv19024271/?spm_id_from=333.999.0.0)

## USB虚拟串口收发

> [【精选】【STM32+cubemx】0016 HAL库开发：USB虚拟串口（virtual COM）_hal usb-CSDN博客](https://blog.csdn.net/little_grapes/article/details/121549875)

`CDC_Receive_FS`和`CDC_Transmit_FS`这两个函数就是应用层实现收、发的函数。

发送函数`CDC_Receive_FS`可以直接被用户使用，调用函数，只要传入待发送数据的**首地址**和**数据长度**就可以把数据从USB虚拟串口中发送出去了。

> [STM32 USB使用记录：使用CDC类虚拟串口（VCP）进行通讯_cdc_control_fs函数是用来干什么的_Naisu Xu的博客-CSDN博客](https://blog.csdn.net/Naisu_kun/article/details/118192032)

## 三环

### 位置环

### 速度环

> [【电机控制Pocket】——噪声与延迟的取舍？速度观测器的设计思路 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/464180273)

#### 测速方法

- M法测速：在规定时间内测脉冲个数来获得被测速度，也被称为**频率法**。M法测速最大误差为一个脉冲，误差率与脉冲个数呈反比，转速越高，单位时间脉冲数越多，误差率越小，因此M法测速适合**高转速**测量。要提高M法测速的分辨率，可以增大编码器每圈的脉冲数，或者增大采样周期。
- T法测速：测相邻两个脉冲的时间间隔来获得被测速度，也被称为**周期法**。T法测速最大误差为一个定时器时钟周期，转速越低，两个脉冲间的定时器时钟周期数越多，误差率越小，因此T法测速适合**低转速**测量。但是低速下两个脉冲间速度不会更新，会带来延迟。
- M/T法测速：同时测量检测时间和在次时间内的脉冲数来获得被测速度，是M法和T法的结合。M/T法测速在低速和高速段都有很高的分辨率，但是速度很低时，需要兼顾系统的实时性。

**问题：**当位置环处在目标位置时，电机会出现振动。

**原因：**速度环出现问题，速度环的频率约为5kHz，速度由角度传感器进行差分得到，由于角度传感器的分辨率有限且存在噪声，速度环频率较高时，测得的速度分辨率就会降低，出现阶梯状的速度波形。单纯使用角度差分得到速度，适合高转速测速，低速下误差率会增高，导致电机出现振动。

**解决方法：**对测量速度进行低通滤波或滑动平均滤波。

**问题：**使用低通滤波后，电机的振动更剧烈了。

**原因：**高延迟+高增益=输出震荡，低通滤波器会增大响应的延时，此时如果还保留原先PID的参数的话，增益可能就过高了，会导致输出震荡，此时可以减小P，同时加入I，保证输出不震荡同时输出跟随输入。

### 电流环

## CAN

> [CAN详解--协议详解_can协议完全讲解-CSDN博客](https://blog.csdn.net/qq_38880380/article/details/84573821)

### CAN总线负载率

> [五分钟教会如何计算CAN总线负载率，包学包会_can线负载率怎么算-CSDN博客](https://blog.csdn.net/LEON1741/article/details/105909870)

**定义：**总线上所有节点单位时间发送的数据占总线理论带宽的比值

**公式：**负载率R = 每秒钟实际上传输的所有数据的BIT总数A / 每秒钟理论上所能传输的BIT总数T

对于标准帧，数据帧，数据长度为8字节：

| SOF  | ID   | RTR  | IDE  | r0   | DLC  | Data | CRC  | DIV  | ACK  | EOF  |
| ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- | ---- |
| 1    | 11   | 1    | 1    | 1    | 4    | 8*8  | 15   | 1    | 2    | 7    |

总计一帧108bit，帧间隔根据协议规定，为3bit，故完成一帧全部发送行为共108+3=111bit。

假设总线波特率为1Mbps，工作在标准帧，数据帧，数据长度8字节，共5个ID，按1kHz频率发送，则总线负载率为：
$$
R=\frac{111\times5\times1000}{1000000}\times100\% \\
=55.5\%
$$
为保证低优先级的报文传输延迟在可接受范围内，总线利用率不应超过30%，否则低优先级的报文可能会延迟。

然而只要保证每个节点最大响应时间满足要求，负载率就无关紧要。

## 中断

### IRQ

> [什么是IRQ（中断） - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/598570224)

IRQ的全称是“Interrupt ReQuest”，即“中断请求”

### 中断与事件的区别

> [STM32---中断与事件的区别_中断和事件的区别-CSDN博客](https://blog.csdn.net/liangweibeijing/article/details/95219626)
>
> [stm32—外部中断、中断和事件的区别-CSDN博客](https://blog.csdn.net/m0_46573394/article/details/112691572)

- 事件就是一件事发生了，比如EXTI检测到边沿；中断就是打断CPU，进入中断处理函数
- 事件能触发中断，流入其他外设，是硬件级的；中断会调用中断处理函数，是软件级的
- 事件是中断的触发源，只要开启相应的中断屏蔽位

### on the fly 的含义

> [On the fly - Wikipedia](https://en.wikipedia.org/wiki/On_the_fly)
>
> [(5 封私信 / 80 条消息) 如何优雅的翻译 on the fly ？ - 知乎 (zhihu.com)](https://www.zhihu.com/question/21136587)

on the fly 是指在做一件事的时候，无需打断前置的事；

比如开飞机的时候更换飞机引擎，而无需降落；安装一个驱动，而无需重启电脑。

## 温度传感

> [0402贴片热敏电阻 10KΩ ±1% B:3380 SDNT1005X103F3380FTF 50只-淘宝网 (taobao.com)](https://item.taobao.com/item.htm?_u=m20ci6tpvfc6b9&id=560131297850&spm=a1z09.8149145.0.0.43f33fb8XeRSUQ)
>
> [NTC热敏电阻-阻值温度计算 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/409548358)

### NTC阻值-温度计算法

常用的计算方法是**温度系数B计算法**：
$$
R_1=R_2e^{B\left( \frac{1}{T_1}-\frac{1}{T_2} \right)}
\\
T_1=\frac{1}{\frac{1}{B}\ln \frac{R_1}{R_2}+\frac{1}{T_2}}
$$
其中：

- 温度T的单位是开尔文（K），T(K) = 273.15 + T(℃)
- R1是热敏电阻在T1温度下的阻值
- R2是热敏电阻在T2温度下的标称阻值，T2通常取25℃，即298.15K

## Debug

### 使用USB CDC虚拟串口时，出现偶发性卡死

#### 现象

使用Debug模式，卡死后查看调用栈Call Stack，发现：

1. HardFault_Handler

    ...

    vsnprintf

2. CDC_printf

3. main_loop

#### 原因

C语言的printf系列函数是线程不安全的，不能被同时调用，这里由于在主循环和中断中都需要打印信息，当主循环调用printf时中断，中断再次调用printf，就会进HardFault_Handler。

#### 解决方法

1. 给CDC_printf函数加全局锁，但是这样中断里的调试信息有几率发不出来；
2. 中断中加标志位，待中断结束后在主循环打印调试信息，可以包装成一个日志函数调用。