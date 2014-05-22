#include <PUHW/Catalog/CatalogServer.h>
#include <PUHW/Catalog/RequestHandlerFactory.h>
#include <PUHW/Catalog/utils.h>
#include <Poco/SharedPtr.h>
#include <Poco/Logger.h>
#include <Poco/PatternFormatter.h>
#include <Poco/FileChannel.h>
#include <Poco/ConsoleChannel.h>
#include <Poco/File.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Net/HTTPServerParams.h>
#include <Poco/Util/IntValidator.h>
#include <Poco/Util/HelpFormatter.h>
#include "Poco/Data/Common.h"
#include "Poco/Data/SQLite/Connector.h"
#include <iostream>

using ::Poco::Data::into;
using ::Poco::Data::now;
using ::Poco::Data::use;

namespace PUHW {
	namespace Catalog {

		CatalogServer::CatalogServer() : ::Poco::Util::ServerApplication(), port(10666), maxQueuedConnections(32), maxThreads(4), version("0.2"), logPath("http-catalog-server.log"), exitImmediately(false), dbPath("catalog-database.sqlite"), createDBIfnotExists(false), exitCode(ExitCode::EXIT_OK) {
			/* register subsystems here*/
		}

		const ::std::list<::Poco::Logger*>& CatalogServer::getLoggers() const { 
			return loggers; 
		}

		void CatalogServer::initialize(::Poco::Util::Application& self) {
			::Poco::Util::Application::initialize(self); // required call
			::Poco::AutoPtr<::Poco::PatternFormatter> patternFormatter = new ::Poco::PatternFormatter("%Y-%m-%d %H:%M:%S.%i    %p\t\t%N    [%s]\t\t%t");
			patternFormatter->setProperty("times","local"); // setting timezone to local

			// creating file logger
			formattingFileChannel = new ::Poco::FormattingChannel(patternFormatter);
			::Poco::AutoPtr<::Poco::FileChannel> fileChannel = new ::Poco::FileChannel(logPath);
			formattingFileChannel->setChannel(fileChannel);
			formattingFileChannel->open();
			::Poco::Logger& fileLogger = ::Poco::Logger::create("CatalogFileLogger", formattingFileChannel, ::Poco::Message::PRIO_DEBUG);
			loggers.push_back(&fileLogger);
			setLogger(fileLogger); // will return this logger with this->logger()

			// creating console logger
			formattingConsoleChannel = new ::Poco::FormattingChannel(patternFormatter);	
			::Poco::AutoPtr<::Poco::ConsoleChannel> consoleChannel = new ::Poco::ConsoleChannel();
			formattingConsoleChannel->setChannel(consoleChannel);
			formattingConsoleChannel->open();
			::Poco::Logger& consoleLogger = ::Poco::Logger::create("CatalogConsoleLogger", formattingConsoleChannel, ::Poco::Message::PRIO_DEBUG);
			loggers.push_back(&consoleLogger);

			// db-init
			::Poco::Data::SQLite::Connector::registerConnector();
			checkDB();
		}

		void CatalogServer::uninitialize() {
			formattingFileChannel->close();
			formattingConsoleChannel->close();
			::Poco::Data::SQLite::Connector::unregisterConnector();
		}

		int CatalogServer::main(const ::std::vector<::std::string>&) {
			if(exitImmediately) { return exitCode; }
			::Poco::AutoPtr<::Poco::Net::HTTPServerParams> serverParams = new ::Poco::Net::HTTPServerParams;
			serverParams->setMaxQueued(maxQueuedConnections);
			serverParams->setMaxThreads(maxThreads);
			::Poco::Net::ServerSocket svs(port);
			::Poco::Net::HTTPServer srv(::Poco::SharedPtr<RequestHandlerFactory>(new RequestHandlerFactory()), svs, serverParams);
			srv.start();
			::std::string msg("Server is running at port number ");
			msg.append(::std::to_string(port)).append(" with maximum queued connections = ").append(::std::to_string(maxQueuedConnections)).append(" and maximum threads = ").append(::std::to_string(maxThreads));
			logBcast("information",msg.c_str());	
			waitForTerminationRequest();
			logBcast("information","Stopping server.");
			srv.stop();
			return exitCode;
		}

