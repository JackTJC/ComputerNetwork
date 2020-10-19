//
// Created by JackTian on 2020/10/19.
//

#ifndef STOPWAIT_GBNRDTSENDER_H
#define STOPWAIT_GBNRDTSENDER_H
#define WINDOW_SIZE 10
#include "RdtSender.h"
#include "Global.h"
#include <list>
using namespace  std;
class JudgeRemove{
    int n;
public:
    JudgeRemove(int n){this->n=n;};
    bool operator ()(Packet &pkt) const{return pkt.seqnum<=n;};
};
class GBNRdtSender: public RdtSender{
private:
    bool waitingState;
    int base;
    int nextSeqNum;
    list<Packet> sentPackets;
    bool isTimerStart;
public:
    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet &ackPkt);
    void timeoutHandler(int seqNum);
public:
    GBNRdtSender();
    virtual ~GBNRdtSender();
};
GBNRdtSender::GBNRdtSender():base(0),nextSeqNum(0),waitingState(false),isTimerStart(false)
{
}
GBNRdtSender::~GBNRdtSender() noexcept {}
bool GBNRdtSender::getWaitingState() {
    return this->waitingState;
}
bool GBNRdtSender::send(const Message &message) {
    if(this->waitingState)
        return false;
    Packet curPkt;
    curPkt.acknum=-1;
    curPkt.checksum=0;
    curPkt.seqnum=nextSeqNum++;
    memcpy(curPkt.payload, message.data, sizeof(message.data));
    curPkt.checksum = pUtils->calculateCheckSum(curPkt);
    pUtils->printPacket("发送方发送报文", curPkt);
    if(!isTimerStart)
    {
        pns->startTimer(SENDER,Configuration::TIME_OUT,this->base);
        this->isTimerStart=true;
    }
    this->sentPackets.push_back(curPkt);
    pns->sendToNetworkLayer(RECEIVER,curPkt);
    //check the window full or not
    if(this->nextSeqNum-this->base==WINDOW_SIZE)
        this->waitingState=true;
    return true;
}
void GBNRdtSender::receive(const Packet &ackPkt) {
//In GBN we use accumulative confirmation
//with ack(n) we can think that seqnum<n have been confirmed
    if(this->waitingState)
    {
        int checkSum=pUtils->calculateCheckSum(ackPkt);
        //check OK
        if(ackPkt.checksum==checkSum)
        {
            pUtils->printPacket("接收方正确收到确认",ackPkt);
            this->sentPackets.remove_if(JudgeRemove(ackPkt.acknum));//when seqnum<n
            pns->stopTimer(SENDER,this->base);
            this->base=ackPkt.acknum+1;
            pns->startTimer(SENDER,Configuration::TIME_OUT,this->base);
            this->isTimerStart=true;
        }
    }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
//Resend the data sent but with no ack
    for(const Packet& pkt:this->sentPackets)
    {
        pUtils->printPacket("发送放重新发送报文",pkt);
        pns->stopTimer(SENDER,seqNum);
        pns->startTimer(SENDER,Configuration::TIME_OUT,this->base);
        this->isTimerStart=true;
        pns->sendToNetworkLayer(RECEIVER,pkt);
    }
}
#endif //STOPWAIT_GBNRDTSENDER_H
