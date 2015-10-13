#include "curl_wrapper.h"
#include <curl/curl.h>
#include <sstream>

class CurlInit {
public:
	CurlInit() { curl_global_init(CURL_GLOBAL_ALL);	}

	~CurlInit() { curl_global_cleanup(); }
};

void Curl::Deleter::operator()(CURL * curl) const
{
	curl_easy_cleanup(curl);
}


Curl::Curl()
{
	static CurlInit init{};
	auto curlPtr = curl_easy_init();
	if( ! curlPtr ) throw "CURL failed to init";
	curl.reset(curlPtr);
}

void boom(std::string func, CURLcode code)
{
	throw "invalid code when calling " + func + " code = " + std::to_string(code);
}

// callback function writes data to a std::ostream
static size_t writeToStream(void * buf, size_t size, size_t nmemb, void * userp)
{
	if( ! userp ) return 0;

	auto & os = *static_cast<std::ostream *>(userp);
	auto len = size * nmemb;
	os.write(static_cast<char *>(buf),len);

	return len;
}

std::string Curl::fetchUrl(std::string const & url)
{
	auto setOpt = [this](auto option, auto arg) {
		auto code = curl_easy_setopt(curl.get(),option,arg);
		if( code != CURLE_OK) {
			boom("curl_easy_setopt",code);
		}
	};
	std::ostringstream os;
	setOpt(CURLOPT_WRITEFUNCTION,&writeToStream);
	setOpt(CURLOPT_WRITEDATA,&os);
	setOpt(CURLOPT_NOPROGRESS,1L);
	setOpt(CURLOPT_FOLLOWLOCATION,1L);
	setOpt(CURLOPT_TIMEOUT,30L);
	setOpt(CURLOPT_URL,url.c_str());
	auto code = curl_easy_perform(curl.get());
	if( code != CURLE_OK) {
		boom("curl_easy_setopt",code);
	}

	return os.str();
}
