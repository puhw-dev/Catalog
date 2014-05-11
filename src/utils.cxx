#include <PUHW/Catalog/utils.h>
#include <string>
#include <cstring> // for strlen())

namespace PUHW {
	namespace Catalog {
		
		void callProperLoggerMethod(::Poco::Logger* logger, const ::std::string& type, const char* message, const char* file, int line) {
			::std::string extMsg;
			if((::std::strlen(file) > 0) && (-1 != line)) {
				extMsg.append(file).append(":").append(::std::to_string(line)).append(" : ").append(message);
			}
			else extMsg.append(message);
			
			if("information" == type) logger->information(extMsg);
			else if("debug" == type) logger->debug(extMsg);
			else if("error" == type) logger->error(extMsg);
			else if("warning" == type) logger->warning(extMsg);
			else if("critical" == type) logger->critical(extMsg);
			else if("fatal" == type) logger->fatal(extMsg);
			else if("notice" == type) logger->notice(extMsg);
			else if("trace" == type) logger->trace(extMsg);
			else {
				::std::string msg("Level of message was misspelled, FATAL used instead. Original message was [");
				msg.append(extMsg).append("]");
				logger->fatal(msg);
			}
		}
		
	} // namespace Catalog
} // namespace PUHW
