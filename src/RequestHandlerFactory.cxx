#include <PUHW/Catalog/RequestHandlerFactory.h>
#include <PUHW/Catalog/MonitorsListHandler.h>
#include <PUHW/Catalog/BadURIHandler.h>
#include <PUHW/Catalog/MonitorHandler.h>
#include <Poco/URI.h>
#include <Poco/Logger.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <regex>

using ::std::regex;
using ::std::regex_match;

namespace PUHW {
	namespace Catalog {
		::Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const ::Poco::Net::HTTPServerRequest& request) {
			/* Do NOT try to free returned RequestHandler. Using smart pointers here will lead to segmentation fault */
			::Poco::URI requestURI(request.getURI());
			const ::std::string reqURIpath = requestURI.getPath();
			regex monitor(R"(/monitors/[a-zA-Z][a-zA-Z0-9_-]*)");
			if(reqURIpath == "/monitors")
				return new MonitorsListHandler();
			//else if(regex_match(reqURIpath,monitor)) {
			//	return new MonitorHandler();
			//}
			else return new BadURIHandler();
		}
	} // namespace Catalog
} // namespace PUHW
