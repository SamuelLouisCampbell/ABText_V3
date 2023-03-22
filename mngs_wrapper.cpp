#include "mngs_wrapper.h"

void MgsWrapper::init(const std::string ipAddress, const int port)
{
	std::string ipAddr = ipAddress + ":" + std::to_string(port);
	mg_mgr_init(&mgr);
	mg_http_listen(&mgr, ipAddr.c_str(), fn, &mgr);
	std::cout << "Listening to: " << ipAddr << std::endl;
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
		char buffer[256] = "";
		int len = mg_http_get_var(&hm->body, "say", buffer, sizeof(buffer));
		if (len > 0)
		{
			std::string m;
			m = convertToString(buffer, len);
			//std::cout << m;
			ss.str(std::string());

			ss << m;
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

