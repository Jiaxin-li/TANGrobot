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

#ifndef MANAGEDLEVELTESTSUITE_H_
#define MANAGEDLEVELTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#ifndef WIN32
    #include <sys/wait.h>
#endif

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>

#include "core/base/ConferenceClientModule.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/base/Lock.h"
#include "core/base/ModuleState.h"
#include "core/base/Mutex.h"
#include "core/base/Service.h"
#include "core/base/Thread.h"
#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"


#include "core/data/TimeStamp.h"

#include "../include/SuperComponent.h"

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::exceptions;

class TestSuiteExample7Data : public core::data::SerializableData {
	public:
		TestSuiteExample7Data() : m_numericalValue(0) {}

		virtual ~TestSuiteExample7Data() {}

		TestSuiteExample7Data(const TestSuiteExample7Data &obj) :
			SerializableData(),
			m_numericalValue(obj.m_numericalValue) {}

		TestSuiteExample7Data& operator=(const TestSuiteExample7Data &obj) {
		    m_numericalValue = obj.m_numericalValue;
		    return (*this);
	    }

		uint32_t getNumericalValue() const {
            return m_numericalValue;
        }

		void setNumericalValue(const uint32_t &nv) {
            m_numericalValue = nv;
        }

		virtual ostream& operator<<(ostream &out) const {
		    SerializationFactory sf;
		    Serializer &s = sf.getSerializer(out);
		    s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('n', 'u', 'm') >::RESULT,
				    m_numericalValue);
		    return out;
	    }

		virtual istream& operator>>(istream &in) {
		    SerializationFactory sf;
		    Deserializer &d = sf.getDeserializer(in);
		    d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('n', 'u', 'm') >::RESULT,
			       m_numericalValue);
		    return in;
	    }

		virtual const string toString() const {
		    stringstream s;
		    s << m_numericalValue;
		    return s.str();
	    }

	private:
		uint32_t m_numericalValue;
};

class Example7SenderApp : public ConferenceClientModule {
    public:
        bool tearDownCalled;
        bool m_withRandomSleep;

    public:
        Example7SenderApp(const int32_t &argc, char **argv, bool withRandomSleep) :
            ConferenceClientModule(argc, argv, "example7sender"),
            tearDownCalled(false),
            m_withRandomSleep(withRandomSleep) {}

        void setUp() {}

        void tearDown() {
            tearDownCalled = true;
        }

        core::base::ModuleState::MODULE_EXITCODE body() {
            uint32_t seed = 24;
        	uint32_t counter = 0;

            srand(seed);

        	while (getModuleState() == ModuleState::RUNNING) {
        		// Create user data.
        		TestSuiteExample7Data data;
        		data.setNumericalValue(counter++);

        		// Create container with user data type ID 5.
        		Container c(Container::USER_DATA_5, data);

        		// Send container.
        		getConference().send(c);

        		// Restrict counter.
        		if (counter > 10000) counter = 0;

                if (m_withRandomSleep) {
                    // Provoke unexpected delays to demonstrate the need for --pulse in supercomponent.
                    double r = ((rand() / (double)RAND_MAX)) * 1000 * 1000 * 1.1;
                    cerr << "Sleep for " << r << " ms." << endl;
                    Thread::usleep((long)r);
                }
            }

        	return ModuleState::OKAY;
        }
};

class Example7ReceiverApp : public ConferenceClientModule {
    public:
        bool tearDownCalled;
        mutable Mutex m_diffMutex;
        uint32_t m_diff;

    public:
        Example7ReceiverApp(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "example7receiver"),
            tearDownCalled(false),
            m_diffMutex(),
            m_diff(0) {}

        void setUp() {}

        void tearDown() {
            tearDownCalled = true;
        }

        core::base::ModuleState::MODULE_EXITCODE body() {
            uint32_t sum = 0;
            uint32_t expected_sum = 0;
            uint32_t counter = 0;

        	while (getModuleState() == ModuleState::RUNNING) {
			    Container c = getKeyValueDataStore().get(Container::USER_DATA_5);
			    TestSuiteExample7Data data = c.getData<TestSuiteExample7Data>();
                sum += data.getNumericalValue();
			    cout << "Latest container from data type " << (uint32_t)c.getDataType() << ", content: " << data.toString() << ", sum = " << sum << endl;

                if (sum > 0) {
                    counter++;
                    expected_sum += counter;
                    {
                        Lock l(m_diffMutex);
                        m_diff = (expected_sum - sum);
                        cout << "Diff: " << m_diff << endl;
                    }
                }
        	}

        	return ModuleState::OKAY;
        }

