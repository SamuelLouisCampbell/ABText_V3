#pragma once
#include "mongoose.h"
#include <string>

class MongooseWrapper {
public:
	void Init(const std::string& ipAddr, const int& port);
	void Update(int timeMs);
	void Close();
    std::string GetString();
private:
	static void fn(struct mg_connection* c, int ev, void* ev_data, void* fn_data);
	static std::string convertToString(char* a, int size);
	struct mg_mgr mgr;
	std::string theMessage;
};