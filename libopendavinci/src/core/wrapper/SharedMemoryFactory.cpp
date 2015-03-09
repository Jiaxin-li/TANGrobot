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

#include "core/wrapper/SharedMemoryFactory.h"

#include "core/wrapper/Libraries.h"
#include "core/wrapper/ConfigurationTraits.h"

#include "core/wrapper/SharedMemoryFactoryWorker.h"
#include "core/wrapper/SystemLibraryProducts.h"

#ifdef WIN32
    #include "core/wrapper/WIN32/WIN32SharedMemoryFactoryWorker.h"
#endif
#ifndef WIN32
    #include "core/wrapper/POSIX/POSIXSharedMemoryFactoryWorker.h"
#endif

namespace core {
    namespace wrapper {

        SharedPointer<SharedMemory> SharedMemoryFactory::createSharedMemory(const string &name, const uint32_t &size)
        {
            typedef ConfigurationTraits<SystemLibraryProducts>::configuration configuration;

            return SharedMemoryFactoryWorker<configuration::value>::createSharedMemory(name, size);
        }

        SharedPointer<SharedMemory> SharedMemoryFactory::attachToSharedMemory(const string &name)
        {
            typedef ConfigurationTraits<SystemLibraryProducts>::configuration configuration;

            return SharedMemoryFactoryWorker<configuration::value>::attachToSharedMemory(name);
        }
    }
} // core::wrapper
