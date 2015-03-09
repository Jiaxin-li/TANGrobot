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

#ifndef SUPERCOMPONENTCLIENTMODULETESTSUITE_H_
#define SUPERCOMPONENTCLIENTMODULETESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "core/SharedPointer.h"
#include "core/base/ModuleState.h"
#include "core/base/FIFOQueue.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/base/Lock.h"
#include "core/base/Mutex.h"
#include "core/base/Service.h"
#include "core/base/Thread.h"
#include "core/exceptions/Exceptions.h"

#include "core/base/ConferenceClientModule.h"
#include "core/dmcp/ModuleConfigurationProvider.h"
#include "core/dmcp/discoverer/Server.h"
#include "core/dmcp/connection/Server.h"
#include "core/dmcp/connection/ConnectionHandler.h"
#include "core/dmcp/connection/ModuleConnection.h"

#include "../include/GlobalConfigurationProvider.h"

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::exceptions;
using namespace core::data::dmcp;
using namespace core::dmcp;
using namespace core::io;

class ClientModuleApp : public ConferenceClientModule {
    public:
        ClientModuleApp(const int32_t &argc, char **argv, const string &name) :
            ConferenceClientModule(argc, argv, name) {}

        const KeyValueConfiguration getKVC() const {
            return getKeyValueConfiguration();
        }

        void setUp() {}

        void tearDown() {}

        core::base::ModuleState::MODULE_EXITCODE body() {
            return core::base::ModuleState::OKAY;
        }
};

class ClientModuleTestService : public Service {
    public:
        ClientModuleTestService(const int32_t &argc, char **argv, const string &name) :
            myApp(argc, argv, name),
            m_hasConfigurationReceivedMutex(),
            m_hasConfigurationReceived(false) {}

        virtual void beforeStop() {
            // Stop app.
            myApp.setModuleState(ModuleState::NOT_RUNNING);
        }

        virtual void run() {
            serviceReady();
            myApp.runModule();

            Lock l(m_hasConfigurationReceivedMutex);
            m_hasConfigurationReceived = true;
        }

        const KeyValueConfiguration getKVC() const {
            return myApp.getKVC();
        }

        bool hasConfigurationReceived() const {
            bool retVal = false;
            {
                Lock l(m_hasConfigurationReceivedMutex);
                retVal = m_hasConfigurationReceived;
            }
            return retVal;
        }

    private:
        ClientModuleApp myApp;
        mutable Mutex m_hasConfigurationReceivedMutex;
        bool m_hasConfigurationReceived;
};

