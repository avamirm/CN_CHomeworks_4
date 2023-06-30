#include "tcpNewReno.hpp"

float TCPNewReno::lostProb(int num, int start, int end)
{
    return (pow(10 * end, static_cast<float>(num - start) / (end - start)) - 1) / (10 * end - 1);
}

bool TCPNewReno::isPacketLost(){
    int prob = static_cast<int>(advwnd * lostProb(_cwnd, 1, advwnd));
    bool isPacketLost = (rand() % advwnd + 1) > prob;
    return isPacketLost;
}

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

bool TCPNewReno::onePacketLoss()
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

void TCPNewReno::oneRTTUpdate()
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