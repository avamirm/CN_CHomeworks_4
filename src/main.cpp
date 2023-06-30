#include <iostream>
#include <vector>

#include "tcpConnection.hpp"
#include "tcpReno.hpp"

const int DATA_SIZE = 100000;

int main()
{
    std::vector<int> data(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; i++)
    {
        data[i] = i;
    }
    TCPReno tcp(data);
    tcp.run();
    return 0;
}