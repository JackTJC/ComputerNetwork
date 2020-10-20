//
// Created by JackTian on 2020/10/19.
//

#ifndef STOPWAIT_GBNRDTSENDER_H
#define STOPWAIT_GBNRDTSENDER_H
#define WINDOW_SIZE 10
#define MAX_BUFF_SIZE 100
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
    Packet sntPkt[MAX_BUFF_SIZE];
public:
    bool getWaitingState();
    bool send(const Message &message);
    void receive(const Packet &ackPkt);
    void timeoutHandler(int seqNum);
public:
    GBNRdtSender();
    virtual ~GBNRdtSender();
};
GBNRdtSender::GBNRdtSender():base(1),nextSeqNum(1),waitingState(false)
{
}
GBNRdtSender::~GBNRdtSender() noexcept {}
bool GBNRdtSender::getWaitingState() {
    return this->waitingState;
}
bool GBNRdtSender::send(const Message &message) {
    if(this->waitingState)
        return false;
    if(this->nextSeqNum<this->base+WINDOW_SIZE)
    {
        Packet curPkt;
        curPkt.acknum=-1;
        curPkt.checksum=0;
        curPkt.seqnum=this->nextSeqNum;
        memcpy(curPkt.payload, message.data, sizeof(message.data));
        curPkt.checksum = pUtils->calculateCheckSum(curPkt);
        pUtils->printPacket("发送方发送报文", curPkt);
        sntPkt[this->nextSeqNum]=curPkt;
        pns->sendToNetworkLayer(RECEIVER,curPkt);
        if(this->base==this->nextSeqNum)
            pns->startTimer(SENDER,Configuration::TIME_OUT,this->base);
        return true;
    }
    else
        this->waitingState=true;
}
void GBNRdtSender::receive(const Packet &ackPkt) {

        int checkSum=pUtils->calculateCheckSum(ackPkt);
        //check OK
        if(ackPkt.checksum==checkSum) {
            pUtils->printPacket("发送方正确收到确认", ackPkt);
            this->base = ackPkt.acknum + 1;
            if (this->base == this->nextSeqNum)
                pns->stopTimer(SENDER, this->nextSeqNum);
            else
                pns->startTimer(SENDER, Configuration::TIME_OUT, this->nextSeqNum);
        }
}

void GBNRdtSender::timeoutHandler(int seqNum) {
//Resend the data sent but with no ack
    for(int i=this->base;i<=this->nextSeqNum-1;i++)
    {
        pns->stopTimer(SENDER,i);
        pns->startTimer(SENDER,Configuration::TIME_OUT,i);
        pUtils->printPacket("发送发重新发送报文",this->sntPkt[i]);
        pns->sendToNetworkLayer(RECEIVER,this->sntPkt[i]);
    }
}
#endif //STOPWAIT_GBNRDTSENDER_H
