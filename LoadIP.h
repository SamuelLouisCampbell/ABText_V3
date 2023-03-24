#pragma once
#include <fstream>
#include <string>

class LoadIPData
{
public:
	LoadIPData(const char* fileName)
	{
		std::string line;
		std::string::size_type n;
		std::ifstream defFile(fileName);
		if (defFile.is_open())
		{
			//serverPort
			std::getline(defFile, line);
			n = line.find("#");
			std::string substr = line.substr(n + 1, line.size());
			serverPort = std::stoi(substr);
			//IP
			std::getline(defFile, line);
			n = line.find("#");
			clientIP = line.substr(n + 1, line.size());
			//close out
			defFile.close();
			fileReadOk = true;
		}
		else
		{
			fileReadOk = false;
		}
	}
	unsigned short GetServerPort() const
	{
		return unsigned short(serverPort);
	}
	const char* GetIP() const
	{
		return clientIP.c_str();
	}
	void SetPortToDefault()
	{
		serverPort = 5000U;
	}
	void SetIPToDefault()
	{
		clientIP = "127.0.0.1";
	}
	bool FileReadOK() const
	{
		return fileReadOk;
	}
private:
	bool fileReadOk = false;
	size_t serverPort;
	std::string clientIP;
};