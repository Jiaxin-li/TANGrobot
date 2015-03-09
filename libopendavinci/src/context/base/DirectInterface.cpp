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

#include <sstream>

#include "context/base/DirectInterface.h"

namespace context {
    namespace base {

        using namespace std;
        using namespace core::base;

        DirectInterface::DirectInterface(const string &multicastGroup, const uint32_t &CID, const string &configuration) :
            m_configuration(),
            m_multicastGroup(multicastGroup),
            m_CID(CID) {
            // Parse configuration data.
            stringstream sstr;
            sstr.str(configuration);
            sstr >> m_configuration;
        }

        DirectInterface::~DirectInterface() {}

        const KeyValueConfiguration DirectInterface::getConfiguration() const {
            return m_configuration;
        }

        const string DirectInterface::getMulticastGroup() const {
            return m_multicastGroup;
        }

        uint32_t DirectInterface::getCID() const {
            return m_CID;
        }

        bool DirectInterface::isVerbose() const {
        	return true;
        }

        bool DirectInterface::isSupercomponent() const {
        	return true;
        }
    }
} // context::base
