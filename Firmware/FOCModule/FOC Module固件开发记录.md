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