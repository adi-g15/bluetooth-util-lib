#include <string>
#include <type_traits>

#include "application.h"
#include "declarations.h"

using std::string;

Application::Application(sdbus::IConnection &connection,
                         const string &application_object_path)
    : connection(connection) {

    application = sdbus::createObject(connection, application_object_path);
    application->addObjectManager();

    application->finishRegistration();
}

sdbus::ObjectPath Application::getObjectPath() const {
    return application->getObjectPath();
}
