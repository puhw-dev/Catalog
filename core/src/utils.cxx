#include <PUHW/Catalog/utils.h>
#include <string>
#include <cstring> // for strlen())
#define BOOST_REGEXP // comment out for libstdc++ regex

#ifdef BOOST_REGEXP
	#include <boost/regex.hpp>
	using ::boost::regex;
	using ::boost::regex_match;
	using ::boost::smatch;
#else
	#include <regex>
	using ::std::regex;
	using ::std::regex_match;
	using ::std::smatch;
#endif

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

		bool isValidPortNr(const ::std::string& port) {
			if(!port.empty()) {
				int prt = ::std::atoi(port.c_str());
				if(prt <= 0 || prt > 65535) return false;
			}
			return true;
		}

		bool isValidAddress(const ::std::string& address) {
			// checking IPv4
			smatch match;
			int byte;
			regex ipv4(R"((http://)?([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})\.([0-9]{1,3})(:([0-9]{1,5}))?)");
			regex ipv6(R"((http://)?([0-9a-fA-F]{4}|0|)(:([0-9a-fA-F]{4}|0|)){3,7})");
			regex ipv6port(R"((http://)?\[([0-9a-fA-F]{4}|0|)(:([0-9a-fA-F]{4}|0|)){3,7}\]:([0-9]{1,5}))");
			regex domain(R"((http://)?[a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9](\.([a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9])){0,61}\.([a-zA-Z0-9][a-zA-Z0-9-]{0,61}[a-zA-Z0-9])(:([0-9]{1,5}))?)");
			if(regex_match(address,match,ipv4)) {
				if(::std::atoi(match[2].str().c_str()) == 0 || 0 == ::std::atoi(match[5].str().c_str())) return false; // first and last IP addr byte
				for(size_t i = 2; i <= 5; ++i) {
					byte = ::std::atoi(match[i].str().c_str());
					if(byte > 255 || byte < 0) return false;
				}
				if(!isValidPortNr(match[7])) return false;
				return true;
			} // IPv4
			else if(regex_match(address,match,ipv6)) return true;
			else if(regex_match(address,match,ipv6port)) {
				if(!isValidPortNr(match[5])) return false;
				return true;
			} // IPv6 with port number
			// checking domain address
			else if(address.size() < 259) {
				if(regex_match(address,match,domain)) {
					return isValidPortNr(match[6]);
				}
				else return false;
			}
			else {
				return false;
			}
		}

		bool isValidMonitorName(const ::std::string& name) {
			regex monitorName(R"([a-zA-Z][a-zA-Z0-9_-]*)");
			return regex_match(name,monitorName);
		}

		bool isValidSearchedPhrase(const ::std::string& phrase) {
			regex phraseRegexp(R"([a-zA-Z0-9\-]+)");
			return regex_match(phrase,phraseRegexp);
		}
	} // namespace Catalog
} // namespace PUHW