        uint32_t getDiff() {
            Lock l(m_diffMutex);
            return m_diff;
        }
};


class ConnectedClientModuleApp : public ConferenceClientModule {
    public:
        ConnectedClientModuleApp(const int32_t &argc, char **argv, const string &name) :
            ConferenceClientModule(argc, argv, name),
            tearDownCalled(false) {}

        const KeyValueConfiguration getKVC() const {
            return getKeyValueConfiguration();
        }

        void setUp() {}

        void tearDown() {
            tearDownCalled = true;
        }

        core::base::ModuleState::MODULE_EXITCODE body() {
            while (getModuleState() == core::base::ModuleState::RUNNING) {
                // Do nothing.
            }

            return core::base::ModuleState::OKAY;
        }

        bool tearDownCalled;
};

class ConnectedClientModuleTestService : public Service {
    public:
        ConnectedClientModuleTestService(ConferenceClientModule *app) :
            myApp(app),
            m_isRunModuleCompletedMutex(),
            m_isRunModuleCompleted(false) {}

        ConnectedClientModuleTestService(const ConnectedClientModuleTestService &/*obj*/);
        ConnectedClientModuleTestService& operator=(const ConnectedClientModuleTestService &/*obj*/);

        virtual void beforeStop() {
            // Stop app.
            myApp->setModuleState(ModuleState::NOT_RUNNING);
        }

        virtual void run() {
            serviceReady();
            myApp->runModule();

            Lock l(m_isRunModuleCompletedMutex);
            m_isRunModuleCompleted = true;
        }

        bool isRunModuleCompleted() const {
            bool retVal = false;
            {
                Lock l(m_isRunModuleCompletedMutex);
                retVal = m_isRunModuleCompleted;
            }
            return retVal;
        }

        const TimeStamp getStartOfCurrentCycle() const {
            return myApp->getStartOfCurrentCycle();
        }

    private:
        ConferenceClientModule *myApp;
        mutable Mutex m_isRunModuleCompletedMutex;
        bool m_isRunModuleCompleted;
};

class SupercomponentService : public Service {
    public:
        SupercomponentService(const int32_t &argc, char **argv) :
            mySC(argc, argv) {}

        virtual void beforeStop() {
            // Stop app.
            mySC.setModuleState(ModuleState::NOT_RUNNING);
        }

        virtual void run() {
            serviceReady();
            mySC.runModule();
        }

        const TimeStamp getStartOfCurrentCycle() const {
            return mySC.getStartOfCurrentCycle();
        }

    private:
        supercomponent::SuperComponent mySC;
};

class ManagedLevelTest : public CxxTest::TestSuite {
    public:

        void testManagedLevelNone() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=109");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=none");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();


            // Create a component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=109");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());
            
            ConnectedClientModuleApp app(c0_argc, c0_argv, "ConnectedClientModuleTestService");
            ConnectedClientModuleTestService ccms(&app);
            ccms.start();

            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 2;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            ccms.stop();
            scs.stop();

            TS_ASSERT(ccms.isRunModuleCompleted());
            TS_ASSERT(app.tearDownCalled);

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void testManagedLevelNone_stop_supercomponent_yields_stop_client() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=108");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=none");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            // Create a component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=108");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

            ConnectedClientModuleApp app(c0_argc, c0_argv, "ConnectedClientModuleTestService");
            ConnectedClientModuleTestService ccms(&app);

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();

            // Start component.
            ccms.start();

            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 2;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Stop supercomponent needs to also stop the client.
            TS_ASSERT(!ccms.isRunModuleCompleted());
            TS_ASSERT(!app.tearDownCalled);

            scs.stop();
            cycles = 0;
            while (cycles < MAX_CYCLES * 5) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Wait for tearDownCalled.
            TS_ASSERT(app.tearDownCalled);

            ccms.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulse() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=107");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();


            // Create a component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=107");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

            ConnectedClientModuleApp app(c0_argc, c0_argv, "ConnectedClientModuleTestService");
            ConnectedClientModuleTestService ccms(&app);
            ccms.start();

            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 15;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            const float delta = fabs(((float)(scs.getStartOfCurrentCycle().getFractionalMicroseconds() - ccms.getStartOfCurrentCycle().getFractionalMicroseconds()))/(1000.0*1000.0));
            cerr << "Alignment delta: " << delta << endl;

            // This fails on FreeBSD and NetBSD and OpenBSD due to non-rt behavior.
