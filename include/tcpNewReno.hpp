#ifndef __TCPNEWRENO_H__
#define __TCPNEWRENO_H__

#include "tcpConnection.hpp"

class TCPNewReno : public TCPConnection
{
public:
    TCPNewReno(vector<int> &data) : TCPConnection(data){};
    TCPNewReno(vector<int> &data, int cwnd, int ssthresh) : TCPConnection(data, cwnd, ssthresh){};
    void sendData() override;
    bool onePacketLoss() override;
    void oneRTTUpdate() override;
    float lostProb(int num, int start, int end);
    bool isPacketLost();

private:
    int mode = 0;
    int dupAckCount = 0;
    bool packetLost = false;
    vector<bool> sacks = vector<bool>(packets.size(), false);
};
#endif