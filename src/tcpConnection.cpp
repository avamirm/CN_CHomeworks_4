#include "tcpConnection.hpp"

TCPConnection::TCPConnection(int sthresh){
    cwnd = 1;
    ssthresh = sthresh;
    rtt = 0;
}