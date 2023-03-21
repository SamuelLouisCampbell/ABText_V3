#include "mngs_wrapper.h"
#include <iostream>

void MongooseWrapper::Init(const std::string& ipAddr, const int& port)
{
	std::string IPV4Addr = ipAddr + ":" + std::to_string(port);
	mg_mgr_init(&mgr);                                    
	mg_http_listen(&mgr, IPV4Addr.c_str(), fn, &mgr);
	
	std::cout << "Listening to: " << IPV4Addr << std::endl;
}

void MongooseWrapper::Update(int timeMS)
{
	//Event Loop
	mg_mgr_poll(&mgr, timeMS);
}

void MongooseWrapper::Close()
{
	mg_mgr_free(&mgr);
}

std::string MongooseWrapper::GetString()
{
	return theMessage;
}



void MongooseWrapper::fn(mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	struct mg_http_serve_opts opts;// = { .root_dir = "." };  // Serve local dir require C++20
	opts.extra_headers = NULL;
	opts.fs = NULL;
	opts.mime_types = NULL;
	opts.root_dir = ".";
	opts.ssi_pattern = NULL;

	if (ev == MG_EV_HTTP_MSG)
	{
		//std::cout << "MG_EV_HTTP_MSG Event Logged" << std::endl;
		mg_http_serve_dir(c, static_cast<mg_http_message*>(ev_data), &opts);

		struct mg_http_message* hm = static_cast<mg_http_message*>(ev_data);
		char buffer[256] = "";

		//
		int len = mg_http_get_var(&hm->body, "say", buffer, sizeof(buffer));
		if (len > 0)
		{
			std::cout << convertToString(buffer, len) << std::endl;
			//theMessage = convertToString(buffer, len);
		}
		
	}
}

std::string MongooseWrapper::convertToString(char* a, int size)
{
	std::string s = "";
	for (int i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}