//            TS_ASSERT_DELTA(delta, 0.001, 5e-3);
            TS_WARN(delta);

            ccms.stop();
            scs.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulse_stop_supercomponent_yields_stop_client() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=106");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            // Create a component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=106");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

            ConnectedClientModuleApp app(c0_argc, c0_argv, "ConnectedClientModuleTestService");
            ConnectedClientModuleTestService ccms(&app);

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();

            // Start component.
            ccms.start();

            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 15;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Stop supercomponent needs to also stop the client.
            TS_ASSERT(!ccms.isRunModuleCompleted());
            TS_ASSERT(!app.tearDownCalled);

            scs.stop();
            cycles = 0;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Wait for tearDownCalled.
            TS_ASSERT(app.tearDownCalled);

            ccms.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulseShift_TwoModules() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=105");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse_shift");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();


            // Create first component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=105");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

            ConnectedClientModuleApp app1(c0_argc, c0_argv, "ConnectedClientModuleTestService1");
            ConnectedClientModuleTestService ccms1(&app1);

            // Create second component.
            string c1_argv0("ClientModuleTestService2");
            string c1_argv1("--cid=105");
            int32_t c1_argc = 2;
            char **c1_argv;
            c1_argv = new char*[c1_argc];
            c1_argv[0] = const_cast<char*>(c1_argv0.c_str());
            c1_argv[1] = const_cast<char*>(c1_argv1.c_str());

            ConnectedClientModuleApp app2(c1_argc, c1_argv, "ConnectedClientModuleTestService2");
            ConnectedClientModuleTestService ccms2(&app2);

            ccms1.start();
            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 3;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            ccms2.start();
            cycles = 0;
            while (cycles < MAX_CYCLES*5) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            const float delta1 = fabs(((float)(scs.getStartOfCurrentCycle().getFractionalMicroseconds() - ccms1.getStartOfCurrentCycle().getFractionalMicroseconds()))/(1000.0*1000.0));
            cerr << "Alignment delta1: " << delta1 << endl;

            const float delta2 = fabs(((float)(scs.getStartOfCurrentCycle().getFractionalMicroseconds() - ccms2.getStartOfCurrentCycle().getFractionalMicroseconds()))/(1000.0*1000.0));
            cerr << "Alignment delta2: " << delta2 << endl;

            // The scheduling of the two modules should be different by at least 10ms.
            const float delta = fabs(delta1-delta2);
            TS_WARN(delta);
            // TS_ASSERT(delta > 0.01); // This does not always hold.

            ccms1.stop();
            ccms2.stop();
            scs.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulseShift_stop_supercomponent_yields_stop_client() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=104");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse_shift");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            // Create a component.
            string c0_argv0("ClientModuleTestService");
            string c0_argv1("--cid=104");
            int32_t c0_argc = 2;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

            ConnectedClientModuleApp app1(c0_argc, c0_argv, "ConnectedClientModuleTestService1");
            ConnectedClientModuleApp app2(c0_argc, c0_argv, "ConnectedClientModuleTestService2");

            ConnectedClientModuleTestService ccms1(&app1);
            ConnectedClientModuleTestService ccms2(&app2);

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();

            // Start component.
            ccms1.start();
            ccms2.start();

            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 15;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Stop supercomponent needs to also stop the clients.
            TS_ASSERT(!ccms1.isRunModuleCompleted());
            TS_ASSERT(!app1.tearDownCalled);
            TS_ASSERT(!ccms2.isRunModuleCompleted());
            TS_ASSERT(!app2.tearDownCalled);

            scs.stop();
            cycles = 0;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // Wait for tearDownCalled.
            TS_ASSERT(app1.tearDownCalled);
            TS_ASSERT(app2.tearDownCalled);

            ccms1.stop();
            ccms2.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulse_SenderReceiver() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=103");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();


            // Create first component.
            string c0_argv0("example7sender");
            string c0_argv1("--cid=103");
            string c0_argv2("--freq=10");
            int32_t c0_argc = 3;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());
            c0_argv[2] = const_cast<char*>(c0_argv2.c_str());

            Example7ReceiverApp app1(c0_argc, c0_argv);
            ConnectedClientModuleTestService ccms1(&app1);

            // Create second component.
            string c1_argv0("example7receiver");
            string c1_argv1("--cid=103");
            string c1_argv2("--freq=10");
            int32_t c1_argc = 3;
            char **c1_argv;
            c1_argv = new char*[c1_argc];
            c1_argv[0] = const_cast<char*>(c1_argv0.c_str());
            c1_argv[1] = const_cast<char*>(c1_argv1.c_str());
            c1_argv[2] = const_cast<char*>(c1_argv2.c_str());

            const bool USE_RANDOM_WAIT = false;
            Example7SenderApp app2(c1_argc, c1_argv, USE_RANDOM_WAIT);
            ConnectedClientModuleTestService ccms2(&app2);

            ccms1.start();
            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 3;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            ccms2.start();
            cycles = 0;
            const uint32_t TIMEOUT = 300;
            while ((app1.getDiff() == 0) && (cycles < TIMEOUT)) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // As both modules are aligned with each other regarding the time, there is a high probability that the sender is sending its data AFTER the receiver is executed and thus, it is using old data.

            // As this test has also the probability to pass without showing this behavior, we don't require an assert for MANAGED_LEVE = PULSE.