		void CatalogServer::defineOptions(::Poco::Util::OptionSet& options) {
			::Poco::Util::ServerApplication::defineOptions(options); // required
			options.addOption(::Poco::Util::Option("port", "p", "Runs server on port number <NUM> (valid range is 10000:65535), defaults to 10666.").required(false).repeatable(false).argument("<NUM>")
				   .validator(new ::Poco::Util::IntValidator(10000,65535)).callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::portHandler)));
			options.addOption(::Poco::Util::Option("max-queued-conn", "c", "Sets the maximum number of queued connections. Must be greater than 0. If there are already the maximum number of connections in the queue, new connections will be silently discarded. Defaults to 32.").required(false).repeatable(false).argument("<NUM>")
				   .validator(new ::Poco::Util::IntValidator(1,300)).callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::maxQueuedConnHandler)));
			options.addOption(::Poco::Util::Option("max-threads", "t", "Sets the maximum number of simultaneous threads available. Must be greater than or equal to 0. If 0 is specified, this parameter will be set to the number of available threads in its thread pool. Defaults to 4.").required(false).repeatable(false).argument("<NUM>")
				   .validator(new ::Poco::Util::IntValidator(0,32)).callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::maxThreadsHandler)));
			options.addOption(::Poco::Util::Option("log-file", "", "Log messages to <FILE>. The given path is relative to server working directory").required(false).repeatable(false).argument("<FILE>")
				   .callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::logPathHandler)));
			options.addOption(::Poco::Util::Option("help","h","Displays this help information and exits").required(false).repeatable(false)
			       .callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::displayHelpHandler)));
			options.addOption(::Poco::Util::Option("version","v","prints version information and exits").required(false).repeatable(false)
			       .callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::versionHandler)));
			options.addOption(::Poco::Util::Option("use-db", "", "Use sqlite3 database-file located at <FILE> instead of default database name. This option should always be used when --daemon was given (with absolute path to db-file).").required(false).repeatable(false).argument("<FILE>")
				   .callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::dbPathHandler)));
			options.addOption(::Poco::Util::Option("create-db","","Creates database file if not exists instead of failing which is default action if this option is ommited.").required(false).repeatable(false)
			       .callback(::Poco::Util::OptionCallback<CatalogServer>(this, &CatalogServer::createDBHandler)));
			//The Option takes ownership of the Validator and deletes it when it's no longer needed. 
		 }

		void CatalogServer::portHandler(const ::std::string&, const ::std::string& value) {
			port = ::std::atoi(value.c_str());
		}

		void CatalogServer::maxQueuedConnHandler(const ::std::string&, const ::std::string& value) {
			maxQueuedConnections = ::std::atoi(value.c_str());
		}

		void CatalogServer::maxThreadsHandler(const ::std::string&, const ::std::string& value) {
			maxThreads = ::std::atoi(value.c_str());
		}

		void CatalogServer::displayHelpHandler(const ::std::string&, const ::std::string&) {
			exitImmediately = true;
			::Poco::Util::HelpFormatter helpFmt(options());
			helpFmt.setUnixStyle(true);
			helpFmt.setAutoIndent();
			helpFmt.setCommand("catalog");
			helpFmt.setUsage("[OPTION]...");	
			helpFmt.setHeader(
							  "Runs standalone HTTP server for cataloguing service intended to work with other modules of PUHW project (see: https://github.com/puhw-dev/doc/wiki)."
							  "\nThis server provides REST API described at: https://github.com/puhw-dev/doc/tree/master/api."
			                  "\nPurpose of this server is to give PUHW-monitors possibility to register in the database of this catalog in order to find the hosts monitored by them later on."
			                 );
			helpFmt.setFooter(
							  "Project location on github: https://github.com/puhw-dev/Catalog"
							  "\nReport bugs on bug-tracker: https://github.com/puhw-dev/Catalog/issues"
			                  "\nAuthor Maciej.Siczek@fis.agh.edu.pl"
			                 );

			helpFmt.format(::std::cout);
			stopOptionsProcessing();
		}

		void CatalogServer::versionHandler(const ::std::string&, const ::std::string&) {
			exitImmediately = true;
			::std::cout << "catalog version: " << version << ::std::endl;
			stopOptionsProcessing();
		}

		void CatalogServer::logPathHandler(const ::std::string&, const ::std::string& value) {
			logPath = value;
		}

		void CatalogServer::dbPathHandler(const ::std::string&, const ::std::string& value) {
			dbPath = value;
		}

		void CatalogServer::createDBHandler(const ::std::string&, const ::std::string&) {
			createDBIfnotExists = true;
		}

		void CatalogServer::checkDB() {
			if(exitImmediately) return;
			::Poco::File dbFile(dbPath);
			bool databaseExists = dbFile.exists();
			if(!databaseExists && !createDBIfnotExists) {
				::std::string msg("Database '");
				msg.append(dbPath).append("' does not exist, exiting");
				logBcast("fatal",msg.c_str());
				exitImmediately = true;
				exitCode = ExitCode::EXIT_NOINPUT;
				return;
			}
			if(!databaseExists) logBcast("notice","Creating requested database-file.");
			::Poco::Data::Session dbsession("SQLite", dbPath.c_str());
			dbsession << "CREATE TABLE IF NOT EXISTS 'MONITORS' ('id' INTEGER PRIMARY KEY NOT NULL UNIQUE, 'name' TEXT NOT NULL UNIQUE, 'ip' TEXT NOT NULL, 'href' TEXT NOT NULL UNIQUE)", now;
			dbsession << "CREATE TABLE IF NOT EXISTS 'HOSTS' ('id' INTEGER PRIMARY KEY NOT NULL UNIQUE, 'hostname' TEXT NOT NULL UNIQUE, 'ip' TEXT NOT NULL, 'href' TEXT NOT NULL UNIQUE)", now;
			int countMonTbl = 0, countHostTbl = 0;
			dbsession << "SELECT COUNT(name) FROM 'SQLITE_MASTER' WHERE (type = 'table' AND name = 'MONITORS')", into(countMonTbl,-1), now;
			dbsession << "SELECT COUNT(name) FROM 'SQLITE_MASTER' WHERE (type = 'table' AND name = 'HOSTS')", into(countHostTbl,-1), now;
			dbsession.close();
			if(countMonTbl != 1 || countHostTbl != 1) {
				logBcast("fatal","Database has not required tables and the tables could not be created, exiting.");
				exitImmediately = true;
				exitCode = ExitCode::EXIT_IOERR;
				return;
			}
			logBcast("information","Database is ready.");
		}

	} // namespace Catalog
} // namespace PUHW
