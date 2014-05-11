/**
* @file RequestHandlerFactory.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef REQUESTHANDLERFACTORY_H
#define	REQUESTHANDLERFACTORY_H

#include <Poco/Net/HTTPRequestHandlerFactory.h>

namespace PUHW {
	namespace Catalog {
		class RequestHandlerFactory: public ::Poco::Net::HTTPRequestHandlerFactory {
			public:
			::Poco::Net::HTTPRequestHandler* createRequestHandler(const ::Poco::Net::HTTPServerRequest& request) override;
		};
	} // namespace Catalog
} // namespace PUHW

#endif	/* REQUESTHANDLERFACTORY_H */
