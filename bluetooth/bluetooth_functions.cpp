/**
 * @file bluetooth_functions.cpp
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Implementation of functions in bluetooth_functions.h
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#include "bluetooth_functions.h"

void send_file() {
    /**
     * @refrences:
     * 1. device_api.txt        -> Connect(), ConnectProfile(uuid),
     * Address{,Type}, not Name use 'Alias'
     * 2. obex-agent-api.txt    -> AuthorizePush(transfer)
     * 3. obex-api.txt          -> Transfer1, ObjectPush1 <sendfile(filename)>,
     * FileTransfer <putfile(...)>
     *
     * Not now, but interesting -> Phonebook, MessageAccess
     */
}

void receive_file() {}

void trasmit_audio() {
    /**
     * @references:
     * 1. media-api.txt -> RegisterPlayer(), MediaTransport1
     *
     */
}

void receive_audio() {}