class SupercomponentClientModuleTest : public CxxTest::TestSuite,
            public connection::ConnectionHandler,
            public ModuleConfigurationProvider {
    public:
        SupercomponentClientModuleTest() :
            m_globaleConfigurationProvider(),
            m_connection() {}

        supercomponent::GlobalConfigurationProvider m_globaleConfigurationProvider;
        core::SharedPointer<connection::ModuleConnection> m_connection;

        virtual KeyValueConfiguration getConfiguration(const ModuleDescriptor &md) {
            return m_globaleConfigurationProvider.getConfiguration(md);
        }

        virtual void onNewModule(connection::ModuleConnection* mc) {
            m_connection = core::SharedPointer<connection::ModuleConnection>(mc);
        }

        void testServerInformation() {
            ServerInformation s1("127.0.0.1", 19000);
            ServerInformation s2;

            stringstream sstr;
            sstr << s1;
            sstr >> s2;

            cerr << s1.toString() << endl;

            TS_ASSERT(s1 == s2);

            ServerInformation s3("127.0.0.1", 19000, ServerInformation::ML_PULSE);
            ServerInformation s4;

            stringstream sstr2;
            sstr2 << s3;
            sstr2 >> s4;

            cerr << s4.toString() << endl;

            TS_ASSERT(s3 == s4);
        }

        void testRegularConfiguration() {
            // Setup DMCP.
            stringstream sstr;
            sstr << "global.key1 = value1" << endl
                 << "otherapp.key2 = value2" << endl
                 << "clientmoduletestservice.key3 = value3" << endl
                 << "clientmoduletestservice:1.key4 = value4" << endl;

            KeyValueConfiguration _configuration;
            sstr >> _configuration;
            m_globaleConfigurationProvider = supercomponent::GlobalConfigurationProvider(_configuration);

            vector<string> noModulesToIgnore;
            ServerInformation serverInformation("127.0.0.1", 19000);
            discoverer::Server dmcpDiscovererServer(serverInformation,
                                                    "225.0.0.100",
                                                    BROADCAST_PORT_SERVER,
                                                    BROADCAST_PORT_CLIENT,
                                                    noModulesToIgnore);
            dmcpDiscovererServer.startResponding();

            connection::Server dmcpConnectionServer(serverInformation, *this);
            dmcpConnectionServer.setConnectionHandler(this);

            // Setup player.
            string argv0("ClientModuleTestService");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            ClientModuleTestService cms(argc, argv, "ClientModuleTestService");

            cms.start();

            uint32_t tries = 0;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while ( (!cms.hasConfigurationReceived()) && (tries < 5) ) {
                Thread::usleep(ONE_SECOND);
                tries++;
            }

            cms.stop();

            TS_ASSERT(cms.hasConfigurationReceived());

            const KeyValueConfiguration kvc = cms.getKVC();

            TS_ASSERT(kvc.getValue<string>("global.key1") == "value1");
            TS_ASSERT(kvc.getValue<string>("clientmoduletestservice.key3") == "value3");

            bool noOtherAppKey2Received = false;
            try {
                kvc.getValue<string>("otherapp.key2");
            }
            catch (ValueForKeyNotFoundException &) {
                noOtherAppKey2Received = true;
            }
            TS_ASSERT(noOtherAppKey2Received);

            bool noKey4Received = false;
            try {
                kvc.getValue<string>("clientmoduletestservice:1.key4");
            }
            catch (ValueForKeyNotFoundException &) {
                noKey4Received = true;
            }
            TS_ASSERT(noKey4Received);

            bool noKey4WithOutIdReceived = false;
            try {
                kvc.getValue<string>("clientmoduletestservice.key4");
            }
            catch (ValueForKeyNotFoundException &) {
                noKey4WithOutIdReceived = true;
            }
            TS_ASSERT(noKey4WithOutIdReceived);
        }

        void testConfigurationForModuleWithID() {
            // Setup DMCP.
            stringstream sstr;
            sstr << "global.key1 = value1" << endl
                 << "otherapp.key2 = value2" << endl
                 << "clientmoduletestservice.key3 = value3" << endl
                 << "clientmoduletestservice:1.key4 = value4" << endl;

            KeyValueConfiguration _configuration;
            sstr >> _configuration;
            m_globaleConfigurationProvider = supercomponent::GlobalConfigurationProvider(_configuration);

            vector<string> noModulesToIgnore;
            ServerInformation serverInformation("127.0.0.1", 19000);
            discoverer::Server dmcpDiscovererServer(serverInformation,
                                                    "225.0.0.100",
                                                    BROADCAST_PORT_SERVER,
                                                    BROADCAST_PORT_CLIENT,
                                                    noModulesToIgnore);
            dmcpDiscovererServer.startResponding();

            connection::Server dmcpConnectionServer(serverInformation, *this);
            dmcpConnectionServer.setConnectionHandler(this);

            // Setup player.
            string argv0("ClientModuleTestService");
            string argv1("--cid=100");
            string argv2("--id=1");
            int32_t argc = 3;
            char **argv;
            argv = new char*[3];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());

            ClientModuleTestService cms(argc, argv, "ClientModuleTestService");

            cms.start();

            uint32_t tries = 0;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while ( (!cms.hasConfigurationReceived()) && (tries < 5) ) {
                Thread::usleep(ONE_SECOND);
                tries++;
            }

            cms.stop();

            TS_ASSERT(cms.hasConfigurationReceived());

            const KeyValueConfiguration kvc = cms.getKVC();

            TS_ASSERT(kvc.getValue<string>("global.key1") == "value1");
            TS_ASSERT(kvc.getValue<string>("clientmoduletestservice.key3") == "value3");

            bool noOtherAppKey2Received = false;
            try {
                kvc.getValue<string>("otherapp.key2");
            }
            catch (ValueForKeyNotFoundException &) {
                noOtherAppKey2Received = true;
            }
            TS_ASSERT(noOtherAppKey2Received);

            // IDs are removed from keys upon broadcast.
            bool noKey4WithIDReceived = false;
            try {
                kvc.getValue<string>("clientmoduletestservice:1.key4");
            }
            catch (ValueForKeyNotFoundException &) {
                noKey4WithIDReceived = true;
            }
            TS_ASSERT(noKey4WithIDReceived);

            TS_ASSERT(kvc.getValue<string>("clientmoduletestservice.key4") == "value4");
        }

        void testConfigurationForModuleWithIDEmptyConfiguration() {
            // Setup DMCP.
            stringstream sstr;
            sstr << "global.key1 = value1" << endl
                 << "otherapp.key2 = value2" << endl
                 << "clientmoduletestservice.key3 = value3" << endl
                 << "clientmoduletestservice:1.key4 = value4" << endl;

            KeyValueConfiguration _configuration;
            sstr >> _configuration;
            m_globaleConfigurationProvider = supercomponent::GlobalConfigurationProvider(_configuration);

            vector<string> noModulesToIgnore;
            ServerInformation serverInformation("127.0.0.1", 19000);
            discoverer::Server dmcpDiscovererServer(serverInformation,
                                                    "225.0.0.100",
                                                    BROADCAST_PORT_SERVER,
                                                    BROADCAST_PORT_CLIENT,
                                                    noModulesToIgnore);
            dmcpDiscovererServer.startResponding();

            connection::Server dmcpConnectionServer(serverInformation, *this);
            dmcpConnectionServer.setConnectionHandler(this);

            // Setup player.
            string argv0("ClientModuleTestService");
            string argv1("--cid=100");
            string argv2("--id=2");
            int32_t argc = 3;
            char **argv;
            argv = new char*[3];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());

            ClientModuleTestService cms(argc, argv, "ClientModuleTestService");

            cms.start();

            uint32_t tries = 0;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while ( (!cms.hasConfigurationReceived()) && (tries < 5) ) {
                Thread::usleep(ONE_SECOND);
                tries++;
            }

            cms.stop();

            TS_ASSERT(cms.hasConfigurationReceived());

            const KeyValueConfiguration kvc = cms.getKVC();

            TS_ASSERT(kvc.getValue<string>("global.key1") == "value1");
            TS_ASSERT(kvc.getValue<string>("clientmoduletestservice.key3") == "value3");

            bool noOtherAppKey2Received = false;
            try {
                kvc.getValue<string>("otherapp.key2");
            }
            catch (ValueForKeyNotFoundException &) {
                noOtherAppKey2Received = true;
            }
            TS_ASSERT(noOtherAppKey2Received);

            bool noKey4Received = false;
            try {
                kvc.getValue<string>("clientmoduletestservice:1.key4");
            }
            catch (ValueForKeyNotFoundException &) {
                noKey4Received = true;
            }
            TS_ASSERT(noKey4Received);

            bool noKey4WithOutIdReceived = false;
            try {
                kvc.getValue<string>("clientmoduletestservice.key4");
            }
            catch (ValueForKeyNotFoundException &) {
                noKey4WithOutIdReceived = true;
            }
            TS_ASSERT(noKey4WithOutIdReceived);
        }
};

#endif /*SUPERCOMPONENTCLIENTMODULETESTSUITE_H_*/
