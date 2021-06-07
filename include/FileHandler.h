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

public:
	ResourceHeader AddFile(const char *path, const char *name, struct in_addr ip)
	{

		const auto p1 = std::chrono::system_clock::now();
		char c;
		Resource res{};

		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (auto &it : OwnFileList)
		{
			if (strcmp(it.header.name, name) == 0)
				return res.header;
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

		OwnFileList.push_back(res);
		NetFileList.emplace_back(ip, res.header);
		return res.header;
	}

	int createFile(Resource resource)
	{
		const auto p1 = std::chrono::system_clock::now();

		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (auto &it : OwnFileList)
		{
			if (strcmp(it.header.name, resource.header.name) == 0)
				return 1;
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

		OwnFileList.push_back(resource);
		return 0;
	}

	// metoda nie jest bezpieczna ale je�li wybieramy z listy to whatever
	Resource getFile(const char *name)
	{
		for (auto &it : OwnFileList)
		{
			if (strcmp(it.header.name, name) == 0)
			{
				return it;
			}
		}
		return Resource{};
	}

	//metoda do usuwania dla tw�rcy pliku od razu usuwa z dw�ch list
	int deleteOwnFile(ResourceHeader rh)
	{
		std::vector<Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, rh.name) == 0 && rh.uuid == it->header.uuid)
			{
				if (remove(rh.name) != 0)
					return 1;
				OwnFileList.erase(it);
				deleteFromNetList(rh);
				return 0;
			}
		}
		return 1;
	}

	// metoda do usuwania pliku je�li nie jeste�my tw�rc� a mamy go na swoim dysku
	int deleteNotOwnFile(ResourceHeader rh)
	{
		std::vector<Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, rh.name) == 0 && rh.uuid == it->header.uuid)
			{
				if (remove(rh.name) != 0)
					return 1;
				OwnFileList.erase(it);
				return 0;
			}
		}
		return 1;
	}

	int deleteFromNetList(ResourceHeader rh)
	{
		std::vector<std::pair<struct in_addr, ResourceHeader>>::iterator it;
		for (it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			if (strcmp(it->second.name, rh.name) == 0 && rh.uuid == it->second.uuid)
			{
				NetFileList.erase(it);
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

	ResourceHeader GetFileInfo(const char *name)
	{
		for (auto &it : NetFileList)
		{
			if (strcmp(it.second.name, name) == 0)
			{
				return it.second;
			}
		}
		return ResourceHeader{};
	}

	int NewFileInfo(ResourceHeader header, struct in_addr ip)
	{
		NetFileList.emplace_back(ip, header);
		return 0;
	}

	std::vector<Resource> getOwnFileList()
	{
		return OwnFileList;
	}

	std::vector<std::pair<struct in_addr, ResourceHeader>> getNetFileList()
	{
		return NetFileList;
	}

	void ShowOwnFiles()
	{
		std::vector<Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			std::cout << it->header.name << std::endl;
		}
	}

	void ShowNetFiles()
	{
		std::vector<std::pair<struct in_addr, ResourceHeader>>::iterator it;
		for (it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			std::cout << it->second.name << std::endl;
		}
	}
};
#endif