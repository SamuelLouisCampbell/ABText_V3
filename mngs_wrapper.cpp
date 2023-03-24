#include "mngs_wrapper.h"

void MgsWrapper::init()
{
	LoadIPData ipData("defaults.txt");
	if (ipData.FileReadOK())
	{
		std::string ipAddr = ipData.GetIP() + ':' + std::to_string(ipData.GetServerPort());
		mg_mgr_init(&mgr);
		mg_http_listen(&mgr, ipAddr.c_str(), fn, &mgr);
		std::cout << "Listening to: " << ipAddr << std::endl;
	}
	else
	{
		//USE LOCALHOST
		std::string ipAddr = "127.0.0.1:8000";
		mg_mgr_init(&mgr);
		mg_http_listen(&mgr, ipAddr.c_str(), fn, &mgr);
		std::cout << "Listening to: " << ipAddr << std::endl;
	}
}

void MgsWrapper::update(const int pollTimeMs)
{
	mg_mgr_poll(&mgr, pollTimeMs);
}

std::string MgsWrapper::getStream()
{
	std::cout << ss.str();
	return ss.str();
}

void MgsWrapper::close()
{
	mg_mgr_free(&mgr);
}

void MgsWrapper::fn(mg_connection* c, int ev, void* ev_data, void* fn_data)
{
	struct mg_http_serve_opts opts;
	opts.extra_headers = NULL;
	opts.fs = NULL;
	opts.mime_types = NULL;
	opts.page404 = NULL;
	opts.root_dir = ".";
	opts.ssi_pattern = NULL;

	if (ev == MG_EV_HTTP_MSG)
	{
		mg_http_serve_dir(c, static_cast<mg_http_message*>(ev_data), &opts);
		struct mg_http_message* hm = static_cast<mg_http_message*>(ev_data);
		char bufferSmall[1024] = "";
		int small = mg_http_get_var(&hm->body, "small", bufferSmall, sizeof(bufferSmall));
		if (small > 0)
		{
			std::string m;
			m = convertToString(bufferSmall, small);
			ss.str(std::string());
			ss << m;
			m.erase(0, 8);
			std::string json = "{smallstring: " + m + "}";
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json.c_str());
			////test
			//char* json_1 = mg_mprintf("{%Q:%d}", "name", 123);
			//mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json_1);
			//free(json_1);
		}
		char bufferLarge[128] = "";
		int large = mg_http_get_var(&hm->body, "large", bufferLarge, sizeof(bufferLarge));
		if (large > 0)
		{
			std::string m;
			m = convertToString(bufferLarge, large);
			ss.str(std::string());
			ss << m;
			m.erase(0, 8);
			std::string json = "{largestring," + m + "}";
			mg_http_reply(c, 200, "Content-Type: application/json\r\n", "%s\n", json.c_str());
		}
	}
}

std::string MgsWrapper::convertToString(char* a, int size)
{
	std::string s = "";
	for (int i = 0; i < size; i++) {
		s = s + a[i];
	}
	return s;
}

