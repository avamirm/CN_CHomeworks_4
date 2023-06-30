#ifndef __TCPCONNECTION_H__
#define __TCPCONNECTION_H__
#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
using namespace std;

const int TIMEOUT = 2;

const int SLOW_START = 0;
const int CONGESTION_AVOIDANCE = 1;
const int FAST_RETRANSMIT = 2;

enum Level
{
    slowStart,
    congestionAvoidance,
    fastRetransmit
};

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
    int _cwnd = 1;
    int _ssthresh = numeric_limits<int>::max();
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