#include <PUHW/Catalog/SearchingHandler.h>
#include <PUHW/Catalog/utils.h>
#include <PUHW/Catalog/CatalogServer.h>
#include <PUHW/Catalog/DataTypes.h>
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
		void SearchingHandler::handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) {
			const ::std::string method(request.getMethod());
			::Poco::URI requestURI(request.getURI());
			::std::string requestPathAndQuery = requestURI.getPathAndQuery();
			::std::string req(method);
			req.append.append(" ").append(requestPathAndQuery);
			::std::string msg("SearchingHandler is handling ");
			msg.append(req).append(" with searched phrase \"").append(searchedPhrase).append("\".");
			logBcast("information",msg.c_str());
			msg.clear();
			::Poco::Net::HTTPResponse::HTTPStatus status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK;
			::Poco::Data::Session dbsession("SQLite", (dynamic_cast<CatalogServer&>(::Poco::Util::ServerApplication::instance())).getDBPath());
			if(method == "GET") {
				Monitors monitors;
				select << "SELECT * FROM MONITORS WHERE name LIKE '%:name%'", into(monitors), use(searchedPhrase), now;
				response.setChunkedTransferEncoding(true);
				response.setContentType("text/plain"); // or "application/json" ?
				response.setStatus(status);
				::std::ostream& ostr = response.send();
				if(monitors.empty()) {
					ostr << "[]"; // according to specification empty list is returned if no results found
				}
				else { // there are results of the query
					Json::Value list;
					Json::Value dict;
					for(const Monitor& monitor : monitors) {
						dict["name"] = monitor.get<1>();
						dict["ip"] = monitor.get<2>();
						dict["href"] = monitor.get<3>();
						list.append(dict);
					}
					ostr << list.toStyledString();
				} // there are results of the query
			} // handling GET
			else { // handling other methods
				status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED;
				response.setStatusAndReason(status,"Requested URI does not handle this method (it handles GET only).");
				logBcast("information","Unsupported method");
				response.send();
			} // handling other methods
			dbsession.close();
			msg.append("Finished handling request: ").append(req).append(" with status ").append(::std::to_string(status));
			logBcast("information",msg.c_str());
		} // handleRequest()
	} // namespace Catalog
} // namespace PUHW
