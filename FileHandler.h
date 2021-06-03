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


	int AddFile(const char* path, const char* name, const char* ip)
	{

		const auto p1 = std::chrono::system_clock::now();
		char c;
		Resource res{};


		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (auto it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, name) == 0)
				return 1;
		}

		std::ifstream file;
		file.open(path);
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
		return 0;
	}

	int createFile(Resource resource, const char* ip)
	{

		
		const auto p1 = std::chrono::system_clock::now();


		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (auto it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, resource.header.name) == 0)
				return 1;
		}

		std::ofstream file;
		file.open(resource.header.name);
		if (file.is_open())
		{
			for (int i = 0; i < resource.data.size(); i++)
			{
				file << resource.data[i];
			}
		}
		file.close();

		OwnFileList.push_back(resource);
		NetFileList.push_back(resource.header);
		return 1;
	}

	// metoda nie jest bezpieczna ale jeœli wybieramy z listy to whatever
	Resource getFile(const char * name)
	{
		for (auto it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, name) == 0)
			{
				return *it;

			}
		}

	}

	//metoda do usuwania dla twórcy pliku od razu usuwa z dwóch list
	int deleteOwnFile(ResourceHeader rh)
	{
		std::vector <Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, rh.name) == 0)
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

	// metoda do usuwania pliku jeœli nie jesteœmy twórc¹ a mamy go na swoim dysku
	int deleteNotOwnFile(ResourceHeader rh)
	{
		std::vector <Resource>::iterator it;
		for (it = OwnFileList.begin(); it != OwnFileList.end(); it++)
		{
			if (strcmp(it->header.name, rh.name) == 0)
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
		std::vector <ResourceHeader>::iterator it;
		for (it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			if (strcmp(it->name, rh.name) == 0)
			{
				NetFileList.erase(it);
				return 0;
			}

		}
		return 1;
	}

	bool isOwner(ResourceHeader rh, const char* ip)
	{
		if (strcmp(rh.uuid, ip) == 0)
		{
			return true;
		}

		return false;
	}

	ResourceHeader NewFileInfo(const char * name)
	{
		for (auto it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			if (strcmp(it->name, name) == 0)
			{
				return *it;

			}
		}

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
			std::cout << it->header.name << std::endl;
		}
	}

	void ShowNetFiles()
	{
		std::vector <ResourceHeader>::iterator it;
		for (it = NetFileList.begin(); it != NetFileList.end(); it++)
		{
			std::cout << it->name << std::endl;
		}
	}


};
#endif