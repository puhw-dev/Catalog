/**
* @file CatalogServer.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef CATALOGSERVER_H
#define	CATALOGSERVER_H

#include <Poco/AutoPtr.h>
#include <Poco/FormattingChannel.h>
#include <Poco/Util/ServerApplication.h>
#include <list>

namespace PUHW {
	namespace Catalog {
		class CatalogServer : public ::Poco::Util::ServerApplication {
			public:
			CatalogServer();
			const ::std::list< ::Poco::Logger*>& getLoggers() const;
			const ::std::string& getDBPath() const { return dbPath; }
			
			private:
			void defineOptions(::Poco::Util::OptionSet& options) override;
			void portHandler(const ::std::string&, const ::std::string& value);
			void displayHelpHandler(const ::std::string&, const ::std::string&);
			void versionHandler(const ::std::string&, const ::std::string&);
			void maxQueuedConnHandler(const ::std::string&, const ::std::string& value);
			void maxThreadsHandler(const ::std::string&, const ::std::string& value);
			void logPathHandler(const ::std::string&, const ::std::string& value);
			void dbPathHandler(const ::std::string&, const ::std::string& value);
			void createDBHandler(const ::std::string&, const ::std::string&);
			void initialize(::Poco::Util::Application& self) override;
			void checkDB();
			void uninitialize() override;
			int main(const ::std::vector< ::std::string>&) override;
						
			::Poco::UInt16 port;
			int maxQueuedConnections;
			int maxThreads;
			::Poco::AutoPtr< ::Poco::FormattingChannel> formattingFileChannel;
			::Poco::AutoPtr< ::Poco::FormattingChannel> formattingConsoleChannel;
			::std::list< ::Poco::Logger*> loggers;
			const ::std::string version;
			::std::string logPath;
			bool exitImmediately;
			std::string dbPath;
			bool createDBIfnotExists;
			ExitCode exitCode;
		}; // class CatalogServer
	} // namespace Catalog
} // namespace PUHW

#endif	/* CATALOGSERVER_H */
