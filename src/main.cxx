#include <PUHW/Catalog/CatalogServer.h>

int main(int argc, char** argv) {
	PUHW::Catalog::CatalogServer catalogServer;
	return catalogServer.run(argc, argv);
}