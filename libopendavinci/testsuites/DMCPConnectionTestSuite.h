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

#ifndef CORE_DMCPCONNECTIONTESTSUITE_H_
#define CORE_DMCPCONNECTIONTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <sstream>
#include <string>
#include <memory>

#include "core/base/KeyValueConfiguration.h"
#include "core/exceptions/Exceptions.h"

#include "mocks/ModuleConfigurationProviderMock.h"
#include "mocks/ConnectionHandlerMock.h"

#include "core/data/dmcp/ModuleDescriptor.h"
#include "core/dmcp/Config.h"
#include "core/dmcp/ServerInformation.h"
#include "core/dmcp/connection/Client.h"
#include "core/dmcp/connection/ConnectionHandler.h"
#include "core/dmcp/connection/ModuleConnection.h"
#include "core/dmcp/connection/Server.h"

using namespace std;

using namespace core::base;
using namespace core::exceptions;
using namespace core::dmcp;
using namespace core::data;
using namespace core::data::dmcp;

class DMCPConnectionTestsuite : public CxxTest::TestSuite,
                                public connection::ConnectionHandler
{
    private:
        DMCPConnectionTestsuite(const DMCPConnectionTestsuite& /*obj*/);
        DMCPConnectionTestsuite& operator=(const DMCPConnectionTestsuite& /*obj*/);

    public:
        DMCPConnectionTestsuite() :
            connection(NULL) {}

        connection::ModuleConnection* connection;

        void testClientAndServer()
        {
            clog << endl << "DMCPConnectionTestsuite::testClientAndServer" << endl;
            connection = NULL;
            stringstream sstr;
            sstr << "global.exampleKey = exampleValue" << endl;
            sstr << "NAME.key=Test" << endl;

            KeyValueConfiguration kv;
            sstr >> kv;

            mocks::ModuleConfigurationProviderMock configProvider;
            mocks::ConnectionHandlerMock connectionHandler;

            ServerInformation serverInfo("127.0.0.1", DMCPConfig_TEST_SERVERPORT);
            ModuleDescriptor descriptor("DMCPConnectionTestSuite", "NONE", "TestVersion", 1);

            configProvider.addConfig(descriptor, kv);

            connection::Server server(serverInfo, configProvider);
            server.setConnectionHandler(&connectionHandler);

            connection::Client client(descriptor, serverInfo);
            TS_ASSERT(connectionHandler.WAITER.wait());
            TS_ASSERT(connectionHandler.WAITER.wasCalled());

            client.initialize();

            TS_ASSERT(client.getConfiguration().getValue<string>("NAME.Key") == "Test");
            TS_ASSERT(client.getConfiguration().getValue<string>("global.exampleKey") == "exampleValue");
        }

        virtual void onNewModule(connection::ModuleConnection* mc)
        {
            connection = mc;
        }
};

#endif

