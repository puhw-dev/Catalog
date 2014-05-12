#include <PUHW/Catalog/MonitorHandler.h>
#include <PUHW/Catalog/utils.h>
#include <PUHW/Catalog/CatalogServer.h>
//#include <PUHW/Catalog/DataTypes.h>
#include <Poco/URI.h>
#include <Poco/StreamCopier.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>
#include "Poco/Data/Common.h"
#include "Poco/Data/SQLite/Connector.h"
#include <json/json.h>
#include <sstream>

using ::Poco::Data::into;
using ::Poco::Data::now;
using ::Poco::Data::use;

namespace PUHW {
	namespace Catalog {
		void MonitorHandler::handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) {
			const ::std::string method(request.getMethod());
			::Poco::URI requestURI(request.getURI());
			::std::string requestPath = requestURI.getPath();
			::std::string monitorId = requestPath.substr(requestPath.find_last_of("/"));
			::std::string req(method);
			req.append(" /monitors/").append(monitorId);
			::std::string msg("MonitorsListHandler is handling ");
			msg.append(req);
			logBcast("information",msg.c_str());
			if(method == "GET") {
				response.send();
			} // handling GET
			else if(method == "POST") {
				response.send();
			} // handling POST
			else if(method == "DELETE") {
				response.send();
			} // handling DELETE
			else {
				response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED,"Requested URI does not handle this method (it handles GET and POST requests only).");
				logBcast("information","Unsupported method");
				response.send();
			}
		}
	} // namespace Catalog
} // namespace PUHW