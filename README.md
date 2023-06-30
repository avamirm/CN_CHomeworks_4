# CN_CHomeworks_4

## How to run?

using the terminal cd to CN_CHomworks_4 directory and then:

```cpp
$ make
```

then run:

```cpp
$./main.out
```

## Project Description

This project compares the performance of different congestion control protocols including Reno, New Reno, BBR.

## Definition of the Classes

To start the program, we define 4 classes:

- TCPConnection

  It is the main class and three other classes inherit from it and contains three main functions.

```cpp
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
```

- TCPReno

  It inherits from the main class and implements the Reno protocol.

```cpp
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
    Level _mode = Level::slowStart;
    int dupAckCount = 0;
    bool packetLost = false;
};
```

## SendData

In this funtion we send the packets which some of them are lost due to a exponential probability. If packets are not lost, we just increase the lastAck pointer, otherwise we should increase the duplicate counter.

```cpp
void TCPReno::sendData()
{
    if (timeout != 0)
        return;

    dupAckCount = 0;
    packetLost = false;
    for (int i = lastAck; i < lastAck + _cwnd && i < static_cast<int>(packets.size()); i++)
    {
        if (lostProb(_cwnd, 1, 1000) < 0.9)
        {
            if (packetLost)
                dupAckCount++;
            else
                sw.lastAck++;
        }
        else
            packetLost = true;
    }
}
```

## onePacketLoss

In this function we should trigger the appropriate congestion control mechanism and handle them.

```cpp
bool TCPReno::onePacketLoss()
{
    if (timeout != 0)
    {
        timeout--;
        return true;
    }

    if (dupAckCount >= 3)
    {
        _mode = Level::fastRetransmit;
        _cwnd = _cwnd / 2;
        _ssthresh = _cwnd;
        return true;
    }
    if (packetLost)
    {
        _mode = Level::slowStart;
        timeout = TIMEOUT;
        _ssthresh = _cwnd / 2;
        _cwnd = 1;
        return true;
    }
    return false;
}
```

## oneRTTUpdate

In this function we should update the RTT and adjust the cwnd accordingly, based on the current congestion control mode.