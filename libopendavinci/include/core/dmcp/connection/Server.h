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

#ifndef OPENDAVINCI_DMCP_CONNECTION_SERVER_H_
#define OPENDAVINCI_DMCP_CONNECTION_SERVER_H_

// core/platform.h must be included to setup platform-dependent header files and configurations.
#include "core/platform.h"

#include "core/SharedPointer.h"

#include "core/io/Connection.h"
#include "core/io/ConnectionAcceptor.h"
#include "core/io/ConnectionAcceptorListener.h"

#include "core/dmcp/ServerInformation.h"
#include "core/dmcp/connection/ConnectionHandler.h"

namespace core {
    namespace dmcp {
        namespace connection {

            class OPENDAVINCI_API Server : protected core::io::ConnectionAcceptorListener
            {
                public:
                    Server(const ServerInformation& serverInformation,
                           ModuleConfigurationProvider& configProvider);
                    virtual ~Server();

                    void setConnectionHandler(ConnectionHandler* connectionHandler);

                protected:
                    virtual void onNewConnection(core::io::Connection* connection);

                    core::base::Mutex m_configProviderMutex;
                    ModuleConfigurationProvider& m_configProvider;

                    core::base::Mutex m_connectionHandlerMutex;
                    ConnectionHandler* m_connectionHandler;

                    core::io::ConnectionAcceptor m_acceptor;

                private:
                    Server(const Server &);
                    Server& operator=(const Server &);

            };
        }
    }
} // core::dmcp

#endif  /*OPENDAVINCI_DMCP_CONNECTION_SERVER_H_*/
