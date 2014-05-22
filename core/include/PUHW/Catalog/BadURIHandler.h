/**
* @file BadURIHandler.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef BADURIHANDLER_H
#define	BADURIHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

namespace PUHW {
	namespace Catalog {
		class BadURIHandler: public ::Poco::Net::HTTPRequestHandler {
			public:
			void handleRequest(::Poco::Net::HTTPServerRequest&, ::Poco::Net::HTTPServerResponse& response) override;
		};
	} // namespace Catalog
} // namespace PUHW

#endif	/* BADURIHANDLER_H */

