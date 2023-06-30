#include "tcpConnection.hpp"

TCPConnection::TCPConnection(vector<int> &data)
{
    packetize(data);
}

TCPConnection::TCPConnection(vector<int> &data, int cwnd, int ssthresh)
    : TCPConnection(data)
{
    _cwnd = cwnd;
    _ssthresh = ssthresh;
}

int TCPConnection::getWindowSize()
{
    return min(_cwnd, advwnd);
}

void TCPConnection::packetize(vector<int> &data)
{
    for (int i = 0; i < data.size(); i++)
    {
        Packet p = {data[i], false};
        packets.push_back(p);
    }
}

void TCPConnection::printStatus()
{
    cout << "cwnd: " << _cwnd << " ssthresh: " << _ssthresh << " rtt: " << _rtt << endl;
}

void TCPConnection::run()
{
    while (sw.lastAck != static_cast<int>(packets.size()))
    {
        printStatus();
        sendData();
        if (!onePacketLoss())
            oneRTTUpdate();
        _rtt++;
    }
}