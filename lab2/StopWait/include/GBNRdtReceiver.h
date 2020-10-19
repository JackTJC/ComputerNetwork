//
// Created by JackTian on 2020/10/19.
//

#ifndef STOPWAIT_GBNRDTRECEIVER_H
#define STOPWAIT_GBNRDTRECEIVER_H
#include "RdtReceiver.h"
#include "Global.h"
class GBNRdtReceiver: public RdtReceiver
{
public:
    GBNRdtReceiver();
    void receive(const Packet &packet);
};
GBNRdtReceiver::GBNRdtReceiver() {

}
void GBNRdtReceiver::receive(const Packet &packet) {
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum)
    {
        Message msg;
        memcpy(msg.data, packet.payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER, msg);
        Packet ackPkt;
        ackPkt.acknum=packet.seqnum;
        pUtils->printPacket("接受方发送ACK",ackPkt);
        pns->sendToNetworkLayer(SENDER,ackPkt);
    }
    else//none
    {
    }
}
#endif //STOPWAIT_GBNRDTRECEIVER_H
