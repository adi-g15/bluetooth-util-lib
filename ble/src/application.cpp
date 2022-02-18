#include <iostream>
#include <string>
#include <type_traits>

#include "adapter.h"
#include "application.h"
#include "declarations.h"
#include "sdbus-c++/IProxy.h"
#include "sdbus-c++/Types.h"

using std::string;

Application::Application(sdbus::IConnection &connection,
                         const string &application_object_path)
    : connection(connection) {

    application = sdbus::createObject(connection, application_object_path);
    application->addObjectManager();

    application->finishRegistration();

#ifdef VERBOSE_DEBUG
    std::cout << "Created service at path: " << application->getObjectPath()
              << std::endl;
#endif
}

sdbus::ObjectPath Application::getObjectPath() const {
    return application->getObjectPath();
}

void Application::registerWithGattManager(std::string adapter_path) const {
    if (adapter_path.empty()) {
        adapter_path = get_advertising_capable_adapter_path();
    }

    /**
     * Similar to the case with registering advertisements
     * Once we call RegisterApplication, we wait for bluez reply
     * BUT, bluez called GetManagedObjects on us, and wait for our reply...
     * but we are waiting... then who replies ?
     * That's why we start the event loop in other thread, that will handle
     **/
    connection.enterEventLoopAsync();

    const auto GATT_MANAGER_IFACE_NAME = "org.bluez.GattManager1";
    sdbus::createProxy(connection, "org.bluez", adapter_path)
        ->callMethod("RegisterApplication")
        .onInterface(GATT_MANAGER_IFACE_NAME)
        .withArguments(sdbus::ObjectPath(application->getObjectPath()),
                       std::map<std::string, sdbus::Variant>());

    /* Clean up, event loop not needed by us */
    connection.leaveEventLoop();
}
