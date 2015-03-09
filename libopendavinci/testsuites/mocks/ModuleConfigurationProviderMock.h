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

#ifndef MODULECONFIGURATIONLISTENERMOCK_H_
#define MODULECONFIGURATIONLISTENERMOCK_H_

// core/platform.h must be included to setup platform-dependent header files and configurations.
#include "core/platform.h"

#include "core/base/KeyValueConfiguration.h"
#include "core/dmcp/ModuleConfigurationProvider.h"
#include "core/data/dmcp/ModuleDescriptor.h"
#include "core/data/dmcp/ModuleDescriptorComparator.h"

namespace mocks {
    class ModuleConfigurationProviderMock : public core::dmcp::ModuleConfigurationProvider {
        public:
            ModuleConfigurationProviderMock() : m_configs() {};
            virtual ~ModuleConfigurationProviderMock() {};

            void addConfig(const core::data::dmcp::ModuleDescriptor& md,
                           const core::base::KeyValueConfiguration& kv)
            {
                m_configs[md] = kv;
            }

            virtual core::base::KeyValueConfiguration getConfiguration(const core::data::dmcp::ModuleDescriptor& md)
            {
                return m_configs[md];
            };

        private:
            std::map<core::data::dmcp::ModuleDescriptor, core::base::KeyValueConfiguration, core::data::dmcp::ModuleDescriptorComparator> m_configs;
    };
}
#endif /* MODULECONFIGURATIONLISTENERMOCK_H_ */
