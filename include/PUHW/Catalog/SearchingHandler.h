/**
* @file SearchingHandler.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef SEARCHINGHANDLER_H
#define SEARCHINGHANDLER_H

#include <Poco/Net/HTTPRequestHandler.h>

namespace PUHW {
	namespace Catalog {
		class SearchingHandler: public ::Poco::Net::HTTPRequestHandler {
			public:
			explicit SearchingHandler(const std::string& searchedPhrase) : searchedPhrase(searchedPhrase) {}
			void handleRequest(::Poco::Net::HTTPServerRequest& request, ::Poco::Net::HTTPServerResponse& response) override;
			private:
			const ::std::string searchedPhrase;
		};
	} // namespace Catalog
} // namespace PUHW

#endif // SEARCHINGHANDLER_H
