/*
 * This file is part of the Naphex project
 * Copyright (C) 2014  Alex Silva Torres <alexst07@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef NAPHEX_CHANNEL_H
#define NAPHEX_CHANNEL_H

#include <string>
#include <map>
#include <mutex>
#include <thread>

/**
 * \class channel
 *
 * \brief A channel to send and receive data across threads
 *
 * Provide a single access point to data, this data can be accessed
 * by any thread created by user
 */

class channel {
 public:
  /**
   * Creates a single instance of the object
   */
  static channel* Instance();

  /**
   * Destructor
   */
  ~channel();

  /**
   * \brief Insert a new channel
   * 
   * Insert a new data on the channel, the data is stored
   * in the folowed format data["ch_name"] = value
   * in C++ is used the map data structure to stored the
   * information
   * 
   * @param ch_name 	Specifies the name of the channel 
   * 			where the data can be recovered
   * @param value	Specifies the data
   * 
   * @return 		Returns true on success or false 	
   * 			opposite case
   */
  static bool insert(std::string ch_name, std::string value);

  /**
   * \brief Erase a channel
   * 
   * Deletes an element of estrtura map of C++ 
   * according to the passed name
   * 
   * @param ch_name	Specifies the name of the channel
   * 			to be deleted
   * 
   * @return		Returns true on success or false 	
   * 			opposite case
   */
  static bool erase(std::string ch_name);

  /**
   * \brief Checks if a channel exists
   * 
   * Checks if a exists a channel called ch_name
   * 
   * @param ch_name	Name of channel
   * 
   * @return		Returns true on success or false 	
   * 			opposite case
   */
  static bool exist(std::string ch_name);

  /**
   * \brief Gets the data stored in a channel
   * 
   * @param ch_name	Name of channel
   * 
   * @return		Return the string stored
   */
  static std::string get(std::string ch_name);

  /**
   * \brief Change a string stored in a channel
   * 
   * @param ch_name	Name of the channel
   * @param value	Value to be stored in the place
   * 			of old string stored
   * @return		Returns true on success or false 	
   * 			opposite case
   */
  static bool set(std::string ch_name, std::string value);

 private:
  static std::mutex mtx;
  static std::map<std::string, std::string> channels;

  /**
   * Store the instance of objecty channel
   */
  static channel* _instance;
};

#endif  // NAPHEX_CHANNEL_H
