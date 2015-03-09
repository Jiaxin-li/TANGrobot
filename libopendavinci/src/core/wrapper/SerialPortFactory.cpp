/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "core/wrapper/SerialPortFactory.h"

#include "core/wrapper/Libraries.h"
#include "core/wrapper/ConfigurationTraits.h"
#include "core/wrapper/NetworkLibraryProducts.h"

#include "core/wrapper/SerialPortFactoryWorker.h"
#include "core/wrapper/SerialPort.h"

#ifdef WIN32
    #include "core/wrapper/WIN32/WIN32SerialPortFactoryWorker.h"
#endif
#ifndef WIN32
  #ifndef __APPLE__
    #include "core/wrapper/POSIX/POSIXSerialPortFactoryWorker.h"
  #endif
#endif

namespace core {
    namespace wrapper {

        SerialPort* SerialPortFactory::createSerialPort(const string &port, const uint32_t &baudRate, const SerialPortSettings &settings)
        {
            typedef ConfigurationTraits<SerialPortLibraryProducts>::configuration configuration;

            return SerialPortFactoryWorker<configuration::value>::createSerialPort(port, baudRate, settings);
        }

    }
} // core::wrapper
