/**
 * supercomponent - Configuration and monitoring component for
 *                  distributed software systems
 * Copyright (C) 2008 - 2015 Christian Berger, Bernhard Rumpe 
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "ConnectedModule.h"

namespace supercomponent {

    using namespace std;
    using namespace core::base;
    using namespace core::dmcp::connection;

    ConnectedModule::ConnectedModule(ModuleConnection* connection, const ModuleState::MODULE_STATE& state) :
        m_state(state),
        m_connection(connection),
        m_hasExitCode(false)
    {}

    ConnectedModule::~ConnectedModule() {
        delete m_connection;
    }

    ModuleState::MODULE_STATE ConnectedModule::getState() const {
        return m_state;
    }

    void ConnectedModule::setExitCode() {
        m_hasExitCode = true;
    }

    bool ConnectedModule::hasExitCode() const {
        return m_hasExitCode;
    }

    ModuleConnection& ConnectedModule::getConnection() {
        return *m_connection;
    }

    void ConnectedModule::setState(const ModuleState::MODULE_STATE& state) {
        m_state = state;
    }
}
