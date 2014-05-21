#include <PUHW/Catalog/MonitorHandler.h>
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
		void MonitorHandler::handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) {
			const ::std::string method(request.getMethod());
			::Poco::URI requestURI(request.getURI());
			::std::string requestPath = requestURI.getPath();
			::std::string req(method);
			req.append(requestPath);
			::std::string msg("MonitorHandler is handling ");
			msg.append(req);
			logBcast("information",msg.c_str());
			msg.clear();
			::Poco::Net::HTTPResponse::HTTPStatus status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK;
			::Poco::Data::Session dbsession("SQLite", (dynamic_cast<CatalogServer&>(::Poco::Util::ServerApplication::instance())).getDBPath());
			if(method == "GET") {
				::Poco::Data::Statement select(dbsession);
				Monitors monitors;
				select << "SELECT * FROM MONITORS WHERE name=:name", into(monitors), use(monitorID), now;
				if(monitors.empty()) {
					status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_GONE;
					::std::string reason("No registered monitor with name \"");
					reason.append(monitorID).append("\".");
					response.setStatusAndReason(status,reason);
					response.send();
				}
				else { // there is monitor with given name
					Json::Value dict;
					Monitor monitor = *monitors.begin();
					dict["name"] = monitor.get<1>();
					dict["ip"] = monitor.get<2>();
					dict["href"] = monitor.get<3>();
					response.setChunkedTransferEncoding(true);
					response.setContentType("text/plain"); // or "application/json" ?
					status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_OK;
					response.setStatus(status);
					::std::ostream& ostr = response.send();
					ostr << dict.toStyledString();
				} // there is monitor with given name
			} // handling GET
			else if(method == "POST") {
				int count;
				dbsession << "SELECT COUNT(id) FROM MONITORS WHERE name=:name", into(count,0), use(monitorID), now;
				if(count != 1) {
					status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_GONE;
					::std::string reason("No registered monitor with name \"");
					reason.append(monitorID).append("\".");
					response.setStatusAndReason(status,reason);
					response.send();
				}
				else { // there is a monitor with given name
					std::stringstream reqBody;
					::Poco::StreamCopier::copyStream(request.stream(),reqBody);
					const ::std::string body(reqBody.str());
					Json::Value root;
					Json::Reader reader;
					bool parsedSuccess = reader.parse(body,root,false);
					if(body.empty() || !parsedSuccess) {
						status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
						response.setStatusAndReason(status,reader.getFormatedErrorMessages());
						response.send();
					}
					else { // non - empty body and parsed successfully
						if(root.isNull() || !root.isObject()) {
							status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
							response.setStatusAndReason(status,"Expected non-empty dictionary as root element");
							response.send();
						}
						else { // root element of body of request is a dictionary
							Json::Value ip = root["ip"];
							if(ip.isNull()) {
								response.setStatusAndReason(status,"Expected non-empty 'ip' dictionary key with corresponding ip address of monitor");
								response.send();
							}
							else { // there is non-empty element 'ip' in the dictionary
								if(!ip.isConvertibleTo(Json::stringValue)) {
									status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
									response.setStatusAndReason(status,"'ip' should be string");
									response.send();
								}
								else { // value for key 'ip' is a string
									const ::std::string monitorIP(ip.asString());
									if(monitorIP.empty()) {
										status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_BAD_REQUEST;
										response.setStatusAndReason(status,"Expected non-empty string 'ip'");
										response.send();
									}
									else { // given ip address is a non-empty string
										logBcast("debug","update");
										dbsession << "UPDATE MONITORS SET ip = :ip WHERE name = :name", use(monitorIP), use(monitorID), now;
										response.send();
									}
								} // value for key 'ip' is a string
							} // there is non-empty element 'ip' in the dictionary
						} // root element of body of request is a dictionary
					} // non - empty body and parsed successfully
				} // there is a monitor with given name
			} // handling POST
			else if(method == "DELETE") {
				int count;
				dbsession << "SELECT COUNT(id) FROM MONITORS WHERE name=:name", into(count,0), use(monitorID), now;
				if(count != 1) {
					status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_GONE;
					::std::string reason("No registered monitor with name \"");
					reason.append(monitorID).append("\".");
					response.setStatusAndReason(status,reason);
					response.send();
				}
				else { // monitor with given name exists
					dbsession << "DELETE FROM MONITORS WHERE name = :name", use(monitorID), now;
					response.send();
				}
			} // handling DELETE
			else { // handling other methods
				status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED;
				response.setStatusAndReason(status,"Requested URI does not handle this method (it handles GET, POST, and DELETE requests only).");
				logBcast("information","Unsupported method");
				response.send();
			} // handling other methods
			dbsession.close();
			msg.append("Finished handling request: ").append(req).append(" with status ").append(::std::to_string(status));
			logBcast("information",msg.c_str());
		} // handleRequest()
	} // namespace Catalog
} // namespace PUHW
