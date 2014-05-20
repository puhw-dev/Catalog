#include <PUHW/Catalog/MonitorsListHandler.h>
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
		void MonitorsListHandler::handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) {
			const ::std::string method(request.getMethod());
			::std::string req(method);
			req.append(" /monitors");
			::std::string msg("MonitorsListHandler is handling ");
			msg.append(req);
			logBcast("information",msg.c_str());
			if(method == "GET") {
				::Poco::Data::Session dbsession("SQLite", (dynamic_cast<CatalogServer&>(::Poco::Util::ServerApplication::instance())).getDBPath());
				::Poco::Data::Statement select(dbsession);
				Monitors monitors;
				select << "SELECT * FROM MONITORS", into(monitors), now;
				Json::Value list;
				Json::Value dict;
				for(const Monitor& monitor : monitors) {
					dict["name"] = monitor.get<1>();
					dict["ip"] = monitor.get<2>();
					dict["href"] = monitor.get<3>();
					list.append(dict);
				}
				response.setChunkedTransferEncoding(true);
				response.setContentType("text/plain"); // or "application/json" ?
				response.setStatus(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK);
				::std::ostream& ostr = response.send();
				if(!list.empty()) ostr << list.toStyledString();
				else ostr << "[]";
				dbsession.close();
				msg.clear(); msg.append("Finished handling request: ").append(req).append(" with status 200");
				logBcast("information",msg.c_str());
			} // handling GET
			else if(method == "POST") {
				std::stringstream reqBody;
				::Poco::StreamCopier::copyStream(request.stream(),reqBody);
				const ::std::string body(reqBody.str());
				Json::Value root;
				Json::Reader reader;
				bool parsedSuccess = reader.parse(body,root,false);
				msg.clear(); msg.append("Finished handling request ").append(req).append(" with status ");
				if(body.empty() || !parsedSuccess) {
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST,reader.getFormatedErrorMessages());
					msg.append("400");
					logBcast("information",msg.c_str());
					response.send();
					return;
				}
				if(root.isNull() || !root.isObject()) {
					msg.append("400");
					logBcast("information",msg.c_str());
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST,"Expected non-empty dictionary as root element");
					response.send();
					return;
				}
				Json::Value name = root["name"];
				Json::Value ip = root["ip"];
				if(name.isNull() || ip.isNull()) {
					msg.append("400");
					logBcast("information",msg.c_str());
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST,"Expected non-empty 'name' and 'ip' dictionary keys with values");
					response.send();
					return;
				}
				if(!name.isConvertibleTo(Json::stringValue) || !ip.isConvertibleTo(Json::stringValue)) {
					msg.append("400");
					logBcast("information",msg.c_str());
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST,"'name' and 'ip' should be strings");
					response.send();
					return;
				}
				const ::std::string monitorName(name.asString());
				const ::std::string monitorIP(ip.asString());
				if(monitorName.empty() || monitorIP.empty()) {
					msg.append("400");
					logBcast("information",msg.c_str());
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST,"Expected non-empty strings 'name' and 'ip'");
					response.send();
					return;
				}
				::Poco::Data::Session dbsession("SQLite", (dynamic_cast<CatalogServer&>(::Poco::Util::ServerApplication::instance())).getDBPath());
				int count;
				dbsession << "SELECT COUNT(id) FROM MONITORS WHERE name = :name", into(count,-1), use(monitorName), now;
				if(count >= 1) {
					msg.append("409");
					logBcast("information",msg.c_str());
					response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT,"Monitor with given name already exists.");
					response.send();
					return;
				}
				::Poco::URI reqURI(request.getURI());
				std::string href(reqURI.getHost());
				href.append("/monitors/").append(monitorName);
				dbsession << "INSERT INTO MONITORS VALUES (NULL,:name,:ip,:href)", use(monitorName), use(monitorIP), use(href), now;
				response.setStatus(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED);
				dbsession.close();
				msg.append("201");
				logBcast("information",msg.c_str());
				response.send();
			} // handling POST
			else { // other methods
				response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED,"Requested URI does not handle this method (it handles GET and POST requests only).");
				logBcast("information","Unsupported method");
				response.send();
			}
		}
	} // namespace Catalog
} // namespace PUHW
