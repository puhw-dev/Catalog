/**
* @file MonitorHandler.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef MONITORHANDLER_H
#define	MONITORHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

namespace PUHW {
	namespace Catalog {
		//class CatalogServer;
		
		class MonitorHandler: public ::Poco::Net::HTTPRequestHandler {
			public:
			void handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) override;
		};
	} // namespace Catalog
} // namespace PUHW

#endif	/* MONITORHANDLER_H */
