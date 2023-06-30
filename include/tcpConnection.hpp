#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <cmath>
using namespace std;

const int TIMEOUT = 2;
const int DATA_SIZE = 10000;

const int SLOW_START = 0;
const int CONGESTION_AVOIDANCE = 1;
const int FAST_RETRANSMIT = 2;

// const int SSTHRESH = numeric_limits<int>::max();
const int SSTHRESH = 1000;
const int CWND = 1;

struct SlidingWindow
{
    int lastSent;
    int lastAck;
    int lastWritten;
};

struct Packet
{
    int data;
    bool ack;
    bool sack;
};

class TCPConnection
{
public:
    TCPConnection(vector<int> &data);
    TCPConnection(vector<int> &data, int cwnd, int ssthresh);
    virtual ~TCPConnection() = default;
    virtual void sendData() = 0;
    virtual bool onePacketLoss() = 0;
    virtual void oneRTTUpdate() = 0;
    void run();

protected:
    int _cwnd = CWND;
    int _ssthresh = SSTHRESH;
    int _rtt = 0;
    int timeout = 0;
    int advwnd = 1000;

    vector<Packet> packets;
    SlidingWindow sw = {-1, -1, -1};
    void packetize(vector<int> &data);
    void printStatus();
    int getWindowSize();
};
#endif