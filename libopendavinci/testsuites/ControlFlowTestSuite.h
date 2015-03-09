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

#ifndef CONTEXT_CONTROLFLOWTESTSUITE_H_
#define CONTEXT_CONTROLFLOWTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "core/macros.h"
#include "core/base/Breakpoint.h"
#include "core/base/Condition.h"
#include "core/base/Deserializer.h"
#include "core/base/Hash.h"
#include "core/base/FIFOQueue.h"
#include "core/base/Lock.h"
#include "core/base/Service.h"
#include "core/base/Serializable.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"
#include "core/base/Thread.h"
#include "core/data/Container.h"
#include "core/data/SerializableData.h"
#include "core/data/TimeStamp.h"
#include "core/io/ContainerConferenceFactory.h"
#include "core/io/ContainerListener.h"
#include "core/io/UDPMultiCastContainerConference.h"

#include "core/base/ConferenceClientModule.h"
#include "core/dmcp/ModuleConfigurationProvider.h"
#include "core/dmcp/discoverer/Server.h"
#include "core/dmcp/connection/Server.h"
#include "core/dmcp/connection/ConnectionHandler.h"
#include "core/dmcp/connection/ModuleConnection.h"

#include "context/base/ControlledContainerConference.h"
#include "context/base/ControlledContainerConferenceForSystemUnderTest.h"
#include "context/base/ControlledContainerConferenceFactory.h"

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::io;
using namespace core::data::dmcp;
using namespace core::dmcp;
using namespace context::base;

////////////////////////////////////////////////////////////////////////////////
//
// Breakpoint for interrupting the inner thread of the application.
//
////////////////////////////////////////////////////////////////////////////////

class ControlFlowTestBreakpoint : public core::base::Breakpoint {
    public:
        ControlFlowTestBreakpoint() :
            m_reachedMutex(),
            m_reached(false),
            m_continueMutex(),
            m_continue(false) {}

        virtual void reached() {
            // Indicate the outer thread that the inner thread has reached its breakpoint.
            {
                Lock l1(m_reachedMutex);
                m_reached = true;
            }

            while (!hasContinue()) {
                Thread::usleep(1000);
            }

            {
                Lock l3(m_continueMutex);
                m_continue = false;
            }
        }

        bool hasReached() {
            bool retVal = false;
            {
                Lock l(m_reachedMutex);
                retVal = m_reached;
            }
            return retVal;
        }

        bool hasContinue() {
            bool retVal = false;
            {
                Lock l1(m_continueMutex);
                retVal = m_continue;
            }
            return retVal;
        }

        void continueExecution() {
            {
                Lock l1(m_continueMutex);
                m_continue = true;
            }
            {
                Lock l2(m_reachedMutex);
                m_reached = false;
            }
        }

    private:
        Mutex m_reachedMutex;
        bool m_reached;

        Mutex m_continueMutex;
        bool m_continue;
};

////////////////////////////////////////////////////////////////////////////////
//
// ControlFlow Test Application.
//
////////////////////////////////////////////////////////////////////////////////

class ControlFlowTestSampleData : public core::data::SerializableData {
    public:
        ControlFlowTestSampleData() :
                m_int(0) {}

        uint32_t m_int;

        const string toString() const {
            stringstream sstr;
            sstr << m_int;
            return sstr.str();
        }

        ostream& operator<<(ostream &out) const {
            SerializationFactory sf;

            Serializer &s = sf.getSerializer(out);

            s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 'i', 'n', 't') >::RESULT,
                    m_int);

            return out;
        }

        istream& operator>>(istream &in) {
            SerializationFactory sf;

            Deserializer &d = sf.getDeserializer(in);

            d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 'i', 'n', 't') >::RESULT,
                   m_int);

            return in;
        }
};

class ControlFlowTestApp : public ConferenceClientModule {
    public:
        ControlFlowTestApp(const int32_t &argc, char **argv, const string &name) :
            ConferenceClientModule(argc, argv, name),
            m_counter(0) {}

        void setUp() {}

        void tearDown() {}

        core::base::ModuleState::MODULE_EXITCODE body() {
            FIFOQueue myFIFO;
            addDataStoreFor(myFIFO);
            while (getModuleState() == ModuleState::RUNNING) {
                const uint32_t SIZE = myFIFO.getSize();
                for(uint32_t i = 0; i < SIZE; i++) {
                    Container c = myFIFO.leave();
                    if (c.getDataType() == Container::TIMESTAMP) {
                        TimeStamp ts = c.getData<TimeStamp>();
                        clog << "ControlFlowTestApp received '" << ts.toString() << "'" << endl;

                        // Compute some data.
                        m_counter++;

                        // Send some data.
                        ControlFlowTestSampleData cftsd;
                        cftsd.m_int = m_counter;
                        Container c2(Container::UNDEFINEDDATA, cftsd);
                        getConference().send(c2);
                    }
                }

            }
            return core::base::ModuleState::OKAY;
        }

    private:
        uint32_t m_counter;
};

class ControlFlowTestService : public Service {
    public:
        ControlFlowTestService(const int32_t &argc, char **argv, const string &name, core::base::Breakpoint *bp) :
            myApp(argc, argv, name) {
            myApp.setBreakpoint(bp);
        }

        virtual void beforeStop() {
            // Stop app.
            myApp.setModuleState(ModuleState::NOT_RUNNING);
        }

        virtual void run() {
            serviceReady();
            myApp.runModule();
        }

        BlockableContainerReceiver& getBlockableContainerReceiver() {
            return dynamic_cast<ControlledContainerConferenceForSystemUnderTest&>(myApp.getConference()).getBlockableContainerReceiver();
        }

