#include <iostream>
#include <vector>

#include "tcpConnection.hpp"
#include "tcpReno.hpp"
#include "tcpNewReno.hpp"

int main()
{
    std::vector<int> data(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; i++)
    {
        data[i] = i;
    }

    cout << "-------------------NEW Reno-------------------" << endl;
    TCPNewReno tcp(data);
    tcp.run();
    cout << "-------------------RENO-----------------------" << endl;
    TCPReno tcp2(data);
    tcp2.run();
    return 0;
}