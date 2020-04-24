/*
	�������Ҫ�������Է����������İ���������ʼ���������Ĳ������Լ�AVPacket������
	�н��ջ���
	�ӵ�packet�󣬷ŵ�queue�ϣ�������������
	��queue����̫��δ�����packet������Ϣ���������������������ݵ��ٶ�

	��Ա������server sockaddr; �����ļ�·��; queue
*/ 

#pragma once

#include <WinSock2.h>

#include "ReadBuffer.h"

class NetModule
{
public:
	NetModule(const char* fileName);
	~NetModule();
	void init(ReadBuffer& readBuffer);	// ���ͣ�����ĳ����Ƶ�ļ�		���գ���Ӧ���������Ƶ�ļ��Ľ������Ĳ���
	void loop();	// ����״̬�������ݻ���Ϣ�����ǳ�������AVPacket
private:
	void sendBreakMessage();	// ���͸����������÷������ݻ����Ͱ�
	void sendMediaFileRequest();	// ����ĳ���ض���Ƶ�ļ�
	void receiveAVPacket();		// ����AVPacket���ݣ�����AVPacket�����ŵ�queue��
	void receiveDecoderParameters();	// ���ս������Ĳ���

	const char* _fileName;
	sockaddr_in _serverSockAddr;
	SOCKET _sockfd;
};