//            TS_ASSERT(app1.getDiff() != 0);
//            TS_ASSERT(cycles < TIMEOUT);

            ccms1.stop();
            ccms2.stop();

            TS_ASSERT(ccms1.isRunModuleCompleted());
            TS_ASSERT(app1.tearDownCalled);
            TS_ASSERT(ccms2.isRunModuleCompleted());
            TS_ASSERT(app2.tearDownCalled);

            scs.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void notestManagedLevelPulseShift_SenderReceiver() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 30000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=102");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse_shift");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();


            // Create first component.
            string c0_argv0("example7sender");
            string c0_argv1("--cid=102");
            string c0_argv2("--freq=10");
            int32_t c0_argc = 3;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());
            c0_argv[2] = const_cast<char*>(c0_argv2.c_str());

            Example7ReceiverApp app1(c0_argc, c0_argv);
            ConnectedClientModuleTestService ccms1(&app1);

            // Create second component.
            string c1_argv0("example7receiver");
            string c1_argv1("--cid=102");
            string c1_argv2("--freq=10");
            int32_t c1_argc = 3;
            char **c1_argv;
            c1_argv = new char*[c1_argc];
            c1_argv[0] = const_cast<char*>(c1_argv0.c_str());
            c1_argv[1] = const_cast<char*>(c1_argv1.c_str());
            c1_argv[2] = const_cast<char*>(c1_argv2.c_str());

            const bool USE_RANDOM_WAIT = false;
            Example7SenderApp app2(c1_argc, c1_argv, USE_RANDOM_WAIT);
            ConnectedClientModuleTestService ccms2(&app2);

            ccms1.start();
            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 10;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            ccms2.start();
            cycles = 0;
            const uint32_t TIMEOUT = 60;
            while ((app1.getDiff() == 0) && (cycles < TIMEOUT)) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // This test adds a delta of 30ms to the execution trigger and thus, it should not fail as both components have a determined execution order. However, as we cannot reliably predict the successful shift, we simply warn about the results.
            TS_WARN(app1.getDiff());
            TS_WARN(cycles);

            ccms1.stop();
            ccms2.stop();

            TS_ASSERT(ccms1.isRunModuleCompleted());
            TS_ASSERT(app1.tearDownCalled);
            TS_ASSERT(ccms2.isRunModuleCompleted());
            TS_ASSERT(app2.tearDownCalled);

            scs.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void testManagedLevelPulseShift_SenderReceiver_random_sleep_to_require_pulse_time_ack() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)
            // Create a configuration.
            stringstream configuration;
            configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                 << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                 << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                 << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                 << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                 << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

            fstream fout("configuration", ios::out);
            TS_ASSERT(fout.good());
            fout << configuration.str();
            fout.close();

            // Start the encapsulated supercomponent.
            string sc_argv0("supercomponent");
            string sc_argv1("--cid=102");
            string sc_argv2("--freq=10");
            string sc_argv3("--managed=pulse_shift");
            int32_t sc_argc = 4;
            char **sc_argv;
            sc_argv = new char*[sc_argc];
            sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
            sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
            sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
            sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

            SupercomponentService scs(sc_argc, sc_argv);
            scs.start();

            // Create first component.
            string c0_argv0("example7sender");
            string c0_argv1("--cid=102");
            string c0_argv2("--freq=10");
            int32_t c0_argc = 3;
            char **c0_argv;
            c0_argv = new char*[c0_argc];
            c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
            c0_argv[1] = const_cast<char*>(c0_argv1.c_str());
            c0_argv[2] = const_cast<char*>(c0_argv2.c_str());

            Example7ReceiverApp app1(c0_argc, c0_argv);
            ConnectedClientModuleTestService ccms1(&app1);

            // Create second component.
            string c1_argv0("example7receiver");
            string c1_argv1("--cid=102");
            string c1_argv2("--freq=10");
            int32_t c1_argc = 3;
            char **c1_argv;
            c1_argv = new char*[c1_argc];
            c1_argv[0] = const_cast<char*>(c1_argv0.c_str());
            c1_argv[1] = const_cast<char*>(c1_argv1.c_str());
            c1_argv[2] = const_cast<char*>(c1_argv2.c_str());

            const bool USE_RANDOM_WAIT = true;
            Example7SenderApp app2(c1_argc, c1_argv, USE_RANDOM_WAIT);
            ConnectedClientModuleTestService ccms2(&app2);

            ccms1.start();
            uint32_t cycles = 0;
            const uint32_t MAX_CYCLES = 3;
            const uint32_t ONE_SECOND = 1*1000*1000;
            while (cycles < MAX_CYCLES) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            ccms2.start();
            cycles = 0;
            const uint32_t TIMEOUT = 180;
            while ((app1.getDiff() == 0) && (cycles < TIMEOUT)) {
                Thread::usleep(ONE_SECOND);
                cycles++;
            }

            // This test adds a delta to the execution trigger and thus, it should not fail as both components have a determined execution order.
            TS_ASSERT(app1.getDiff() != 0);
            TS_ASSERT(cycles < TIMEOUT);

            ccms1.stop();
            ccms2.stop();

            TS_ASSERT(ccms1.isRunModuleCompleted());
            TS_ASSERT(app1.tearDownCalled);
            TS_ASSERT(ccms2.isRunModuleCompleted());
            TS_ASSERT(app2.tearDownCalled);

            scs.stop();

            UNLINK("configuration");
