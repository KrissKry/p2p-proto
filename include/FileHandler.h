#ifndef TIN_FileHandler
#define TIN_FileHandler

#include<stdio.h> 
#include<vector>
#include<iostream>
#include<chrono>
#include<fstream>
#include "Resource.h"
#include <string.h>

class FileHandler {
private:
	std::vector <Resource> OwnFileList;
	std::vector <ResourceHeader> NetFileList;
public:


	int AddFile(const char* path, const char* name, const char *ip)
	{

		std::vector <Resource>::iterator it;
		const auto p1 = std::chrono::system_clock::now();
		char* fullPath = new char[strlen(path) + strlen(name) + 7];// 7 bo / .txt i \0
		strcpy(fullPath, path);
		strcat(fullPath, "/");
		strcat(fullPath, name);
		strcat(fullPath, ".txt");
		char c;
		Resource res{};


		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, name))
				return 1;
		}

		std::ifstream file;
		file.open(fullPath);
		if (file.is_open())
		{
			while (EOF != (c = getchar()))
			{
				res.data.push_back(c);
			}
		}
		file.close();
		strcpy(res.header.name, name);
		strcpy(res.header.uuid, ip);
		res.header.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
			p1.time_since_epoch()).count();
		res.header.size = sizeof(res);

		OwnFileList.push_back(res);
		NetFileList.push_back(res.header);
		return 1;
	}

	int createFile(Resource resource, const char* ip)
	{

		std::vector <Resource>::iterator it;
		const auto p1 = std::chrono::system_clock::now();


		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, resource.header.name))
				return 1;
		}

		std::ofstream file;
		file.open(resource.header.name);
		if (file.is_open())
		{
			for(int i=0; i< resource.data.size();i++)
			{
				file<<resource.data[i];
			}
		}
		file.close();
	
		OwnFileList.push_back(resource);
		NetFileList.push_back(resource.header);
		return 1;
	}

	int deleteownFile(ResourceHeader rh)
	{
		std::vector <Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, rh.name))
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

	int deleteFromNetList(ResourceHeader rh)
	{
		std::vector <ResourceHeader>::iterator it;
		for (it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			if (strcmp(it->name, rh.name))
			{
				NetFileList.erase(it);
				return 0;
			}

		}
		return 1;
	}

	int NewFileInfo(ResourceHeader rh, const char* ip)
	{
		NetFileList.push_back(rh);
		return 0;

	}

	std::vector<Resource> getOwnFileList()
	{
		return OwnFileList;
	}

	std::vector<ResourceHeader> getNetFileList()
	{
		return NetFileList;
	}

	void ShowOwnFiles()
	{
		std::vector <Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			std::cout << it->header.name <<std::endl;
		}
	}

	void ShowNetFiles()
	{
		std::vector <ResourceHeader>::iterator it;
		for (it= NetFileList.begin(); it != NetFileList.end(); it++)
		{
			std::cout << it->name << std::endl;
		}
	}


};
#endif