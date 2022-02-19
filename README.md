## Bluetooth + BLE util library

<center>
<a href="https://www.codefactor.io/repository/github/adi-g15/bluetooth-util-lib"><img src="https://www.codefactor.io/repository/github/adi-g15/bluetooth-util-lib/badge" alt="CodeFactor" /></a>
</center>

## Initiating

* File transfer (Sender) using bluetooth protocol
* Audio transmission (eg to bluetooth speaker)
* Audio Receiver (eg. receive audio from mobile and play on internal speaker)

> Additional descriptive comments to be added at end

## BLE

> First priority

### BLE Peripheral

1. Turn on/off BLE advertising
2. Implement individual characteristics
3. Implement services

### BLE Central (Host)

1. Scan for all available BLE peripheral devices
2. List all services and characteristics
3. Read/Write Characteristics

## Tools

Used this android app to try connecting with the server: https://play.google.com/store/apps/details?id=no.nordicsemi.android.mcp

Follow on screen instructions and start scanning

## Example Usage

> Mai bas tests se idhar copy kr rha hu, sayad only sdbus wale programs ho skte is library ko use bhi na krte ho

> Dont pay too much attention on why BLE vs bluetooth written in two parts :')

* BLE
  1. [Registering Application (GATT Peripheral)](#registering-application)
  2. [Start advertising](#start-advertising)

* Bluetooth
  1. [Connect to device](#connect-to-device)

* Miscellaneous
  1. [Create a object](#create-a-object)
  2. [Check if adapter is on](#check-if-adapter-is-on)
  3. [Get adapter capable of advertising](#get-adapter-capable-of-advertising)
  4. [Get device address](#get-device-address)

### BLE

#### Registering Application

This is a long one...
The steps are:

1. Declare application, services, characteristics classes, inheriting respective base classes

```cpp
class MyApplication : public Application {
  public:
    MyApplication(sdbus::IConnection &connection,
                  const std::string &application_object_path)
        : Application(connection, application_object_path) {}

    void onInterfacesAdded(
        const sdbus::ObjectPath &object_path,
        const std::map<std::string, std::map<std::string, sdbus::Variant>>
            &interfaces_and_properties) override {
        cout << "Kuchh karo idhar" << endl;
    }
    void
    onInterfacesRemoved(const sdbus::ObjectPath &object_path,
                        const std::vector<std::string> &interfaces) override {
        cout << "Isko kaun handle karega :' )" << endl;
    }
};

class MyService : public Service {
  public:
    MyService(sdbus::IConnection &connection,
                     std::string application_path, unsigned int index,
                     std::string UUID)
        : Service(connection, application_path, 0, UUID) {}

    struct MyCharacteristic1 : public Characteristic {
        MyCharacteristic1(sdbus::IConnection &connection,
                                 std::string service_path, unsigned int index,
                                 std::string UUID)
            : Characteristic(connection, service_path, index, UUID) {}

        std::vector<uint8_t> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<uint8_t> value,
                   std::map<std::string, sdbus::Variant> options) override {
            cout << "Kuchh write karo idhar\n";
        }
    };

    struct MyCharacteristic2 : public Characteristic {
        MyCharacteristic2(sdbus::IConnection &connection,
                                 std::string service_path, unsigned int index,
                                 std::string UUID)
            : Characteristic(connection, service_path, index, UUID) {}

        std::vector<uint8_t> ReadValue(
            std::map<std::string, sdbus::Variant> options) const override {
            return {};
        }

        void
        WriteValue(std::vector<uint8_t> value,
                   std::map<std::string, sdbus::Variant> options) override {
            cout << "Kuchh write karo idhar\n";
        }
    };
};
```

2. Create instance of your application class

```cpp
    auto myapp = new MyApplication(conn, "/com/example");

    auto &service = myapp->addService<MyService>(
        0, "0000180d-0000-1000-8000-00805f9b34fb");

    service.addCharacteristic<MyService::MyCharacteristic1>(
        0, "00002a37-0000-1000-8000-00805f9b34fb");

    service.addCharacteristic<MyService::MyCharacteristic2>(
        1, "00002a38-0000-1000-8000-00805f9b34fb");
```

3. Register application (automatically registers services and their characteristics)

```cpp
    myapp->registerWithGattManager();
```

#### Start advertising

For this, the library provides an advertisement object, just create it and call turnOnAdvertising.

TODO: Provide user to set advertised name etc.

```cpp
    #include "ble/advertisement.h"

    auto conn = sdbus::createSystemBusConnection();
    auto advertisement = Advertisement(conn);
    advertisement.turnOnAdvertising();
```

### Bluetooth

#### Connect to device

Two ways: Using the address, or using name

```cpp
    #include "bluetooth/functions.h"

    connect_to_device_using_address("XX:XX:XX:XX:XX:XX");
    connect_to_device_using_name("Rockerz 450");
```

#### Send file

```cpp
    #include "bluetooth/file_transfer.h"

    sendFile("XX:XX:XX:XX:XX:XX", "/etc/fstab");
```

### Miscellaneous

#### Create an object

For eg. just creating a root object

```cpp
    auto root_obj = sdbus::createObject(conn, "/");
    root_obj->registerProperty("name")
        .onInterface("me.adig.DBus.Properties")
        .withGetter([]() { return "gupta"; });
    root_obj->finishRegistration();
```

#### Check if adapter is on

Just get the "Powered" property on the adapter object

```cpp
    bool is_powered_on = sdbus::createProxy("org.bluez", "/org/bluez/hci0")
            ->getProperty("Powered")
            .onInterface("org.bluez.Adapter1")
            .get<bool>();
```

#### Get adapter capable of advertising

For an adapter (eg. "/org/bluez/hci0") to support advertising, it must implement "org.bluez.LEAdvertisingManager1" interface

```cpp
    auto results = std::map<
        sdbus::ObjectPath,
        std::map<std::string, std::map<std::string, sdbus::Variant>>>();
    sdbus::createProxy("org.bluez", "/")
        ->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(results);

    for (const auto &obj : results) {
        auto it = obj.second.find("org.bluez.LEAdvertisingManager1");
        if (it != obj.second.cend()) {
            return obj.first;
        }
    }

    throw std::logic_error(
        "No adapter implementing org.bluez.LEAdvertisingManager1 found !");
```

#### Get device address

If you know the device name, you can find the address programmatically using:

```cpp
    auto actual_name = string();
    auto address = string();
    auto result =
        std::map<sdbus::ObjectPath,
                 std::map<string, std::map<string, sdbus::Variant>>>();

    sdbus::createProxy("org.bluez", "/")
        ->callMethod("GetManagedObjects")
        .onInterface("org.freedesktop.DBus.ObjectManager")
        .storeResultsTo(result);

    for (auto &p : result) {
        if (p.first.find("/org/bluez/hci0/dev_") == 0 &&
            p.first.length() ==
                sizeof("/org/bluez/hci0/dev_XX_XX_XX_XX_XX_XX") - 1) {
            auto name = p.second["org.bluez.Device1"]["Alias"].get<string>();
            const auto &addr =
                p.second["org.bluez.Device1"]["Address"].get<string>();

            if (name.find(device_name) != string::npos) {
                // `device_name` matched a substring in name
                return addr;
            }
        }
    }
```

Or simply,


```cpp
    #include "bluetooth/device.h"

    auto addr = get_device_address_by_name("Rockerz 450");
```

### Developer Notes

First of all:

> Reading documentation for 2 hour, will prevent you from 12 hours of frustration due to "why it's not working".

> I read it somewhere once, and i just keep passing this :)

To read the code, or modify it:

1. Read about bluez DBus API. I suggest https://www.bluetooth.com/bluetooth-resources/bluetooth-for-linux/, read Chapter 2-4 atleast
2. Read about sdbus-c++, a short wiki is at https://github.com/Kistler-Group/sdbus-cpp/blob/master/docs/using-sdbus-c++.md

Then, check if the tests are working:

```sh
cmake -B build && cmake --build build
./build/ble/test_ble
./build/bluetooth/test_bluetooth
```

