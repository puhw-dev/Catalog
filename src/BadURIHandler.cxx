#include <PUHW/Catalog/BadURIHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

namespace PUHW {
	namespace Catalog {
		void BadURIHandler::handleRequest(::Poco::Net::HTTPServerRequest&, ::Poco::Net::HTTPServerResponse& response) {
				/*response.setChunkedTransferEncoding(true);
				response.setContentType("text/plain");
				::std::ostream& ostr = response.send();*/
				response.setStatusAndReason(::Poco::Net::HTTPResponse::HTTPStatus::HTTP_NOT_IMPLEMENTED,"Requested URI is not valid.");
				response.send();
			}
	} // namespace Catalog
} // namespace PUHW
