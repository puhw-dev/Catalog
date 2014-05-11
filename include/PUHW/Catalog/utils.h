#ifndef UTILS_H
#define	UTILS_H

#include <Poco/Logger.h>
#include <Poco/Util/ServerApplication.h>
#include <list>

namespace PUHW {
	namespace Catalog {

		#define logBcast(type,message) \
			broadcastLog<::PUHW::Catalog::CatalogServer>(type,message,__FILE__,__LINE__)
		
		void callProperLoggerMethod(::Poco::Logger* logger, const ::std::string& type, const char* message, const char* file = "", int line = -1); 
		
		template <typename T>
		void broadcastLog(const ::std::string& type, const char* message, const char* file = "", int line = -1) {
			try {
				T& loggersOwner = dynamic_cast<T&>(::Poco::Util::Application::instance());
				for(auto logger : loggersOwner.getLoggers()) {
					callProperLoggerMethod(logger,type,message,file,line);
				}
			}
			catch(const ::std::bad_cast&) {
				::Poco::Util::Application::instance().logger().error("Down-casting error in helper function broadcastLog. Trying to log original message with application's default logger");
				callProperLoggerMethod(&((::Poco::Util::Application::instance()).logger()),type,message,file,line);
			}
		}				
		
	} // namespace Catalog
} // namespace PUHW
	
#endif	/* UTILS_H */

