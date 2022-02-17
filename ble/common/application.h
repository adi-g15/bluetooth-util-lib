#pragma once

#include <memory>
#include <vector>

#include "sdbus-c++/Types.h"
#include "sdbus-c++/sdbus-c++.h"
#include "service.h"

class Application {
    std::unique_ptr<sdbus::IObject> application;
    std::vector<Service *> services;
    sdbus::IConnection &connection;

  public:
    Application(sdbus::IConnection &connection,
                const std::string &application_object_path);

    template <typename ServiceType, class... Args>
    ServiceType &addService(unsigned int index, std::string UUID,
                            Args... args) {
        /* Checks if ServiceType provides a constructor with the signature as in
         * the below message */
        static_assert(
            std::is_constructible_v<ServiceType, decltype(connection),
                                    std::string, unsigned int, std::string,
                                    Args...>,
            "\n======================================================\n"
            "Your Service class must provide a constructor: \n"
            "with this signature: \n"
            "YourService::YourService(sdbus::IConnection& "
            "connection, std::string application_path, unsigned int index, "
            "std::string UUID, ...otherargs)");

        /* Checks if ServiceType inherits from Service base class type */
        static_assert(std::is_base_of_v<Service, ServiceType>,
                      "Invalid ServiceType: Services should inherit from the "
                      "`Service` base class");

        auto service =
            new ServiceType(connection, this->application->getObjectPath(),
                            index, UUID, args...);
        services.push_back(service);

        // Note: @adig This pattern may cause dangling references, but
        // correcting it, will be more verbose code
        return *service;
    }

    sdbus::ObjectPath getObjectPath() const;
    void registerWithGattManager(std::string adapter_path = "") const;

    virtual void onInterfacesAdded(
        const sdbus::ObjectPath &object_path,
        const std::map<std::string, std::map<std::string, sdbus::Variant>>
            &interfaces_and_properties) = 0;

    virtual void
    onInterfacesRemoved(const sdbus::ObjectPath &object_path,
                        const std::vector<std::string> &interfaces) = 0;

    virtual ~Application() { /*Nothing to do*/
    }
};
