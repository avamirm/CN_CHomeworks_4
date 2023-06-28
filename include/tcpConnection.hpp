class TCPConnection {
public:
    TCPConnection(int sthresh);
private:
    int cwnd;
    int ssthresh;
    int rtt;
};