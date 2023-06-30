#include "BBR.hpp"

float BBR::lostProb(int num, int start, int end)
{
    return (pow(10 * end, static_cast<float>(num - start) / (end - start)) - 1) / (10 * end - 1);
}

bool BBR::isPacketLost()
{
    int prob = static_cast<int>(advwnd * lostProb(_cwnd, 1, advwnd));
    bool isPacketLost = (rand() % advwnd + 1) > prob;
    return isPacketLost;
}

void BBR::sendData()
{
    packetLost = false;
    int lastAck = sw.lastAck;
    for (int i = lastAck; i < lastAck + _cwnd && i < static_cast<int>(packets.size()); i++)
    {
        if (isPacketLost())
        {
            if (!packetLost)
                sw.lastAck++;
        }
        else
            packetLost = true;
    }

    if (mode == PROBE_RTT)
    {
        float rand1 = static_cast<float>(30 + rand() % (200 - 30 + 1));
        float rand2 = (rand() % 100 + 1) / 100;
        estRtt = static_cast<float>(rand1 + rand2);
        minRtt = min(minRtt, estRtt);
    }
    else if (mode == PROBE_BANDWIDTH)
    {
        isbwIncrease = !isbwIncrease;
    }
}

bool BBR::onePacketLoss()
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
        if (mode == DRAIN)
        {
            mode = PROBE_BANDWIDTH;
        }
    }
    return false;
}

void BBR::oneRTTUpdate()
{
    if (mode != DRAIN && mode != STARTUP)
    {
        probRttCounter--;
        if (probRttCounter == 0)
        {
            probRttCounter = INTERVAL + WAIT_RTT;
            minRtt = 1000;
            actualCwnd = _cwnd;
            mode = PROBE_RTT;
        }

        if (probRttCounter == INTERVAL)
        {
            _cwnd = actualCwnd;
            mode = PROBE_BANDWIDTH;
            return;
        }
    }

    if (mode == STARTUP)
    {
        _cwnd *= 2;
    }

    else if (mode == DRAIN)
    {
        _cwnd *= (1 - sendProb);
        sendProb -= 0.02;
        actualCwnd = _cwnd;
    }

    else if (mode == PROBE_BANDWIDTH)
    {
        if (isbwIncrease)
        {
            _cwnd = actualCwnd;
            _cwnd *= 1.05;
        }
        else
        {
            _cwnd *= 0.95;
        }
    }

    else if (mode == PROBE_RTT)
    {
        _cwnd = 25;
    }
}