    private:
        ControlFlowTestApp myApp;
};

class ControlFlowTestContainerReceiverFromSystemUnderTest : public core::io::ContainerListener {
    public:
        ControlFlowTestContainerReceiverFromSystemUnderTest() :
            queueContainingContainersSentToConferenceBySystemUnderTest() {}

        virtual void nextContainer(core::data::Container &c) {
            queueContainingContainersSentToConferenceBySystemUnderTest.add(c);
        }

        FIFOQueue& getFIFO() {
            return queueContainingContainersSentToConferenceBySystemUnderTest;
        }

    private:
        FIFOQueue queueContainingContainersSentToConferenceBySystemUnderTest;
};

////////////////////////////////////////////////////////////////////////////////

class ControlFlowTest : public CxxTest::TestSuite,
                        public connection::ConnectionHandler,
                        public ModuleConfigurationProvider{
    public:
        ControlFlowTest() :
            m_configuration(),
            m_connection() {}

        KeyValueConfiguration m_configuration;
        core::SharedPointer<connection::ModuleConnection> m_connection;

        virtual KeyValueConfiguration getConfiguration(const ModuleDescriptor& /*md*/) {
            return m_configuration;
        }

        virtual void onNewModule(connection::ModuleConnection* mc) {
            m_connection = core::SharedPointer<connection::ModuleConnection>(mc);
        }

        void testControlledContainerFactoryTestSuite() {
            // Destroy any existing ContainerConferenceFactory.
            ContainerConferenceFactory &ccf = ContainerConferenceFactory::getInstance();
            ContainerConferenceFactory *ccf2 = &ccf;
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);

            // Create controlled ContainerConferenceFactory.
            ControlledContainerConferenceFactory *controlledCF = new ControlledContainerConferenceFactory();
            ContainerConferenceFactory &controlledccf = ContainerConferenceFactory::getInstance();
            ccf2 = &controlledccf;
            TS_ASSERT(ccf2 == controlledCF);

            // Get ControlledContainerConference.
            ContainerConference *cf = controlledccf.getContainerConference("testControlledContainerFactoryTestSuite");
            ControlledContainerConferenceForSystemUnderTest *controlledConferenceForSystemUnderTest = NULL;
            TS_ASSERT(cf != NULL);
            bool castIntoCCFSuccessful = false;
            try {
                controlledConferenceForSystemUnderTest = dynamic_cast<ControlledContainerConferenceForSystemUnderTest*>(cf);
                if (controlledConferenceForSystemUnderTest != NULL) {
                    castIntoCCFSuccessful = true;
                }
            }
            catch(...) {}
            TS_ASSERT(castIntoCCFSuccessful);
            TS_ASSERT(controlledConferenceForSystemUnderTest != NULL);

            // Register a ContainerListener at ControlledContainerConferenceFactory to get Containers sent by System Under Test.
            ControlFlowTestContainerReceiverFromSystemUnderTest cftcrfcut;
            controlledCF->add(&cftcrfcut);

            // Create application to test.
            // Setup DMCP.
            stringstream sstr;
            sstr << "ControlFlowTestApp.key = value" << endl;
            m_configuration = KeyValueConfiguration();
            sstr >> m_configuration;

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

            // Setup recorder.
            string argv0("ControlFlowTestApp");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            ControlFlowTestBreakpoint controlflowBreakpoint;
            ControlFlowTestService cfts(argc, argv, argv0, &controlflowBreakpoint);
            cfts.start();

            for(uint32_t i = 0; i < 5; i++) {
                while (!controlflowBreakpoint.hasReached()) {
                    Thread::usleep(1000);
                }
                cfts.getBlockableContainerReceiver().setNextContainerAllowed(false);

                clog << "Do some computation." << endl;

                // Send to application.
                TimeStamp tsSendFromSimulatorToContainerConference(i, i+1);
                Container c = Container(Container::TIMESTAMP, tsSendFromSimulatorToContainerConference);
                controlledCF->sendToSystemsUnderTest(c);

                cfts.getBlockableContainerReceiver().setNextContainerAllowed(true);
                controlflowBreakpoint.continueExecution();
            }

            {
                cfts.getBlockableContainerReceiver().setNextContainerAllowed(true);
                // Break last waiting.
                controlflowBreakpoint.continueExecution();
                cfts.getBlockableContainerReceiver().setNextContainerAllowed(false);
            }

            cfts.stop();

            Thread::usleep(10 * 1000);

            // Check received data from application.
            FIFOQueue &queueContainingContainersSentToConference = cftcrfcut.getFIFO();
            clog << "Received: " << queueContainingContainersSentToConference.getSize() << " container." << endl;
            TS_ASSERT(queueContainingContainersSentToConference.getSize() > 3);

            uint32_t undefDataCnt = 0;
            for(uint32_t i = 0; i < queueContainingContainersSentToConference.getSize(); i++) {
                Container c = queueContainingContainersSentToConference.leave();
                if (c.getDataType() == Container::UNDEFINEDDATA) {
                    undefDataCnt++;
                    ControlFlowTestSampleData data = c.getData<ControlFlowTestSampleData>();
                    TS_ASSERT(data.m_int == undefDataCnt);
                }
            }

            // Destroy existing ContainerConferenceFactory.
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);
            OPENDAVINCI_CORE_DELETE_POINTER(controlledConferenceForSystemUnderTest);
        }
};

#endif /*CONTEXT_CONTROLFLOWTESTSUITE_H_*/
