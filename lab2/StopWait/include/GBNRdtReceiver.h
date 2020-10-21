//
// Created by JackTian on 2020/10/19.
//

#ifndef STOPWAIT_GBNRDTRECEIVER_H
#define STOPWAIT_GBNRDTRECEIVER_H
#include "RdtReceiver.h"
#include "Global.h"
class GBNRdtReceiver: public RdtReceiver
{
private:
    int expectedSeqNum;
    Packet sndPkt;
public:
    GBNRdtReceiver();
    void receive(const Packet &packet);
};
GBNRdtReceiver::GBNRdtReceiver():expectedSeqNum(1) {
    this->sndPkt.seqnum=0;
    this->sndPkt.acknum=0;
    this->sndPkt.checksum=0;
    this->sndPkt.checksum=pUtils->calculateCheckSum(this->sndPkt);
}
void GBNRdtReceiver::receive(const Packet &packet) {
    //检验校验和
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum&&packet.seqnum==this->expectedSeqNum)
    {
        pUtils->printPacket("接受方接收正确的报文",packet);
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        this->sndPkt.acknum=this->expectedSeqNum;
        this->sndPkt.checksum=0;
        this->sndPkt.checksum=pUtils->calculateCheckSum(this->sndPkt);
        pUtils->printPacket("接受方发送确认报文",this->sndPkt);
        pns->sendToNetworkLayer(SENDER,this->sndPkt);
        this->expectedSeqNum++;
    }
    else
        pns->sendToNetworkLayer(SENDER,this->sndPkt);

}
#endif //STOPWAIT_GBNRDTRECEIVER_H
