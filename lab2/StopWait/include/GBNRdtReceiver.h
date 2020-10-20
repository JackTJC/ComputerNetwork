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
public:
    GBNRdtReceiver();
    void receive(const Packet &packet);
};
GBNRdtReceiver::GBNRdtReceiver():expectedSeqNum(1) {

}
void GBNRdtReceiver::receive(const Packet &packet) {
    //检验校验和
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum&&packet.seqnum>0)
    {
        pUtils->printPacket("接受方接收正确的报文",packet);
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        Packet ackPkt;
        ackPkt.acknum=this->expectedSeqNum;
        ackPkt.checksum=0;
        ackPkt.checksum=pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("接受方发送确认报文",ackPkt);
        pns->sendToNetworkLayer(SENDER,ackPkt);
        this->expectedSeqNum++;
    }
}
#endif //STOPWAIT_GBNRDTRECEIVER_H