#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void testManagedLevelPulseTimeAck_SenderReceiver() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)

// The following test case design pattern (singleton of system service & fork/waitpid) is only available on Linux, OpenBSD, and FreeBSD.
#if defined(__linux__) || (__OpenBSD__) || (__FreeBSD__)
            const uint32_t ONE_SECOND = 1*1000*1000;

            // As the dependent modules replace the time factory, we need a separate process context for them by using the syscall fork.

            pid_t child;
            child = fork();

            if (child == 0) {
                // In child process.

                // Create first component.
                string c0_argv0("example7sender");
                string c0_argv1("--cid=101");
                string c0_argv2("--freq=10");
                int32_t c0_argc = 3;
                char **c0_argv;
                c0_argv = new char*[c0_argc];
                c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
                c0_argv[1] = const_cast<char*>(c0_argv1.c_str());
                c0_argv[2] = const_cast<char*>(c0_argv2.c_str());

                Example7ReceiverApp app1(c0_argc, c0_argv);
                ConnectedClientModuleTestService ccms1(&app1);

                // Create second component.
                string c1_argv0("example7receiver");
                string c1_argv1("--cid=101");
                string c1_argv2("--freq=10");
                int32_t c1_argc = 3;
                char **c1_argv;
                c1_argv = new char*[c1_argc];
                c1_argv[0] = const_cast<char*>(c1_argv0.c_str());
                c1_argv[1] = const_cast<char*>(c1_argv1.c_str());
                c1_argv[2] = const_cast<char*>(c1_argv2.c_str());

                const bool USE_RANDOM_WAIT = true;
                Example7SenderApp app2(c1_argc, c1_argv, USE_RANDOM_WAIT);
                ConnectedClientModuleTestService ccms2(&app2);

                ccms1.start();
                uint32_t cycles = 0;
                const uint32_t MAX_CYCLES = 3;
                while (cycles < MAX_CYCLES) {
                    Thread::usleep(ONE_SECOND);
                    cycles++;
                }

                ccms2.start();
                cycles = 0;
                const uint32_t TIMEOUT = 180;
                while ((app1.getDiff() == 0) && (cycles < TIMEOUT)) {
                    Thread::usleep(ONE_SECOND);
                    cycles++;
                }

                // This test adds a delta to the execution trigger and thus, it should not fail as both components have a determined execution order.
                TS_ASSERT(app1.getDiff() == 0);
                TS_ASSERT(cycles == TIMEOUT);

                TS_ASSERT(app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 0 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 100000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 200000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 300000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 400000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 500000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 600000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 700000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 800000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 900000 );

                TS_ASSERT(app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 0 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 100000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 200000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 300000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 400000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 500000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 600000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 700000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 800000 ||
                          app2.getStartOfCurrentCycle().getFractionalMicroseconds() == 900000 );

                ccms1.stop();
                ccms2.stop();

                TS_ASSERT(ccms1.isRunModuleCompleted());
                TS_ASSERT(app1.tearDownCalled);
                TS_ASSERT(ccms2.isRunModuleCompleted());
                TS_ASSERT(app2.tearDownCalled);

                uint32_t retVal = (!ccms1.isRunModuleCompleted() ? 1<<1 : 0) |
                                  (!app1.tearDownCalled ? 1<<2 : 0) |
                                  (!ccms2.isRunModuleCompleted() ? 1<<3 : 0) |
                                  (!app2.tearDownCalled ? 1<<4 : 0);

                // Leave child's process context.
                exit(retVal);
            }
            else if (child > 0) {
                // In parent process.

                // Create a configuration.
                stringstream configuration;
                configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                     << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                     << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                     << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                     << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                     << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

                fstream fout("configuration", ios::out);
                TS_ASSERT(fout.good());
                fout << configuration.str();
                fout.close();

                // Start the encapsulated supercomponent.
                string sc_argv0("supercomponent");
                string sc_argv1("--cid=101");
                string sc_argv2("--freq=10");
                string sc_argv3("--managed=pulse_time_ack");
                int32_t sc_argc = 4;
                char **sc_argv;
                sc_argv = new char*[sc_argc];
                sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
                sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
                sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
                sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

                SupercomponentService scs(sc_argc, sc_argv);
                scs.start();

                // Monitor the child's process and wait for its exit.
                int status = 0;
                do {
                    pid_t w = waitpid(child, &status, WUNTRACED | WCONTINUED);
                    TS_ASSERT(w > -1);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                TS_ASSERT(WIFEXITED(status));

                if (WIFEXITED(status)) {
                    TS_ASSERT(WEXITSTATUS(status) == 0);
                }

                scs.stop();

                UNLINK("configuration");
            }
#endif /*defined(__linux__) || (__OpenBSD__) || (__FreeBSD__)*/

#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }

        void testManagedLevelPulseTime() {
// This test suite does not run properly on Win32 and MacOS. Thus, skip it for the time being...
#if !defined(WIN32) && !defined(__APPLE__)

// The following test case design pattern (singleton of system service & fork/waitpid) is only available on Linux, OpenBSD, and FreeBSD.
#if defined(__linux__) || (__OpenBSD__) || (__FreeBSD__)
            const uint32_t ONE_SECOND = 1*1000*1000;

            // As the dependent modules replace the time factory, we need a separate process context for them by using the syscall fork.

            pid_t child;
            child = fork();

            if (child == 0) {
                // In child process.

                // Create a component.
                string c0_argv0("ClientModuleTestService");
                string c0_argv1("--cid=99");
                int32_t c0_argc = 2;
                char **c0_argv;
                c0_argv = new char*[c0_argc];
                c0_argv[0] = const_cast<char*>(c0_argv0.c_str());
                c0_argv[1] = const_cast<char*>(c0_argv1.c_str());

                ConnectedClientModuleApp app1(c0_argc, c0_argv, "ConnectedClientModuleTestService1");
                ConnectedClientModuleTestService ccms1(&app1);

                ccms1.start();
                uint32_t cycles = 0;
                const uint32_t MAX_CYCLES = 5;
                while (cycles < MAX_CYCLES) {
                    Thread::usleep(ONE_SECOND);
                    cycles++;
                }
                TS_ASSERT(app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 0 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 100000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 200000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 300000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 400000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 500000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 600000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 700000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 800000 ||
                          app1.getStartOfCurrentCycle().getFractionalMicroseconds() == 900000 );

                ccms1.stop();

                TS_ASSERT(ccms1.isRunModuleCompleted());
                TS_ASSERT(app1.tearDownCalled);

                uint32_t retVal = (!ccms1.isRunModuleCompleted() ? 1<<1 : 0) |
                                  (!app1.tearDownCalled ? 1<<2 : 0);

                // Leave child's process context.
                exit(retVal);
            }
            else if (child > 0) {
                // In parent process.

                // Create a configuration.
                stringstream configuration;
                configuration << "global.buffer.memorySegmentSize = 2800000 # Size of a memory segment for a shared data stream in bytes." << endl
                     << "global.buffer.numberOfMemorySegments = 20 # Number of memory segments used for buffering." << endl
                     << "supercomponent.pulseshift.shift = 10000 # (in microseconds) If the managed level is pulse_shift, all connected modules will be informed about the supercomponent's real time by this increment per module. Thus, the execution times per modules are better aligned with supercomponent and the data exchange is somewhat more predictable." << endl
                     << "supercomponent.pulsetimeack.timeout = 5000 # (in milliseconds) If the managed level is pulse_time_ack, this is the timeout for waiting for an ACK message from the dependent client." << endl
                     << "supercomponent.pulsetimeack.yield = 5000 # (in microseconds) If the managed level is pulse_time_ack, the modules are triggered sequentially by sending pulses and waiting for acknowledgment messages. To allow the modules to deliver their respective containers, this yielding time is used to sleep before supercomponent sends the pulse messages the next module in this execution cycle. This value needs to be adjusted for networked simulations to ensure deterministic execution. " << endl
                     << "supercomponent.pulsetimeack.exclude = cockpit,monitor # List of modules that will not get a pulse message from supercomponent." << endl;

                fstream fout("configuration", ios::out);
                TS_ASSERT(fout.good());
                fout << configuration.str();
                fout.close();

                // Start the encapsulated supercomponent.
                string sc_argv0("supercomponent");
                string sc_argv1("--cid=99");
                string sc_argv2("--freq=10");
                string sc_argv3("--managed=pulse_time");
                int32_t sc_argc = 4;
                char **sc_argv;
                sc_argv = new char*[sc_argc];
                sc_argv[0] = const_cast<char*>(sc_argv0.c_str());
                sc_argv[1] = const_cast<char*>(sc_argv1.c_str());
                sc_argv[2] = const_cast<char*>(sc_argv2.c_str());
                sc_argv[3] = const_cast<char*>(sc_argv3.c_str());

                SupercomponentService scs(sc_argc, sc_argv);
                scs.start();

                // Monitor the child's process and wait for its exit.
                int status = 0;
                do {
                    pid_t w = waitpid(child, &status, WUNTRACED | WCONTINUED);
                    TS_ASSERT(w > -1);
                } while (!WIFEXITED(status) && !WIFSIGNALED(status));
                TS_ASSERT(WIFEXITED(status));
                if (WIFEXITED(status)) {
                    TS_ASSERT(WEXITSTATUS(status) == 0);
                }

                scs.stop();

                UNLINK("configuration");
            }
#endif /*defined(__linux__) || (__OpenBSD__) || (__FreeBSD__)*/

#endif /*!defined(WIN32) && !defined(__APPLE__)*/
        }
};

#endif /*MANAGEDLEVELTESTSUITE_H_*/
