//
// Created by JackTian on 2020/10/27.
//

#ifndef LAB2_TCPRDTSENDER_H
#define LAB2_TCPRDTSENDER_H
#include "RdtSender.h"
#include "Global.h"
#define MAX_SENDER_BUFF 512
#define REDUNDANCY_NUM_LIMIT 3
#define WINDOW_SIZE 10
class TCPRdtSender:public RdtSender{
private:
    int send_base;
    int nextSeqNum;
    bool waitingState;
    int lastAckNum;
    int repeatNum;
    Packet sntPkt[MAX_SENDER_BUFF];
public:
    TCPRdtSender();
    virtual ~TCPRdtSender();
public:
    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet &ackPkt);
    void timeoutHandler(int seqNum);
};
TCPRdtSender::TCPRdtSender():send_base(1),nextSeqNum(1),
waitingState(false),lastAckNum(0),repeatNum(0) {}
TCPRdtSender::~TCPRdtSender() noexcept {}
bool TCPRdtSender::getWaitingState() {
    return this->waitingState;
}
/**
 * send packet when the window is not full, and then start a timer for a group packet
 * @param message
 * @return
 */
bool TCPRdtSender::send(const Message &message) {
    if(this->nextSeqNum<this->send_base+WINDOW_SIZE)
    {
        Packet curPkt;
        curPkt.acknum=-1;
        curPkt.checksum=0;
        curPkt.seqnum=this->nextSeqNum;
        memcpy(curPkt.payload, message.data, sizeof(message.data));
        curPkt.checksum = pUtils->calculateCheckSum(curPkt);
        pUtils->printPacket("发送方发送报文", curPkt);
        this->sntPkt[this->nextSeqNum]=curPkt;
        pns->sendToNetworkLayer(RECEIVER,curPkt);
        if(this->send_base==this->nextSeqNum)
            pns->startTimer(SENDER,Configuration::TIME_OUT,this->nextSeqNum);
        this->nextSeqNum++;
        return true;
    }
    else
    {
        this->waitingState=true;
        return false;
    }
}
/**
 * receive a packet and resend when get REDUNDANCY_NUM_LIMIT ACK number
 * @param ackPkt
 */
void TCPRdtSender::receive(const Packet &ackPkt) {
    int checkSum=pUtils->calculateCheckSum(ackPkt);
    if(ackPkt.checksum==checkSum) {
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        if(ackPkt.acknum!=this->lastAckNum)
        {
            this->lastAckNum=ackPkt.acknum;
            this->repeatNum=1;
        }
        else
            this->repeatNum++;
        //reach to the limit we resend the packet
        if(this->repeatNum==REDUNDANCY_NUM_LIMIT)
        {
            pUtils->printPacket("快速重传报文",this->sntPkt[ackPkt.acknum]);
            pns->sendToNetworkLayer(RECEIVER,this->sntPkt[ackPkt.acknum]);
        }
        this->send_base = ackPkt.acknum + 1;
        if(this->send_base==this->nextSeqNum)
            pns->stopTimer(SENDER,this->send_base);
    }
}
/**
 * when the time out happens, we resend the packet sent earliest but with no ack number
 * @param seqNum
 */
void TCPRdtSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER,this->send_base);
    pns->startTimer(SENDER,Configuration::TIME_OUT,this->send_base);
    pUtils->printPacket("发生超时事件，重传报文",this->sntPkt[this->send_base]);
    pns->sendToNetworkLayer(RECEIVER,this->sntPkt[this->send_base]);
}
#endif //LAB2_TCPRDTSENDER_H
