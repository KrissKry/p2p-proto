#ifndef GETUP_H
#define GETUP_H

#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <iostream>

struct in_addr getMyIp()
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;
    struct in_addr chosenIp;

    getifaddrs(&ifap);
    int i = -1;
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
        {
            i++;
            sa = (struct sockaddr_in *)ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            printf("%d Interface: %s\tAddress: %s\n", i, ifa->ifa_name, addr);
        }
    }

    int choise;
    std::cout << "Choose your interface" << std::endl;
    std::cin >> choise;
    while (std::cin.fail() || choise > i || choise < 0)
    {

        std::cout << "Error wrong input " << choise << std::endl;
        std::cout << "Choose your interface" << std::endl;
        std::cin.clear();
        std::cin.ignore();
        std::cin >> choise;
    }
    int j = 0;
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET)
        {
            if (j == choise)
            {
                sa = (struct sockaddr_in *)ifa->ifa_addr;
                addr = inet_ntoa(sa->sin_addr);
                chosenIp = sa->sin_addr;
            }

            j++;
        }
    }
    std::cout << "Chosen ip: " << inet_ntoa(chosenIp) << std::endl;

    freeifaddrs(ifap);

    return chosenIp;
}

#endif