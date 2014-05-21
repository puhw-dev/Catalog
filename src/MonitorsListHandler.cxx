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
			msg.clear();
			::Poco::Net::HTTPResponse::HTTPStatus status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK;
			::Poco::Data::Session dbsession("SQLite", (dynamic_cast<CatalogServer&>(::Poco::Util::ServerApplication::instance())).getDBPath());
			if(method == "GET") {
				Monitors monitors;
				dbsession << "SELECT * FROM MONITORS", into(monitors), now;
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
				::std::ostream& ostr = response.send();
				if(!list.empty()) ostr << list.toStyledString();
				else ostr << "[]";
			} // handling GET
			else if(method == "POST") {
				std::stringstream reqBody;
				::Poco::StreamCopier::copyStream(request.stream(),reqBody);
				const ::std::string body(reqBody.str());
				Json::Value root;
				Json::Reader reader;
				bool parsedSuccess = reader.parse(body,root,false);
				if(body.empty() || !parsedSuccess) {
					status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
					msg.append("Parsing error: ");
					if(body.empty()) msg.append("empty request body");
					else msg.append(reader.getFormatedErrorMessages());
					response.setStatusAndReason(status,msg);
					msg.clear();
					response.send();
				}
				else { // request body not empty and parsed successfully
					if(root.isNull() || !root.isObject()) {
						status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
						response.setStatusAndReason(status,"Expected non-empty dictionary as root element");
						response.send();
					}
					else { // Root element of request body is a dictionary
						Json::Value name = root["name"];
						Json::Value ip = root["ip"];
						if(name.isNull() || ip.isNull()) {
							status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
							response.setStatusAndReason(status,"Expected non-empty 'name' and 'ip' dictionary keys with values");
							response.send();
						}
						else { // dictionary from request body contains 'name' and 'ip' keys
							if(!name.isConvertibleTo(Json::stringValue) || !ip.isConvertibleTo(Json::stringValue)) {
								status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
								response.setStatusAndReason(status,"'name' and 'ip' should be strings");
								response.send();
							}
							else { // values of both attributes 'name' and 'ip' are strings
								const ::std::string monitorName(name.asString());
								const ::std::string monitorIP(ip.asString());
								if(monitorName.empty() || monitorIP.empty()) {
									status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
									response.setStatusAndReason(status,"Expected non-empty strings 'name' and 'ip'");
									response.send();
								}
								else { // values of both attributes 'name' and 'ip' are valid
									int count;
									dbsession << "SELECT COUNT(id) FROM MONITORS WHERE name = :name", into(count,-1), use(monitorName), now;
									if(count >= 1) {
										status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_CONFLICT;
										response.setStatusAndReason(status,"Monitor with given name already exists.");
										response.send();
									}
									else { // monitor with such name does not exist yet - it can be inserted now
										::Poco::URI reqURI(request.getURI());
										std::string href(reqURI.getAuthority());
										href.append("/monitors/").append(monitorName);
										dbsession << "INSERT INTO MONITORS VALUES (NULL,:name,:ip,:href)", use(monitorName), use(monitorIP), use(href), now;
										status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_CREATED;
										response.setStatus(status);
										response.send();
									}  // monitor with such name does not exist yet - it can be inserted now
								} // values of both attributes 'name' and 'ip' are valid
							} // values of both attributes 'name' and 'ip' are strings
						} // dictionary from request body contains 'name' and 'ip' keys
					} // Root element of request body is a dictionary
				} // request body not empty and parsed successfully
			} // handling POST
			else { // other methods
				status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED;
				response.setStatusAndReason(status,"Requested URI does not handle this method (it handles GET and POST requests only).");
				logBcast("information","Unsupported method");
				response.send();
			}
			// finalizing
			dbsession.close();
			msg.clear();
			msg.append("Finished handling request: ").append(req).append(" with status ").append(::std::to_string(status));
			logBcast("information",msg.c_str());
		} // handleRequest()
	} // namespace Catalog
} // namespace PUHW
