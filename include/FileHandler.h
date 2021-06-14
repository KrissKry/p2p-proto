#ifndef TIN_FileHandler
#define TIN_FileHandler

#include <cstdio>
#include <vector>
#include <iostream>
#include <chrono>
#include <fstream>
#include "Resource.h"
#include <cstring>
#include <netinet/in.h>
#include <arpa/inet.h>

class FileHandler
{
private:
    std::vector<Resource> OwnFileList;
    std::vector<std::pair<struct in_addr, ResourceHeader>> NetFileList;
    std::mutex own_tx;
    std::mutex net_tx;

public:
    ResourceHeader addFile(const char *path, const char *name, struct in_addr ip)
    {

        const auto p1 = std::chrono::system_clock::now();
        char c;
        Resource res{};

        //sprawdzenie czy nie ma pliku o takiej samej nazwie
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (auto &it : OwnFileList)
            {
                if (strcmp(it.header.name, name) == 0)
                    return res.header;
            }
        }
        {
            std::unique_lock<std::mutex> lock(net_tx);
            for (auto &it : NetFileList)
            {
                if (strcmp(it.second.name, name) == 0)
                    return res.header;
            }
        }

        std::ifstream file;
        file.open(path);
        if (file.is_open())
        {
            while (file.get(c))
            {
                res.data.push_back(c);
            }
            file.close();
        }
        else
        {
            return res.header;
        }

        strcpy(res.header.name, name);
        strcpy(res.header.uuid, inet_ntoa(ip));
        res.header.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
                                   p1.time_since_epoch())
                                   .count();
        res.header.size = res.data.size();

        {
            std::unique_lock<std::mutex> lock(own_tx);
            OwnFileList.push_back(res);
        }
        {
            std::unique_lock<std::mutex> lock(net_tx);
            NetFileList.emplace_back(ip, res.header);
        }
        return res.header;
    }

    int createFile(Resource resource, struct in_addr ip)
    {
        const auto p1 = std::chrono::system_clock::now();

        //sprawdzenie czy nie ma pliku o takiej samej nazwie
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (auto &it : OwnFileList)
            {
                if (strcmp(it.header.name, resource.header.name) == 0)
                    return 1;
            }
        }

        std::ofstream file;
        file.open(resource.header.name);
        if (file.is_open())
        {
            for (unsigned char i : resource.data)
            {
                file << i;
            }
        }
        file.close();

        {
            std::unique_lock<std::mutex> lock(own_tx);
            OwnFileList.push_back(resource);
        }
        {
            std::unique_lock<std::mutex> lock(net_tx);
            NetFileList.emplace_back(ip, resource.header);
        }
        return 0;
    }

    // metoda nie jest bezpieczna ale je�li wybieramy z listy to whatever
    Resource getFile(const char *name)
    {
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (auto &it : OwnFileList)
            {
                if (strcmp(it.header.name, name) == 0)
                {
                    return it;
                }
            }
        }
        return Resource{};
    }

    //metoda do usuwania dla tw�rcy pliku od razu usuwa z dw�ch list
    int deleteOwnFile(ResourceHeader rh)
    {
        std::vector<Resource>::iterator it;
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
            {
                if (strcmp(it->header.name, rh.name) == 0)
                {
                    // if (remove(rh.name) != 0)
                    // 	return 1;
                    OwnFileList.erase(it);
                    return 0;
                }
            }
        }
        return 1;
    }

    // metoda do usuwania pliku je�li nie jeste�my tw�rc� a mamy go na swoim dysku
    int deleteNotOwnFile(ResourceHeader rh)
    {
        std::vector<Resource>::iterator it;
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
            {
                if (strcmp(it->header.name, rh.name) == 0)
                {
                    // if (remove(rh.name) != 0)
                    // 	return 1;
                    OwnFileList.erase(it);
                    return 0;
                }
            }
        }
        return 1;
    }

    int deleteFromNetList(ResourceHeader rh, struct in_addr ip)
    {
        {
            std::unique_lock<std::mutex> lock(net_tx);
            auto it = NetFileList.begin();
            while (it != NetFileList.end())
            {
                if (strcmp(it->second.name, rh.name) == 0 && (it->first.s_addr == ip.s_addr))
                {
                    // if (INFO_LOG) std::cout << "[I] Deleting file " << rh.name << " from netlist\n";
                    it = NetFileList.erase(it);
                }
                else
                {
                    ++it;
                }
            }
        }
        return 0;
    }

    bool isOwner(ResourceHeader rh, const char *ip)
    {
        if (strcmp(rh.uuid, ip) == 0)
        {
            return true;
        }

        return false;
    }

    ResourceHeader getFileInfo(const char *name)
    {
        {
            std::unique_lock<std::mutex> lock(net_tx);
            for (auto &it : NetFileList)
            {
                if (strcmp(it.second.name, name) == 0)
                {
                    return it.second;
                }
            }
        }
        return ResourceHeader{};
    }

    int newFileInfo(ResourceHeader header, struct in_addr ip)
    {
        {
            std::unique_lock<std::mutex> lock(net_tx);
            for (auto &it : NetFileList)
            {
                if (strcmp(it.second.name, header.name) == 0 && (it.first.s_addr == ip.s_addr))
                {   
                    return -1;
                }
            }
            // name || source ip || our ip

            if (INFO_LOG) std::cout << "[I] FH:: " << header.name << " || " << header.uuid << " || " << inet_ntoa(ip) << "\n";
            
            NetFileList.emplace_back(ip, header);
            std::cout << "[I] New file in network: " << header.name << "\n\n";
        }
        return 0;
    }

    std::vector<Resource> getOwnFileList()
    {
        std::unique_lock<std::mutex> lock(own_tx);
        return OwnFileList;
    }

    std::vector<std::pair<struct in_addr, ResourceHeader>> getNetFileList()
    {
        std::unique_lock<std::mutex> lock(net_tx);
        return NetFileList;
    }

    void showOwnFiles()
    {
        if (INFO_LOG)
            std::cout << "[I] FH:: __NAME__\n";
        std::vector<Resource>::iterator it;
        {
            std::unique_lock<std::mutex> lock(own_tx);
            for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
            {
                std::cout << "   " << it->header.name << "\n";
            }
        }
    }

    void showNetFiles()
    {
        if (INFO_LOG)
            std::cout << "FH:: __NAME__ || ___OWNER_IP___ || ___COPY_IP___\n";

        std::vector<std::pair<struct in_addr, ResourceHeader>>::iterator it;
        {
            std::unique_lock<std::mutex> lock(net_tx);
            for (it = NetFileList.begin(); it != NetFileList.end(); it++)
            {
                std::cout << it->second.name << " || " << it->second.uuid << " || " << it->first.s_addr << std::endl;
            }
        }
    }
};
#endif