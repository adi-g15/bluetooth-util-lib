/**
 * @file file_transfer.cpp
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Function definitions for File transfer via Bluetooth
 * @version 0.1
 * @date 2022-02-19
 *
 * @copyright Apache License (c) 2022
 *
 */

#include <filesystem>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "bluetooth/file_transfer.h"
#include "sdbus-c++/sdbus-c++.h"

using std::string, std::cout, std::endl, std::map, std::vector;

namespace fs = std::filesystem;

/**
 * @brief Send a file to a connected device, and intentionally blocks till the
 * transfer is complete or errors
 *
 * @note Additional capabilities, like resume, pause, cancel etc. is on
 * interface org.bluez.obex.Transfer1
 *
 * @devnotes: There are many targets, such as File Transfer (ftp), etc. we chose
 * ObjectPush (opp) which provides a simple SendFile method
 * More info on this is in bluez/doc/obex-api.txt in the bluez source code
 *
 * @param remote_device_address Address of the connected device (in the form of
 * eg. "XX:XX:XX:XX:XX:XX")
 * @param local_filepath Filepath of the file to send, this can be a relative
 * path or an absolute path
 */
void sendFile(const string &remote_device_address,
              const string &local_filepath) {

    /* Get a connection to 'session bus', and start an event loop asynchronously
     * to catch signals, while we call methods on objects in this thread */
    auto conn = sdbus::createSessionBusConnection();
    conn->enterEventLoopAsync();

    /* /org/bluez/obex is the object of interest here, as it provides method to
     * create a new session, which is then used for the transfers (reference:
     * obex-api.txt) */
    auto obex = sdbus::createProxy(*conn, "org.bluez.obex", "/org/bluez/obex");

    /* Create a session, and get it's session path (required to start a
     * transfer) */
    const auto OBEX_CLIENT_INTERFACE = "org.bluez.obex.Client1";
    sdbus::ObjectPath session_object;
    obex->callMethod("CreateSession")
        .onInterface(OBEX_CLIENT_INTERFACE)
        .withArguments(remote_device_address,
                       map<string, sdbus::Variant>({{"Target", "opp"}}))
        .storeResultsTo(session_object);

#ifdef VERBOSE_DEBUG
    cout << "Created session: " << session_object << endl;
#endif

    /* Now, the session object is of our interest, providing `SendFile`, we use
     * the path returned earlier by CreateSession */
    const auto OBEX_OBJECTPUSH_IFACE = "org.bluez.obex.ObjectPush1";
    auto session = sdbus::createProxy(*conn, "org.bluez.obex", session_object);

    /* Register a signal handler for PropertiesChanged, bluez emits this signals
     * in between, with transfer updates (how much is sent); and at start/end
     * of the transfer, with status update (completed, or errored) */
    session->uponSignal("PropertiesChanged")
        .onInterface("org.freedesktop.DBus.Properties")
        .call([&conn](string interface, map<string, sdbus::Variant> properties,
                      vector<string> _invalidated) {
#ifdef VERBOSE_DEBUG
    // TODO - Register signal handler to see size, ref: obex-api.txt,
    // obex-client Properties of interest: "Status", "Transferred"
#endif
            auto status = properties.find("Status");
            if (status != properties.end()) {
                /* Leave the event loop (and hence the sendFile function), after
                 * status changes (completed/errored) */

                auto is_error = status->second.get<string>() == "error";
                auto is_complete = status->second.get<string>() == "complete";

                if (is_error) {
                    /* Reason will be in the exception thrown by SendFile */
                    cout << "ERROR: "
                         << "Object push failed\n";
                }

                /* Other values of 'Status' can be "queued", "active",
                 * "suspended", which I will ignore*/
                if (is_error || is_complete) {
                    conn->leaveEventLoop();
                }
            }
        });

    /* call SendFile, with the absolute path of `local_filepath`, it returns two
     * things: firstly the object path to the Transfer object which also can be
     * used to get update on status, and transfer properties (status,
     * transferred etc.) */
    sdbus::ObjectPath transfer_path;
    map<string, sdbus::Variant> transfer_properties;
    session->callMethod("SendFile")
        .onInterface(OBEX_OBJECTPUSH_IFACE)
        .withArguments(fs::absolute(local_filepath).c_str())
        .storeResultsTo(transfer_path, transfer_properties);

#ifdef VERBOSE_DEBUG
    cout << "Created transfer: " << transfer_path << endl;
    cout << "Transfer Properties: \n";
    for (const auto &p : transfer_properties) {
        cout << p.first << " : ";

        auto type = p.second.peekValueType();
        if (type == "s") {
            cout << p.second.get<string>();
        } else if (type == "u" || type == "y" || type == "n" || type == "q" ||
                   type == "i" || type == "u" || type == "x") {
            cout << p.second.get<i64>();
        } else if (type == "t") {
            cout << p.second.get<u64>();
        } else if (type == "b") {
            cout << std::boolalpha << p.second.get<bool>();
        } else if (type == "as") {
            auto vec = p.second.get<vector<string>>();
            cout << "[ ";
            for (auto &s : vec) {
                cout << s << ", ";
            }
            cout << " ]";
        } else if (type == "o") {
            cout << p.second.get<sdbus::ObjectPath>();
        } else {
            cout << type;
        }

        cout << endl;
    }
#endif

    // Since we are going to block in this thread, stop the event loop in other
    // thread (not require but better ensuring we stop what we started and dont
    // need :)
    conn->leaveEventLoop();
    // 'Blocking Wait' till we get an update on the transfer, will only end when
    // the signal handler (declared above) on PropertiesChanged runs
    conn->enterEventLoop();
}
