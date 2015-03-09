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

#ifndef CONTEXT_RUNTIMECONTROLCONTAINERTESTSUITE_H_
#define CONTEXT_RUNTIMECONTROLCONTAINERTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <stdint.h>
#include <iostream>
#include <string>

#include "core/base/KeyValueConfiguration.h"
#include "core/base/Deserializer.h"
#include "core/base/FIFOQueue.h"
#include "core/base/Hash.h"
#include "core/base/Serializable.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"
#include "core/base/Thread.h"
#include "core/data/Container.h"
#include "core/data/SerializableData.h"
#include "core/base/ConferenceClientModule.h"
#include "core/data/environment/Point3.h"
#include "core/data/environment/Position.h"
#include "context/base/DirectInterface.h"
#include "context/base/RuntimeControl.h"
#include "context/base/RuntimeEnvironment.h"
#include "context/base/SystemFeedbackComponent.h"

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace context::base;

class RuntimeControlContainerTestSampleData : public core::data::SerializableData {
    public:
        RuntimeControlContainerTestSampleData() :
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

class RuntimeControlContainerTestModule : public ConferenceClientModule {
    public:
        RuntimeControlContainerTestModule(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "RuntimeControlContainerTestModule"),
            m_cycleCounter(0),
            m_config(),
            m_receivedData() {}

        virtual void setUp() {}

        virtual void tearDown() {}

        virtual ModuleState::MODULE_EXITCODE body() {
            m_config = getKeyValueConfiguration();

            m_config.getValue<string>("runtimecontrolcontainertestmodule.key1");

            addDataStoreFor(Container::POSITION, m_receivedData);
            while (getModuleState() == ModuleState::RUNNING) {
                m_cycleCounter++;

                RuntimeControlContainerTestSampleData tcctsd;
                tcctsd.m_int = m_cycleCounter;
                Container c(Container::UNDEFINEDDATA, tcctsd);
                getConference().send(c);
            }

            return ModuleState::OKAY;
        }

        KeyValueConfiguration getConfiguration() {
            return m_config;
        }

        uint32_t getCycleCounter() const {
            return m_cycleCounter;
        }

        FIFOQueue& getReceivedData() {
            return m_receivedData;
        }

    private:
        uint32_t m_cycleCounter;
        KeyValueConfiguration m_config;
        FIFOQueue m_receivedData;
};

////////////////////////////////////////////////////////////////////////////////

class RuntimeControlContainerTestDummySystemPart : public SystemFeedbackComponent {
    public:
        RuntimeControlContainerTestDummySystemPart() :
            m_freq(1),
            m_receivedDataFIFO() {}

        float getFrequency() const {
            return m_freq;
        }

        virtual void setup() {}

        virtual void tearDown() {}

        virtual void step(const core::wrapper::Time &t, SendContainerToSystemsUnderTest &/*sender*/) {
            clog << "Call to RuntimeControlContainerTestDummySystemPart for t = " << t.getSeconds() << "." << t.getPartialMicroseconds() << ", containing " << getFIFO().getSize() << " containers." << endl;

            while (!getFIFO().isEmpty()) {
                Container c = getFIFO().leave();
                clog << "(RuntimeControlContainerTestDummySystemPart) Sent at " << c.getSentTimeStamp().toString() << ", Received at " << c.getReceivedTimeStamp().toString() << endl;
                m_receivedDataFIFO.add(c);

                // Wait for adding containers.
                Thread::usleep(100);
            }
        }

        const float m_freq;
        FIFOQueue m_receivedDataFIFO;
};

class RuntimeControlContainerTestDummySystemPartReply : public SystemFeedbackComponent {
    public:
        RuntimeControlContainerTestDummySystemPartReply(const float &freq) :
            m_freq(freq),
            m_receivedDataFIFO(),
            m_replyCounter(0) {}

        float getFrequency() const {
            return m_freq;
        }

        virtual void setup() {}

        virtual void tearDown() {}

        virtual void step(const core::wrapper::Time &t, SendContainerToSystemsUnderTest &sender) {
            clog << "Call to RuntimeControlContainerTestDummySystemPart for t = " << t.getSeconds() << "." << t.getPartialMicroseconds() << ", containing " << getFIFO().getSize() << " containers." << endl;

            while (!getFIFO().isEmpty()) {
                Container c = getFIFO().leave();
                clog << "(RuntimeControlContainerTestDummySystemPartReply) Sent at " << c.getSentTimeStamp().toString() << ", Received at " << c.getReceivedTimeStamp().toString() << endl;
                m_receivedDataFIFO.add(c);

                // Wait for adding containers.
                Thread::usleep(100);
            }

            core::data::environment::Position p;
            p.setPosition(core::data::environment::Point3(m_replyCounter, m_replyCounter+1, m_replyCounter+2));

            Container c(Container::POSITION, p);
            sender.sendToSystemsUnderTest(c);

            m_replyCounter++;
        }

