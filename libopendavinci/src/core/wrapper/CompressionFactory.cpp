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

#include "core/wrapper/CompressionFactory.h"
#include "core/wrapper/CompressionFactoryWorker.h"
#include "core/wrapper/Libraries.h"
#include "core/wrapper/CompressionLibraryProducts.h"
#include "core/wrapper/ConfigurationTraits.h"

#include "core/wrapper/Zip/ZipCompressionFactoryWorker.h"

namespace core {
    namespace wrapper {

        DecompressedData* CompressionFactory::getContents(istream &in)
        {
            typedef ConfigurationTraits<CompressionLibraryProducts>::configuration configuration;

            return CompressionFactoryWorker<configuration::value>::getContents(in);
        }
    }
} // core::wrapper
