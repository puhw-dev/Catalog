/**
* @file DataTypes.h
* @author Maciej.Siczek@fis.agh.edu.pl
*/

#ifndef DATATYPES_H
#define	DATATYPES_H

#include <Poco/Tuple.h>
#include <vector>

namespace PUHW {
	namespace Catalog {
		typedef ::Poco::Tuple<int,::std::string, ::std::string, ::std::string> Monitor;
		typedef ::std::vector<Monitor> Monitors;
	} // namespace Catalog
} // namespace PUHW

#endif	/* DATATYPES_H */
