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

  It is the main class and three other classes inherit from it and contains three main functions. This is an abstract class and we implement sendData(), onePacketLoss(), and oneRTTUpdate() methods in this class's children. we first set ssthresh to infinity and timout to 0.

- Sliding Window

  We define a sliding window struct that stores the id of last sent and last acked  packet, it also stores the id of the last written packet.
```cpp
struct SlidingWindow
{
    int lastSent;
    int lastAck;
    int lastWritten;
};
```

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

  It inherits from the main class and implements the Reno protocol. it sets the mode to slow start and define a boolean variable which sets to true if a packet is lost; To start the  program we set it to false.

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
    int mode = 0;
    int dupAckCount = 0;
    bool packetLost = false;
};
```

## SendData

In this funtion we send the packets which some of them are lost due to a exponential probability. If packets are not lost, we just increase the lastAck pointer, otherwise we should increase the duplicate counter. If the probability of the packet loss is more that a constant value like 0.9, we assume that the packet is lost.

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

In this function we should trigger the appropriate congestion control mechanism and handle them. First of all it checks if the timeout variable is not equal to 0. If it is not 0, it decrements the timeout variable by 1 and returns true.
If the dupAckCount variable is greater than or equal to 3, it sets the mode variable to fastRetransmit, divides _cwnd (congestion window) by 2, sets _ssthresh (slow start threshold) to _cwnd, and returns true.
If the packetLost variable is true, it sets the _mode variable to slowStart, sets timeout to a constant value TIMEOUT, sets _ssthresh to half of _cwnd, sets _cwnd to 1, and returns true. If none of the above conditions are met, it returns false

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
        mode = FAST_RETRANSMIT;
        _cwnd = _cwnd / 2;
        _ssthresh = _cwnd;
        return true;
    }
    if (packetLost)
    {
        mode = SLOW_START;
        timeout = TIMEOUT;
        // timeout += 1;
        _ssthresh = _cwnd / 2;
        _cwnd = 1;
        return true;
    }
    return false;
}
```

## oneRTTUpdate

In this function we should update the RTT and adjust the cwnd accordingly, based on the current congestion control mode. if the current mode is slow start, we set cwnd according to ssthresh; if the current mode is cogestion avoidance, we increase cwnd by 1 and if the mode is fast retransmit we set the mode to congestion avoidance.
```cpp
void TCPReno::oneRTTUpdate()
{
    if (mode == SLOW_START)
    {
        if (_cwnd < _ssthresh)
            _cwnd *= 2;
        else if (_cwnd == _ssthresh)
            mode = CONGESTION_AVOIDANCE;
        else if (2 * _cwnd > _ssthresh)
            _cwnd = _ssthresh;
    }
    else if (mode == CONGESTION_AVOIDANCE)
    {
        _cwnd++;
    }
    else if (mode == FAST_RETRANSMIT)
    {
        mode = CONGESTION_AVOIDANCE;
    }
}
```
### TCP Reno with ssthresh = INF & cwnd = 1:
  ![reno,cwnd=1,ssthresh=inf](/images/reno,cwnd=1,ssthresh=inf.png "reno,cwnd=1,ssthresh=inf")

### TCP Reno with ssthresh = INF & cwnd = 200:
  ![reno,cwnd=200,ssthresh=inf](/images/reno,cwnd=200,ssthresh=inf.png "reno,cwnd=200,ssthresh=inf")
 As we can see when we increase the initial cwnd to 200, it takes less RTTs to send the data thus the performance is better in this case because we send more data at first and the cwnd grow sooner but we cannot always set cwnd to a big number because it may cause timeout sooner.
### TCP Reno with ssthresh = 10 & cwnd = 1:
![reno,cwnd=1,ssthresh=10](/images/reno,cwnd=1,ssthresh=10.png "reno,cwnd=1,ssthresh=10")
As we can see when we decrease the initial ssthresh to 10, it takes more RTTs to send the data thus the performance is worse in this case because we reach the threshold sooner and decrease the cwnd sooner and can't use the whole cwnd effectively.

![renoChart](/images/renoChart.png "renoChart")
As we can see cwnd increases until finding the threshold and then after recieving three duplicate ACKs, the mode changes to fast retransmit. Fast retransmit are obviously visible in the chart.

- TCPNewReno

  It inherits from the main class and implements the NewReno protocol. it sets the mode to slow start and define a boolean variable which sets to true if a packet is lost; To start the  program we set it to false. We also define an integer variable called dupAckCount to count the number of duplicated ack the sender gets. In NewReno protocol we also have sacks to know which packet is recieved in addition of the ack that shows us what packet the reciever is waiting to get.

```cpp
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
```

## SendData

