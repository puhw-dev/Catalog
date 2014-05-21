/**
* @file MonitorHandler.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef MONITORHANDLER_H
#define	MONITORHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

namespace PUHW {
	namespace Catalog {		
		class MonitorHandler: public ::Poco::Net::HTTPRequestHandler {
			public:
			explicit MonitorHandler(const std::string& id) : monitorID(id) {}
			void handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) override;
			private:
			const ::std::string monitorID;
		};
	} // namespace Catalog
} // namespace PUHW

#endif	/* MONITORHANDLER_H */
