//
// Created by JackTian on 2020/10/27.
//

#ifndef LAB2_TCPRDTRECEIVER_H
#define LAB2_TCPRDTRECEIVER_H
#define MAX_RECEIVER_BUFF 512
#include "RdtReceiver.h"
#include "Global.h"
class TCPRdtReceiver: public RdtReceiver{
private:
    int rcvBase;
    int expectedSeqNum;
    Packet rcvPkt[MAX_RECEIVER_BUFF];
    Packet sndPkt;
public:
    TCPRdtReceiver();
    virtual ~TCPRdtReceiver();
public:
    void receive(const Packet &packet);
};
TCPRdtReceiver::TCPRdtReceiver() :rcvBase(1),expectedSeqNum(1){
    for(auto & i : rcvPkt)
        i.seqnum=0;//initialize
}
TCPRdtReceiver::~TCPRdtReceiver() noexcept {}
void TCPRdtReceiver::receive(const Packet &packet) {
    int checkSum=pUtils->calculateCheckSum(packet);
    if(checkSum==packet.checksum&&packet.seqnum==this->expectedSeqNum)
    {
//        Packet sndPkt;
        pUtils->printPacket("接受方接收正确的报文",packet);
        this->rcvPkt[packet.seqnum]=packet;//store the packet when it's correct
        sndPkt.acknum=this->expectedSeqNum;
        sndPkt.checksum=0;
        sndPkt.checksum=pUtils->calculateCheckSum(sndPkt);
        pUtils->printPacket("接收方发送确认报文",this->sndPkt);
        pns->sendToNetworkLayer(SENDER,sndPkt);
        this->expectedSeqNum++;
    }
    else
        pns->sendToNetworkLayer(SENDER,this->sndPkt);
    if(this->rcvPkt[this->rcvBase].seqnum!=0){//deliver it to app layer in order
        Message msg;
        memcpy(msg.data, this->rcvPkt[this->rcvBase].payload, sizeof(packet.payload));
        pns->delivertoAppLayer(RECEIVER,msg);
        this->rcvBase++;
    }
}
#endif //LAB2_TCPRDTRECEIVER_H