In this function we send the packets. At first if we have sent all packets or the timeout is not 0, we return. Otherwise set the dupAckCount and sackCount to zero, and also packetLost to false; Then in a loop from the the packet that the sender recieved its ack to the last we do the following: if we get the sack of the packet and the packet is sent successfully, we increase sackCount by 1 and if packetLost is false, it means that we have recieved an ack. Then we calculate the probability of a loss by isPacketLost() function. If the function returns true, the sack of that packet is true. According to packetLost value we increase dupAckCount or lastAck by one. Otherwise we set packetLost to true.

```cpp
void TCPNewReno::sendData()
{
    if (sw.lastAck + 1 == static_cast<int>(packets.size()) || timeout != 0)
        return;

    dupAckCount = 0;
    packetLost = false;
    int lastAck = sw.lastAck;
    int sackCount = 0;
    for (int i = lastAck; i < sackCount + lastAck + _cwnd && i < static_cast<int>(packets.size()); i++)
    {
        if (packets[i].sack)
        {
            sackCount++;
            if (!packetLost)
                sw.lastAck++;
            continue;
        }
        if (isPacketLost())
        {
            if (packetLost)
            {
                dupAckCount++;
                packets[i].sack = true;
            }
            else
            {
                packets[i].sack = true;
                sw.lastAck++;
            }
        }
        else
            packetLost = true;
    }
}
```
The two other methods in New Reno including onePacketLoss() and oneRTTUpdate() is like Reno.

### TCP newReno with ssthresh = INF & cwnd = 1:
  ![newReno,cwnd=1,ssthresh=inf](/images/newReno,cwnd=1,ssthresh=inf.png "newReno,cwnd=1,ssthresh=inf")

### TCP newReno with ssthresh = INF & cwnd = 200:
  ![newReno,cwnd=200,ssthresh=inf](/images/newReno,cwnd=200,ssthresh=inf.png "newReno,cwnd=200,ssthresh=inf")
 As we can see like Reno protocol when we increase the initial cwnd to 200, it takes less RTTs to send the data thus the performance is better in this case because we send more data at first and the cwnd grow sooner but we cannot always set cwnd to a big number because it may cause timeout sooner.
### TCP newReno with ssthresh = 10 & cwnd = 1:
  ![newReno,cwnd=1,ssthresh=10](/images/newReno,cwnd=1,ssthresh=10.png "newReno,cwnd=1,ssthresh=10")
As we can see like Reno protocol when we decrease the initial ssthresh to 10, it takes more RTTs to send the data thus the performance is worse in this case because we reach the threshold sooner and decrease the cwnd sooner and can't use the whole cwnd effectively.

- BBR

## SendData

In this function we send the packets. At first we set the packetLost to false and foe every packet we change lastAck and packetLost if needed. If the mode is ProbeRTT, we set the minRTT to the minimum of estimatedRTT and miRTT

```cpp
void TCPReno::sendData()
{
    packetLost = false;
    int lastAck = sw.lastAck;
    for (int i = lastAck; i < lastAck + _cwnd && i < static_cast<int>(packets.size()); i++)
    {
        if (isPacketLost())
            if (!packetLost)
                sw.lastAck++;
            else
                packetLost = true;
    }
}
```

## onePacketLoss
BBR has 5 modes. The STARTUP mode is like slow start in TCP Reno. Congestion window size grows exponentially until packet loss happens and then we decrease some percent of cwnd size and do this with different percentages until no loss happens. If packetLost is true and the mode is startup, we set it to drain and if it is drain, we set it to probeBw.

```cpp
bool TCPReno::onePacketLoss()
{
    if (packetLost)
    {
        if (mode == STARTUP)
        {
            mode = DRAIN;
        }
    }
    else
    {
        if (mode == DRAIN){
            mode = PROBE_BANDWIDTH;
        }
    }    
}
```



## oneRTTUpdate
As mentioned earlier in startup mode, the cwnd increases exponentially. In drain mode we decrease cwnd and set it to a percentage of it. in probBandwidth mode we increase cwnd by multiplying a number between 1 and 2 to the cwnd that we have. In prob_RTT mode we set cwnd to the min cwnd.
```cpp
void TCPReno::oneRTTUpdate()
{
    if (mode == STARTUP)
    {
        _cwnd *= 2;
    }
    else if (mode == DRAIN)
    {
        _cwnd *= (1 - sendProb);
        sendProb -= 0.02;
    }
    else if (mode == PROBE_BANDWIDTH)
    {
    }
    else if (mode == PROBE_RTT)
    {
    }
}
```


# Questions

