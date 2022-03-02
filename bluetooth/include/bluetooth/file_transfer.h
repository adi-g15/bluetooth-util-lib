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
void sendFile(const std::string &remote_device_address,
              const std::string &local_filepath);

/* For recieving, there is org.bluez.obex.FileTransfer.GetFile, but not doing it
 * now not required*/
