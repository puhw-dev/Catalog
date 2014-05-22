/**
* @file MonitorsListHandler.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef MONITORSLISTHANDLER_H
#define	MONITORSLISTHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

namespace PUHW {
	namespace Catalog {
		class MonitorsListHandler: public ::Poco::Net::HTTPRequestHandler {
			public:
			void handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) override;
		}; // class MonitorsListHandler
	} // namespace Catalog
} // namespace PUHW

#endif	/* MONITORSLISTHANDLER_H */

