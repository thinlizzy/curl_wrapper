#ifndef DIEGO_CURL_WRAPPER_H_
#define DIEGO_CURL_WRAPPER_H_

#include <string>
#include <memory>

typedef void CURL;

class Curl {
	struct Deleter {
		void operator()(CURL * curl) const;
	};
	std::unique_ptr<CURL, Deleter> curl;
public:
	Curl();
	Curl(Curl &&) = default;
	std::string fetchUrl(std::string const & url);
};

#endif
