//
// Created by JackTian on 2020/10/21.
//

#ifndef LAB2_SRRDTSENDER_H
#define LAB2_SRRDTSENDER_H
#include "RdtSender.h"
#include "Global.h"
#define MAX_SENDER_BUFF 512
#define WINDOW_SIZE 10
class SRRdtSender: public RdtSender
{
private:
    int send_base;
    int nextSeqNum;
    bool waitingState;
    Packet sntPkt[MAX_SENDER_BUFF];
public:
    SRRdtSender();
    virtual ~SRRdtSender();
public:
    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet &ackPkt);
    void timeoutHandler(int seqNum);
};
SRRdtSender::SRRdtSender():send_base(1),nextSeqNum(1),waitingState(false) {}
SRRdtSender::~SRRdtSender() noexcept {}
bool SRRdtSender::getWaitingState(){
    return this->waitingState;
}
/**
 * send a packet if the sender's window is not full, and start a a timer for each single
 * packet
 * @param message
 * @return
 */
bool SRRdtSender::send(const Message &message) {
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
 * when receive a correct packet we update the send base immediately
 * @param ackPkt
 */
void SRRdtSender::receive(const Packet &ackPkt) {
    int checkSum=pUtils->calculateCheckSum(ackPkt);
    if(ackPkt.checksum==checkSum) {
        pUtils->printPacket("发送方正确收到确认", ackPkt);
        this->send_base = ackPkt.acknum + 1;
        pns->stopTimer(SENDER,ackPkt.acknum);
    }
}
/**
 * we resend  the packet numbered seqNum and start a new timer for it.
 * @param seqNum
 */
void SRRdtSender::timeoutHandler(int seqNum) {
    pns->stopTimer(SENDER,seqNum);
    pns->startTimer(SENDER,Configuration::TIME_OUT,seqNum);
    pUtils->printPacket("发送发重新发送报文",this->sntPkt[seqNum]);
    pns->sendToNetworkLayer(RECEIVER,this->sntPkt[seqNum]);
}
#endif //LAB2_SRRDTSENDER_H
