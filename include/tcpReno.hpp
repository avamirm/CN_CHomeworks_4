#ifndef __TCPRENO_H__
#define __TCPRENO_H__

#include "tcpConnection.hpp"
#include <cmath>

class TCPReno : public TCPConnection
{
public:
    TCPReno(vector<int> &data) : TCPConnection(data){};
    TCPReno(vector<int> &data, int cwnd, int ssthresh) : TCPConnection(data, cwnd, ssthresh){};
    void sendData() override;
    bool onePacketLoss() override;
    void oneRTTUpdate() override;
    float lostProb(double num, double start, double end);

private:
    int mode = 0;
    int dupAckCount = 0;
    bool packetLost = false;
};
#endif