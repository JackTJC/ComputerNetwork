//
// Created by JackTian on 2020/10/21.
//

#ifndef LAB2_SRRDTRECEIVER_H
#define LAB2_SRRDTRECEIVER_H
#include "RdtReceiver.h"
#include "Global.h"
#define MAX_RECEIVER_BUF 512
class SRRdtReceiver: public RdtReceiver
{
private:
    Packet rcvPkt[MAX_RECEIVER_BUF];
    int rcv_base;
public:
    SRRdtReceiver();
    virtual ~SRRdtReceiver();
public:
    void receive(const Packet &packet);
};
SRRdtReceiver::SRRdtReceiver():rcv_base(1) {
    for(auto & i : this->rcvPkt)
        i.seqnum=0;
}
SRRdtReceiver::~SRRdtReceiver() noexcept {}
/**
 * if we receive a correct packet, we send a acknowledge packet and store the packet in buffer
 * first, and then we check the next number has been received whether or not, if so, we diliver
 * it to the app layer.
 * @param packet
 */
void SRRdtReceiver::receive(const Packet &packet) {
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum)
    {
        Packet ackPkt;
        pUtils->printPacket("接受方接收正确的报文",packet);
        this->rcvPkt[packet.seqnum]=packet;//store the packet when it's correct
        ackPkt.acknum=packet.seqnum;
        ackPkt.checksum=0;
        ackPkt.checksum=pUtils->calculateCheckSum(ackPkt);
        pUtils->printPacket("接受方发送确认报文",ackPkt);
        pns->sendToNetworkLayer(SENDER,ackPkt);
    }
    if(this->rcvPkt[this->rcv_base].seqnum!=0){//deliver it to app layer in order
        Message msg;
        memcpy(msg.data, this->rcvPkt[this->rcv_base].payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER,msg);
        this->rcv_base++;
    }
}
#endif //LAB2_SRRDTRECEIVER_H
