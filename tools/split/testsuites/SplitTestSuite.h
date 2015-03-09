/**
 * split- Tool for splitting recorded data
 * Copyright (C) 2015 Christian Berger
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

#ifndef RECINTEGRITYTESTSUITE_H_
#define RECINTEGRITYTESTSUITE_H_

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include "cxxtest/TestSuite.h"

#include "core/SharedPointer.h"
#include "core/StringToolbox.h"
#include "core/base/ModuleState.h"
#include "core/data/Container.h"
#include "core/data/SharedData.h"
#include "core/data/TimeStamp.h"
#include "core/io/ContainerConference.h"
#include "core/io/ContainerConferenceFactory.h"
#include "core/dmcp/ModuleConfigurationProvider.h"
#include "core/dmcp/discoverer/Server.h"
#include "core/dmcp/connection/Server.h"
#include "core/dmcp/connection/ConnectionHandler.h"
#include "core/dmcp/connection/ModuleConnection.h"
#include "core/wrapper/SharedMemory.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "tools/player/Player.h"
#include "tools/recorder/Recorder.h"

// Include local header files.
#include "../include/Split.h"

using namespace std;
using namespace core;
using namespace core::base;
using namespace core::data;
using namespace core::data::dmcp;
using namespace core::dmcp;
using namespace core::io;
using namespace tools::player;
using namespace tools::recorder;
using namespace split;

/**
 * This class derives from Split to allow access to protected methods.
 */
class SplitTestling : public Split {
    private:
        SplitTestling();
    
    public:
        SplitTestling(const int32_t &argc, char **argv) :
            Split(argc, argv) {}

        // Here, you need to add all methods which are protected in Split and which are needed for the test cases.
};

// Size of the memory buffer.
const uint32_t MEMORY_SEGMENT_SIZE = 100;

// Number of memory segments can be set to a fixed value.
const uint32_t NUMBER_OF_SEGMENTS = 1;

/**
 * The actual testsuite starts here.
 */
