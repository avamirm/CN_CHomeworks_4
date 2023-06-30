#include <iostream>
#include <vector>

#include "tcpConnection.hpp"
#include "tcpReno.hpp"
#include "tcpNewReno.hpp"
#include "BBR.hpp"

int main()
{
    std::vector<int> data(DATA_SIZE);
    for (int i = 0; i < DATA_SIZE; i++)
    {
        data[i] = i;
    }

    // cout << "-------------------RENO-----------------------" << endl;
    // TCPReno tcp1(data);
    // tcp1.run();

    // cout << "-------------------NEW Reno-------------------" << endl;
    // TCPNewReno tcp2(data);
    // tcp2.run();

    cout << "-------------------BBR------------------------" << endl;
    BBR tcp3(data);
    tcp3.run();
    return 0;
}