        const float m_freq;
        FIFOQueue m_receivedDataFIFO;
        uint32_t m_replyCounter;
};

class RuntimeControlContainerTest : public CxxTest::TestSuite {
    public:
        void testRuntimeControlContainerRegularRunReceivingNotSending() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontrolcontainertestmodule.key1 = value1" << endl
                 << "runtimecontrolcontainertestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontrolcontainertestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlContainerTestModule rcctm(argc, argv);

            RuntimeControlContainerTestDummySystemPart rcctdsc;

            RuntimeEnvironment rte;
            rte.add(rcctm);
            rte.add(rcctdsc);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            // At least at the end, the FIFO must be empty.
            const uint32_t SIZE = rcctdsc.m_receivedDataFIFO.getSize();

            clog << "SIZE: " << SIZE << endl;
            TS_ASSERT(SIZE == 8);
            for(uint32_t i = 0; i < SIZE; i++) {
                Container c = rcctdsc.m_receivedDataFIFO.leave();
                RuntimeControlContainerTestSampleData data = c.getData<RuntimeControlContainerTestSampleData>();

                clog << "A-i: " << i << ", S: " << c.getSentTimeStamp().toMicroseconds() << ", R: " << c.getReceivedTimeStamp().toMicroseconds() << " data.m_int: " << data.m_int << endl;

                TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));
                TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));

                TS_ASSERT((i+1) == data.m_int);
            }

            sc.tearDown();

            TS_ASSERT(rcctm.getReceivedData().getSize() == 0);

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rcctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontrolcontainertestmodule.key1=value1" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 9 times (first cycle is head of app's while-loop).
            clog << "testRuntimeControlContainerRegularRunReceivingNotSending cycles : " << rcctm.getCycleCounter() << endl;
            TS_ASSERT(rcctm.getCycleCounter() == 9);
        }

        void testRuntimeControlContainerRegularRunReceivingSending() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontrolcontainertestmodule.key1 = value1" << endl
                 << "runtimecontrolcontainertestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontrolcontainertestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlContainerTestModule rcctm(argc, argv);

            RuntimeControlContainerTestDummySystemPartReply rcctdscr(1);

            RuntimeEnvironment rte;
            rte.add(rcctm);
            rte.add(rcctdscr);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            // At least at the end, the FIFO must be empty.
            const uint32_t SIZE = rcctdscr.m_receivedDataFIFO.getSize();
            clog << "SIZE: " << SIZE << endl;
            TS_ASSERT(SIZE == 17);
            uint32_t undefDataCnt = 0;
            for(uint32_t i = 0; i < SIZE; i++) {
                Container c = rcctdscr.m_receivedDataFIFO.leave();
                if (c.getDataType() == Container::UNDEFINEDDATA) {
                    undefDataCnt++;
                    RuntimeControlContainerTestSampleData data = c.getData<RuntimeControlContainerTestSampleData>();

                    clog << "2-i: " << i << ", S: " << c.getSentTimeStamp().toMicroseconds() << ", R: " << c.getReceivedTimeStamp().toMicroseconds() << endl;
                    TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == (undefDataCnt * 1000 * 1000));
                    TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == (undefDataCnt * 1000 * 1000));
                    TS_ASSERT(undefDataCnt == data.m_int);
                }
            }

            sc.tearDown();

            const uint32_t SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST = rcctm.getReceivedData().getSize();
            TS_ASSERT(SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST == 9);
            for(uint32_t i = 0; i < SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST; i++) {
                Container c = rcctm.getReceivedData().leave();
                core::data::environment::Position pos = c.getData<core::data::environment::Position>();
                core::data::environment::Point3 p = pos.getPosition();
                core::data::environment::Point3 ref(i+1, i+2, i+3);

                clog << "B1-i: " << i << ", S: " << c.getSentTimeStamp().toMicroseconds() << ", R: " << c.getReceivedTimeStamp().toMicroseconds() << endl;

                TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));
                TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));
                TS_ASSERT((p-ref).length() < 1e-5);
            }

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rcctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontrolcontainertestmodule.key1=value1" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 9 times (first cycle is head of app's while-loop).
            clog << "testRuntimeControlContainerRegularRunReceivingSending cycles : " << rcctm.getCycleCounter() << endl;
            TS_ASSERT(rcctm.getCycleCounter() == 9);
        }

        void testRuntimeControlContainerRegularRunReceivingSendingAppTwiceAsFastAsSC() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontrolcontainertestmodule.key1 = value1" << endl
                 << "runtimecontrolcontainertestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontrolcontainertestmodule");
            string argv1("--cid=100");
            string argv2("--freq=2");
            int32_t argc = 3;
            char **argv;
            argv = new char*[3];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());

            RuntimeControlContainerTestModule rcctm(argc, argv);

            RuntimeControlContainerTestDummySystemPartReply rcctdscr(1);

            RuntimeEnvironment rte;
            rte.add(rcctm);
            rte.add(rcctdscr);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            // At least at the end, the FIFO must be empty.
            const uint32_t SIZE = rcctdscr.m_receivedDataFIFO.getSize();
            clog << "SIZE: " << SIZE << endl;
            TS_ASSERT(SIZE == 26);
            uint32_t undefDataCnt = 0;
            for(uint32_t i = 0; i < SIZE; i++) {
                Container c = rcctdscr.m_receivedDataFIFO.leave();
                if (c.getDataType() == Container::UNDEFINEDDATA) {
                    undefDataCnt++;
                    RuntimeControlContainerTestSampleData data = c.getData<RuntimeControlContainerTestSampleData>();

                    clog << "1-i: " << i << ", S: " << c.getSentTimeStamp().toMicroseconds() << ", R: " << c.getReceivedTimeStamp().toMicroseconds() << endl;
                    TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == (undefDataCnt * 500 * 1000));
                    TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == (undefDataCnt * 500 * 1000));
                    TS_ASSERT(undefDataCnt == data.m_int);
                }
            }

            sc.tearDown();

            const uint32_t SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST = rcctm.getReceivedData().getSize();
            TS_ASSERT(SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST == 9);
            for(uint32_t i = 0; i < SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST; i++) {
                Container c = rcctm.getReceivedData().leave();
                core::data::environment::Position pos = c.getData<core::data::environment::Position>();
                core::data::environment::Point3 p = pos.getPosition();
                core::data::environment::Point3 ref(i+1, i+2, i+3);

                TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));
                TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == ((i+1) * 1000 * 1000));
                TS_ASSERT((p-ref).length() < 1e-5);
            }

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rcctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontrolcontainertestmodule.key1=value1" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 19 times (see above).
            clog << "testRuntimeControlContainerRegularRunReceivingSendingAppTwiceAsFastAsSC cycles : " << rcctm.getCycleCounter() << endl;
            TS_ASSERT(rcctm.getCycleCounter() == 19);
        }

        void testRuntimeControlContainerRegularRunReceivingSendingAppHalfAsFastAsSC() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontrolcontainertestmodule.key1 = value1" << endl
                 << "runtimecontrolcontainertestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontrolcontainertestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlContainerTestModule rcctm(argc, argv);

            RuntimeControlContainerTestDummySystemPartReply rcctdscr(2);

            RuntimeEnvironment rte;
            rte.add(rcctm);
            rte.add(rcctdscr);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            // At least at the end, the FIFO must be empty.
            const uint32_t SIZE = rcctdscr.m_receivedDataFIFO.getSize();
            clog << "SIZE: " << SIZE << endl;
            TS_ASSERT(SIZE == 28);
            uint32_t undefDataCnt = 0;
            for(uint32_t i = 0; i < SIZE; i++) {
                Container c = rcctdscr.m_receivedDataFIFO.leave();
                if (c.getDataType() == Container::UNDEFINEDDATA) {
                    undefDataCnt++;
                    RuntimeControlContainerTestSampleData data = c.getData<RuntimeControlContainerTestSampleData>();

                    clog << "2-i: " << i << ", S: " << c.getSentTimeStamp().toMicroseconds() << ", R: " << c.getReceivedTimeStamp().toMicroseconds() << endl;
                    TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == (undefDataCnt * 1000 * 1000));
                    TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == (undefDataCnt * 1000 * 1000));
                    TS_ASSERT(undefDataCnt == data.m_int);
                }
            }

            sc.tearDown();

            const uint32_t SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST = rcctm.getReceivedData().getSize();
            TS_ASSERT(SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST == 19);
            for(uint32_t i = 0; i < SIZE_RECEIVED_AT_SYSTEM_UNDER_TEST; i++) {
                Container c = rcctm.getReceivedData().leave();
                core::data::environment::Position pos = c.getData<core::data::environment::Position>();
                core::data::environment::Point3 p = pos.getPosition();
                core::data::environment::Point3 ref(i+1, i+2, i+3);

                // The app receives the data with a shift of +1.
                TS_ASSERT(static_cast<uint32_t>(c.getSentTimeStamp().toMicroseconds()) == ((i+1) * 500 * 1000));
                TS_ASSERT(static_cast<uint32_t>(c.getReceivedTimeStamp().toMicroseconds()) == ((i+1) * 500 * 1000));
                TS_ASSERT((p-ref).length() < 1e-5);
            }

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rcctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontrolcontainertestmodule.key1=value1" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 9 times (first cycle is head of app's while-loop).
            clog << "testRuntimeControlContainerRegularRunReceivingSendingAppHalfAsFastAsSC cycles : " << rcctm.getCycleCounter() << endl;
            TS_ASSERT(rcctm.getCycleCounter() == 9);
        }
};

#endif /*CONTEXT_RUNTIMECONTROLCONTAINERTESTSUITE_H_*/
