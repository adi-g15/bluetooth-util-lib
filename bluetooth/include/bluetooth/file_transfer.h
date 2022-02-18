/**
 * @file file_transfer.h
 * @author Aditya Gupta (adityag.ug19.cs@nitp.ac.in)
 * @brief Function declarations for File transfer via Bluetooth
 * @version 0.1
 * @date 2022-02-15
 *
 * @copyright Apache License (c) 2022
 *
 */

#pragma once

#include <string>

void sendFile(const std::string &remote_device_address,
              const std::string &local_filepath);

/* For recieving, there is org.bluez.obex.FileTransfer.GetFile, but not doing it
 * now not required*/
