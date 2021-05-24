#ifndef FileHandler
#define FileHandler

#include<stdio.h> 
#include<vector>
#include<iostream>
#include<chrono>
#include<fstream>
#include"resource.h"

class FileHandler {
private:
	std::vector <Resource> FileList;
	int createFile( char*  fileName,  char* data, char* uuid)
	{
		
		std::vector <Resource>::iterator it;
		const auto p1 = std::chrono::system_clock::now();
		Resource res{};


		//sprawdzenie czy nie ma pliku o takiej samej nazwie
		for (it = FileList.begin(); it != FileList.end(); it++)
		{
			if (strcmp(it->header.name, fileName))
				return 0;
		}

		std::ifstream file(fileName);
		if (!file.good())
			return 0;

		strcpy(res.header.name, fileName);
		strcpy(res.data, data);
		strcpy(res.header.uuid, uuid);
		res.header.timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(
			p1.time_since_epoch()).count();
		res.header.size = sizeof(res);

		FileList.push_back(res);
		return 1;
	}

	int deleteFile( char* fileName)
	{
		std::vector <Resource>::iterator it;
		for (it = FileList.begin(); it != FileList.end(); it++)
		{
			if (strcmp(it->header.name, fileName))
			{
				if (remove(fileName) != 0)
					return 0; 
				FileList.erase(it);
				return 1;
			}
			
		}
		return 0;

	}


public:
	std::vector<Resource> getFileList()
	{
		return FileList;
	}

};
#endif