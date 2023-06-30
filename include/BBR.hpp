#ifndef __BBR_H__
#define __BBR_H__

#include "tcpConnection.hpp"

const int STARTUP = 0;
const int DRAIN = 1;
const int PROBE_BANDWIDTH = 2;
const int PROBE_RTT = 3;

const int INTERVAL = 25;
const int WAIT_RTT = 5;

class BBR : public TCPConnection
{
public:
    BBR(vector<int> &data) : TCPConnection(data){};
    BBR(vector<int> &data, int cwnd, int ssthresh) : TCPConnection(data, cwnd, ssthresh){};
    void sendData() override;
    bool onePacketLoss() override;
    void oneRTTUpdate() override;
    float lostProb(int num, int start, int end);
    bool isPacketLost();

private:
    int mode = 0;
    int dupAckCount = 0;
    bool packetLost = false;
    bool isbwIncrease = true;
    int sendProb = 0.3;
    int probRttCounter = INTERVAL - 1;
    float minRtt = 1000;
    float estRtt;
    float bandwidth = 1000;
    int actualCwnd = 1;
};
#endif