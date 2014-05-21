#include <PUHW/Catalog/BadURIHandler.h>
/*#include <PUHW/Catalog/utils.h>
#include <Poco/URI.h>*/
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace PUHW {
	namespace Catalog {
		void BadURIHandler::handleRequest(::Poco::Net::HTTPServerRequest&, ::Poco::Net::HTTPServerResponse& response) {
				/*const ::std::string method(request.getMethod());
				::Poco::URI requestURI(request.getURI());
				::std::string requestPath = requestURI.getPath();
				::std::string req(method);
				req.append(" ").append(requestPath);
				::std::string msg("BadURIHandler is handling ");
				msg.append(req);
				logBcast("information",msg.c_str());
				msg.clear();*/
				::Poco::Net::HTTPResponse::HTTPStatus status = ::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED;
				response.setStatusAndReason(status,"Requested URI is not valid.");
				response.send();
				/*msg.append("Finished handling request ").append(req).append(" with status ").append(::std::to_string(status));
				logBcast("information",msg.c_str());*/
			}
	} // namespace Catalog
} // namespace PUHW