class SplitTest : public CxxTest::TestSuite,
                     public connection::ConnectionHandler,
                     public ModuleConfigurationProvider {
    private:
        SplitTestling *dt;

    public:
        SplitTest() :
            dt(NULL),
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

        void setUp() {
            // Run recorder in synchronous mode.
            const bool THREADING = false;
            const string file("file://A.rec");
            Recorder recorder(file, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);        

            {
                core::SharedPointer<core::wrapper::SharedMemory> memServer = core::wrapper::SharedMemoryFactory::createSharedMemory("SharedMemoryServer", 50);
                TS_ASSERT(memServer->isValid());
                TS_ASSERT(memServer->getSize() == 50);

                // Create some test data.
                for(uint32_t i = 0; i < 200; i++) {
                    // Create regular container.
                    TimeStamp t(i, 0);
                    Container c(Container::TIMESTAMP, t);
                    c.setReceivedTimeStamp(TimeStamp(i,500));
                    recorder.store(c);

                    // Create shared memory.
                    memServer->lock();
                        stringstream sstr;
                        sstr << "Data-" << i << endl;
                        memcpy(memServer->getSharedMemory(), sstr.str().c_str(), sstr.str().size());
                    memServer->unlock();

                    SharedData sd(memServer->getName(), memServer->getSize());

                    Container c2(Container::SHARED_DATA, sd);
                    c2.setReceivedTimeStamp(TimeStamp(i,1000));
                    recorder.store(c2);
                }
            }
        }

        void tearDown() {
            UNLINK("A.rec");
            UNLINK("A.rec.mem");
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the actual testcases are defined.
        ////////////////////////////////////////////////////////////////////////////////////

        void testSplitSuccessfullySplit() {
            // Setup ContainerConference.
            ContainerConference *conference = ContainerConferenceFactory::getInstance().getContainerConference("225.0.0.100");

            // Setup DMCP.
            stringstream sstr;
            sstr << "recorder.output = file://RecorderTest.rec" << endl
            << "global.buffer.memorySegmentSize = 1000" << endl
            << "global.buffer.numberOfMemorySegments = 1" << endl
            << "recorder.remoteControl = 0" << endl;

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

            // Prepare the data that would be available from commandline.
            string argv0("split");
            string argv1("--cid=100");
            string argv2("--freq=100");
            string argv3("--source=A.rec");
            string argv4("--range=50-60");
            int32_t argc = 5;
            char **argv;
            argv = new char*[5];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());
            argv[3] = const_cast<char*>(argv3.c_str());
            argv[4] = const_cast<char*>(argv4.c_str());

            // Create an instance of sensorboard through SensorBoardTestling which will be deleted in tearDown().
            dt = new SplitTestling(argc, argv);

            // Run the split.
            TS_ASSERT(dt != NULL);
            TS_ASSERT(dt->runModule() == ModuleState::OKAY);

            delete dt;
            dt = NULL;

            // Compare the split.

            // Stop playback at EOF.
            const bool AUTO_REWIND = false;
            // Run player in synchronous mode.
            const bool THREADING = false;
            // Construct player.
            string file("file://A.rec_50-60.rec");
            Player player(file, AUTO_REWIND, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);

            int32_t rangeBasis = 25;
            int32_t sharedMemorySegments = 0;
            const uint32_t MAX_ITERATIONS = 1000;
            uint32_t i = 0;

            core::SharedPointer<core::wrapper::SharedMemory> memClient;

            while (player.hasMoreData() && (i < MAX_ITERATIONS)) {
                i++;
                // Get container to be sent.
                Container nextContainer = player.getNextContainerToBeSent();

                if (nextContainer.getDataType() == Container::TIMESTAMP) {
                    TimeStamp ts = nextContainer.getData<TimeStamp>();
                    TS_ASSERT(ts.getSeconds() == rangeBasis);
                    rangeBasis++;
                }
                else if (nextContainer.getDataType() == Container::SHARED_DATA) {
                    if (!memClient.isValid()) {
                        SharedData sd = nextContainer.getData<SharedData>();
                        memClient = core::wrapper::SharedMemoryFactory::attachToSharedMemory(sd.getName());
                    }

                    TS_ASSERT(memClient->isValid());
                    TS_ASSERT(memClient->getSize() == 50);
                    memClient->lock();
                        char *c = (char*)(memClient->getSharedMemory());
                        string s(c);

                        stringstream sstr2;
                        sstr2 << "Data-" << (rangeBasis-1) << endl;

                        TS_ASSERT(core::StringToolbox::equalsIgnoreCase(s, sstr2.str()));
                    memClient->unlock();

                    sharedMemorySegments++;
                }
            }

            // As the range is given as 50-60, the 6 TimeStamps (delivered at 25, 26, 27, ..., counter will be 31 at the end) and 5 SharedMemory segments will be handled.
            TS_ASSERT(rangeBasis == 31);
            TS_ASSERT(sharedMemorySegments == 5);

            // "Ugly" cleaning up conference.
            OPENDAVINCI_CORE_DELETE_POINTER(conference);
            ContainerConferenceFactory &ccf = ContainerConferenceFactory::getInstance();
            ContainerConferenceFactory *ccf2 = &ccf;
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);

            // Clean up temporarily created files.
            UNLINK("A.rec_50-60.rec");
            UNLINK("A.rec_50-60.rec.mem");
        }

        void testSplitWrongRange() {
            // Setup DMCP.
            stringstream sstr;
            sstr << "recorder.output = file://RecorderTest.rec" << endl
            << "global.buffer.memorySegmentSize = 1000" << endl
            << "global.buffer.numberOfMemorySegments = 1" << endl
            << "recorder.remoteControl = 0" << endl;

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

            // Prepare the data that would be available from commandline.
            string argv0("split");
            string argv1("--cid=100");
            string argv2("--freq=100");
            string argv3("--source=A.rec");
            string argv4("--range=70-60");
            int32_t argc = 5;
            char **argv;
            argv = new char*[5];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());
            argv[3] = const_cast<char*>(argv3.c_str());
            argv[4] = const_cast<char*>(argv4.c_str());

            // Create an instance of sensorboard through SensorBoardTestling which will be deleted in tearDown().
            dt = new SplitTestling(argc, argv);

            // Run the split.
            TS_ASSERT(dt != NULL);
            TS_ASSERT(dt->runModule() == ModuleState::SERIOUS_ERROR);

            delete dt;
            dt = NULL;
        }

        ////////////////////////////////////////////////////////////////////////////////////
        // Below this line the necessary constructor for initializing the pointer variables,
        // and the forbidden copy constructor and assignment operator are declared.
        //
        // These functions are normally not changed.
        ////////////////////////////////////////////////////////////////////////////////////

    private:
        /**
         * "Forbidden" copy constructor. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the copy constructor.
         *
         * @param obj Reference to an object of this class.
         */
        SplitTest(const SplitTest &/*obj*/);

        /**
         * "Forbidden" assignment operator. Goal: The compiler should warn
         * already at compile time for unwanted bugs caused by any misuse
         * of the assignment operator.
         *
         * @param obj Reference to an object of this class.
         * @return Reference to this instance.
         */
        SplitTest& operator=(const SplitTest &/*obj*/);

};

#endif /*RECINTEGRITYTESTSUITE_H_*/

