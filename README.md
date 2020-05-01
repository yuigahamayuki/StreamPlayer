# StreamPlayer
A Win media player for video-on-demand based on FFmpeg and SDL2.
## 概述
此客户端程序StreamPlayer与项目中另一服务器程序StreamServer搭配，构成了Windows/Linux上的视频点播软件。此视频点播软件的原理并不是基于HTTP的文件下载，而采用了流媒体传输的方式。
网络传输使用的是UDP。
视频解码使用的是FFmpeg。
视频渲染使用的是SDL2。

## 客户端主要构成
主要组件有3个：网络模块；解码器；渲染器。每一个模块分别有自己的线程进行循环处理。
### 1. 网络模块
功能：给服务器发送需要播放的文件名并接收服务器发来的解码器参数来初始化解码器；接收服务器发来的AVPacket数据，并构造AVPacket作为解码器的输入。
### 2. 解码器
功能：对网络模块获得的AVPacket输入到解码器进行解码，进行必要的图像格式转换（分辨率，像素格式）后作为渲染器的输入。
### 3. 渲染器
功能：对解码器解码出的AVFrame进行定时渲染（按帧率）。

## 客户端大致流程
1. 网络模块给服务器发送要点播的视频文件名，阻塞等待服务器发送解码器参数。
2. 收到解码器参数后用参数初始化解码器。初始化完成后，网络模块、解码器、渲染器各自在自己的线程循环处理。
3. 网络模块循环等待服务器发来的AVPacket，收到完整的AVPacket后，构造出AVPacket并放到PacketQueue上。
4. 解码器从PacketQueue取AVPacket，解码出AVFrame，并转换图像格式后放到FrameQueue上。
5. 渲染器从FrameQueue取AVFrame，渲染完一帧后，等待40ms（25FPS）后渲染下一帧。

## 客户端技术细节
1. 网络模块接收AVPacket时解决UDP传输可能的乱序以及丢包。每个UDP包的头参考StreamServer的描述。针对乱序，使用了以分片号为key，UDP数据报为value的std::map；针对丢包，检查哪些分片号的包没有收到，请求服务器重新发送这些分片。
2. FrameQueue以及PacketQueue都使用了互斥锁和条件变量实现不同线程的同步问题。
