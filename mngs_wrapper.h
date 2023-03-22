#pragma once
#include "mongoose.h"
#include <string>
#include <iostream>
#include <sstream>

static std::stringstream ss;

class MgsWrapper
{
public:
	void init(const std::string ipAddress, const int port);
	void update(const int pollTimeMs);
	std::string getStream();
	void close();

private:
	struct mg_mgr mgr;

private:

	static void fn(mg_connection* c, int ev, void* ev_data, void* fn_data);
	static std::string convertToString(char* a, int size);

};