## Flow Control vs Congestion Control:
The main difference between flow control and congestion control is that, In flow control, rate of traffic received from a sender can be controlled by a receiver. On the other hand, In congestion control, rate of traffic from sender to the network is controlled. Flow Control ensures that a sender does not overwhelm a receiver with too much data too quickly. The goal of flow control is to prevent buffer overflow, which can lead to dropped packets and poor network performance. Congestion control is a technique used to prevent congestion in a network. Congestion occurs when too much data is being sent over a network, and the network becomes overloaded, leading to dropped packets and poor network performance. Both congestion control and flow control regulate the flow of data and prevent packet loss.

## New Reno Algorithm:
Reno algorithm has the problem of "multiple packet loss problem". New Ren uses the concept of partial acknowledgement. When the sender receives the ACK of the first retransmitted packet then it does not consider it a “New ACK” and checks if all the previously transmitted packets of that particular window are ACKed or not. If there are multiple packets lost in the same congestion window then the receiver would have been sending the duplicate ACKs only even after receiving the retransmitted packet. This will make it clear to the sender that all the packets are not reached the receiver and hence sender will not consider that ACK as new. It will consider it a partial ACK because only a partial window is being ACKed not the whole. New Reno considers the new ACK as partial and does not come out of the fast recovery phase. In Reno, if multiple packets are lost from a window, the sender cannot recover from the loss within one RTT because it only retransmits one lost packet per round. New Reno improves this by allowing the sender to retransmit more than one lost packet per RTT.

## BBR Algorithm:
BBR does not rely on packet loss as a signal for network congestion. Instead, it continuously monitors the network's current bottleneck bandwidth and round-trip propagation time (RTT). BBR uses these measurements to build an explicit model of the network. It then adjusts the rate of packet sending based on this model to optimize network utilization and avoid congestion. BBR works by observing how fast a network is already delivering traffic and the latency of current round trips. It then uses that data as input to packet-pacing heuristics that can improve performance.

## Difference between Reno, New Reno and BBR Algorithm:
- BBR is less sensitive to random packet loss, as it does not use packet loss as a signal for network congestion. This makes it more suitable for networks where packet loss can occur independently of congestion, such as wireless networks.
- Reno can only recover from one packet loss per round-trip time, which can lead to suboptimal performance when multiple packets are lost from a window of data. New Reno improves on this by allowing the sender to recover from multiple packet losses per round-trip time
- BBR aims to fully utilize available network bandwidth while keeping latency low. It can achieve higher throughput and lower latency than Reno and New Reno, especially in networks with high bandwidth and high latency (also known as long fat networks).
- Reno and New Reno are loss-based algorithms. They use packet loss as a signal of network congestion. BBR, on the other hand, is a model-based algorithm. It uses measurements of the network's bottleneck bandwidth and RTT to build a model of the network and adjust the sending rate.

## Compare BBR, Reno and NewReno:
 BBR algorithm had the best performance and Reno protocol had the worst performance. Actually in case of raw performance (throughput and latency) BBR is generally superior but in terms of fairness to other flows, TCP Reno and TCP New Reno can be considered better because when BBR flows share the same bottleneck link with flows using other congestion control algorithms. When ssthresh = INF and cwnd = 1, Reno sends the packets in 61 RTTs while New Reno sends them in 54 RTTs.



## Other Congestion Control Algorithms:
- TCP Hybla:
TCP Hybla aims to eliminate penalties to TCP connections that use high-latency terrestrial or satellite radio links. Hybla improvements are based on analytical evaluation of the congestion window dynamics.
- Agile-SD TCP:
Agile-SD is a Linux-based congestion control algorithm which is designed for the real Linux kernel. It is a receiver-side algorithm that employs a loss-based approach using a novel mechanism, called agility factor (AF). to increase the bandwidth utilization over high-speed and short-distance networks (low-BDP networks) such as local area networks or fiber-optic network, especially when the applied buffer size is small.
- Compound TCP:
Compound TCP is a Microsoft implementation of TCP which maintains two different congestion windows simultaneously, with the goal of achieving good performance on LFNs while not impairing fairness.

- CUBIC: CUBIC is another congestion control algorithm that achieve higher throughput and fairness in high-speed and long-distance networks than Reno, New Reno and BBR. It uses a different approach to calculate the congestion window size, which allows it to achieve better performance in high-speed and long-distance networks. It uses a cubic function to determine the congestion window size, which results in a more aggressive increase in the window size during periods of low congestion and a slower increase during periods of high congestion. This allows CUBIC to better utilize available network capacity and achieve higher throughput. Additionally, CUBIC has been designed to be more fair in sharing network resources among competing flows. It uses a fairness mechanism that takes into account the round-trip time (RTT) of the connection, ensuring that connections with longer RTTs do not unfairly dominate the available bandwidth.