#include <PUHW/Catalog/RequestHandlerFactory.h>
#include <PUHW/Catalog/MonitorsListHandler.h>
#include <PUHW/Catalog/BadURIHandler.h>
#include <PUHW/Catalog/MonitorHandler.h>
#include <Poco/URI.h>
#include <Poco/Logger.h>
#include <Poco/Net/HTTPServerRequest.h>

#define BOOST_REGEXP // comment out for libstdc++ regex

#ifdef BOOST_REGEXP
	#include <boost/regex.hpp>
	using ::boost::regex;
	using ::boost::regex_match;
	using ::boost::smatch;
#else
	#include <regex>
	using ::std::regex;
	using ::std::regex_match;
	using ::std::smatch;
#endif



namespace PUHW {
	namespace Catalog {
		::Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const ::Poco::Net::HTTPServerRequest& request) {
			/* Do NOT try to free returned RequestHandler. Using smart pointers here will lead to segmentation fault */
			::Poco::URI requestURI(request.getURI());
			const ::std::string reqURIpath = requestURI.getPath();
			regex monitor(R"(/monitors/([a-zA-Z][a-zA-Z0-9_-]*))");
			smatch match;
			if(reqURIpath == "/monitors")
				return new MonitorsListHandler();
			else if(regex_match(reqURIpath,match,monitor)) {
				return new MonitorHandler(match[1].str());
			}
			else return new BadURIHandler();
		}
	} // namespace Catalog
} // namespace PUHW
