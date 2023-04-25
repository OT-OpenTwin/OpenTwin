// Toolkit API
#include "ProjectCollection.h"

OToolkitAPI::ProjectCollection& OToolkitAPI::ProjectCollection::instance(void) {
	static ProjectCollection g_instance;
	return g_instance;
}

// ################################################################################################################################################

void OToolkitAPI::ProjectCollection::addImportDirectory(const QString& _directory) {

}

void OToolkitAPI::ProjectCollection::startImport(void) {

}

// ################################################################################################################################################

void OToolkitAPI::ProjectCollection::importWorker(void) {

}

// ################################################################################################################################################

OToolkitAPI::ProjectCollection::ProjectCollection() {

}

OToolkitAPI::ProjectCollection::~ProjectCollection() {

}