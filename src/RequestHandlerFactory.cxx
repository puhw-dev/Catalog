#include <PUHW/Catalog/RequestHandlerFactory.h>
#include <PUHW/Catalog/MonitorsListHandler.h>
#include <PUHW/Catalog/BadURIHandler.h>
#include <Poco/URI.h>
#include <Poco/Logger.h>
#include <Poco/Net/HTTPServerRequest.h>

namespace PUHW {
	namespace Catalog {
		::Poco::Net::HTTPRequestHandler* RequestHandlerFactory::createRequestHandler(const ::Poco::Net::HTTPServerRequest& request) {
			/* Do NOT try to free returned RequestHandler. Using smart pointers here will lead to segmentation fault */
			::Poco::URI requestURI(request.getURI());
			const ::std::string reqURIpath = requestURI.getPath();
			if(reqURIpath == "/monitors")
				return new MonitorsListHandler();
			else return new BadURIHandler();
		}
	} // namespace Catalog
} // namespace PUHW
