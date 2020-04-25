/*
	�������Ҫ�������Է����������İ���������ʼ���������Ĳ������Լ�AVPacket������
	�н��ջ���
	�ӵ�packet�󣬷ŵ�queue�ϣ�������������
	��queue����̫��δ�����packet������Ϣ���������������������ݵ��ٶ�

	��Ա������server sockaddr; �����ļ�·��; queue
*/ 

#pragma once

#include <WinSock2.h>

#include <map>

#include "ReadBuffer.h"


class Status;
class PacketQueue;

class NetModule
{
public:
	NetModule(const char* fileName, Status* statusPtr, PacketQueue* packetQueuePtr);
	~NetModule();
	// ���ͣ�����ĳ����Ƶ�ļ�		���գ���Ӧ���������Ƶ�ļ��Ľ������Ĳ���
	void init(ReadBuffer& readBuffer);	
	// ����״̬�������ݻ���Ϣ�����ǳ�������AVPacket
	void loop();	
private:
	// ���͸����������÷������ݻ����Ͱ���Ȼ�����״̬   ����'w'
	void sendBreakMessageAndSetStatus();	
	// �÷������ط�AVPacket�� ����'r'
	void reSendPacketRequest();
	// ����������ɹ�����packet  ����'a'
	void sendACK();
	// ����ĳ���ض���Ƶ�ļ�
	//void sendMediaFileRequest();	
	// ����AVPacket���ݣ�����AVPacket�����ŵ�queue��
	bool receiveAVPacket();	
	// ���ս������Ĳ���
	//void receiveDecoderParameters();	
	// ��buffer���յ���packet���ݹ����AVPacket�����ŵ�queue��
	void consturctPacketAndPutOnQueue(std::map<uint16_t, ReadBuffer>& bufferMap);

	const char* _fileName;
	sockaddr_in _serverSockAddr;
	SOCKET _sockfd;
	Status* _statusPtr;
	// �Ѵ���õ�packet sequence number�����ֵ�����ڿͻ��˷��͵�ack�������������ط��˿ͻ����Ѿ��������packet��������packet��sequence number���С�ڵ��ڸ�ֵ��ֱ���������ͻ����ٷ���ack
	int _maxFinishedPacketNumber = -1;
	PacketQueue* _packetQueuePtr;
};