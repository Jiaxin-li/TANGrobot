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

#ifndef CONTEXT_RUNTIMECONTROLTESTSUITE_H_
#define CONTEXT_RUNTIMECONTROLTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <stdint.h>
#include <iostream>
#include <string>

#include "core/base/KeyValueConfiguration.h"
#include "core/base/ConferenceClientModule.h"
#include "context/base/DirectInterface.h"
#include "context/base/RuntimeControl.h"
#include "context/base/RuntimeEnvironment.h"
#include "context/base/SystemFeedbackComponent.h"

using namespace std;
using namespace core::base;
using namespace context::base;

class RuntimeControlTestModule : public ConferenceClientModule {
    public:
        RuntimeControlTestModule(const int32_t &argc, char **argv) :
            ConferenceClientModule(argc, argv, "RuntimeControlTestModule"),
            m_cycleCounter(0),
            m_config() {}

        virtual void setUp() {}

        virtual void tearDown() {}

        virtual ModuleState::MODULE_EXITCODE body() {
            m_config = getKeyValueConfiguration();

            m_config.getValue<string>("runtimecontroltestmodule.key1");

            while (getModuleState() == ModuleState::RUNNING) {
                m_cycleCounter++;
            }

            return ModuleState::OKAY;
        }

        KeyValueConfiguration getConfiguration() {
            return m_config;
        }

        uint32_t getCycleCounter() const {
            return m_cycleCounter;
        }

    private:
        uint32_t m_cycleCounter;
        KeyValueConfiguration m_config;
};

class RuntimeControlTestDummySystemPart : public SystemFeedbackComponent {
    public:
        RuntimeControlTestDummySystemPart() :
            m_freq(1),
            m_setupCalled(false),
            m_tearDownCalled(false) {}

        float getFrequency() const {
            return m_freq;
        }

        virtual void setup() {
            m_setupCalled = true;
        }

        virtual void tearDown() {
            m_tearDownCalled = true;
        }

        virtual void step(const core::wrapper::Time &t, SendContainerToSystemsUnderTest &/*sender*/) {
            clog << "Call to RuntimeControlTestDummySystemPart for t = " << t.getSeconds() << "." << t.getPartialMicroseconds() << ", containing " << getFIFO().getSize() << " containers." << endl;
        }

        const float m_freq;
        bool m_setupCalled;
        bool m_tearDownCalled;
};

class RuntimeControlTest : public CxxTest::TestSuite {
    public:
        void testRuntimeControlRegularRun() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontroltestmodule.key1 = value1" << endl
                 << "runtimecontroltestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontroltestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlTestModule rctm(argc, argv);

            RuntimeControlTestDummySystemPart rctdsc;

            RuntimeEnvironment rte;
            rte.add(rctm);
            rte.add(rctdsc);

            TS_ASSERT(rte.getListOfConferenceClientModules().size() == 1);
            TS_ASSERT(rte.getListOfSystemFeedbackComponents().size() == 1);

            TS_ASSERT(!rctdsc.m_setupCalled);
            TS_ASSERT(!rctdsc.m_tearDownCalled);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            TS_ASSERT(rctdsc.m_setupCalled);
            TS_ASSERT(rctdsc.m_tearDownCalled);

            // Try to add more modules after executing (should fail).
            rte.add(rctm);
            rte.add(rctdsc);

            TS_ASSERT(rte.getListOfConferenceClientModules().size() == 1);
            TS_ASSERT(rte.getListOfSystemFeedbackComponents().size() == 1);

            sc.tearDown();

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontroltestmodule.key1=value1" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 9 times (first cycle is head of app's while-loop).
            TS_ASSERT(rctm.getCycleCounter() == 9);
        }

        void testRuntimeControlSpecializedConfigurationRegularRun() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontroltestmodule.key1 = value1" << endl
                 << "runtimecontroltestmodule:241280.key2 = value2" << endl
                 << "othermodule.key3 = value3" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontroltestmodule");
            string argv1("--cid=100");
            string argv2("--id=241280");
            int32_t argc = 3;
            char **argv;
            argv = new char*[3];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());
            argv[2] = const_cast<char*>(argv2.c_str());

            RuntimeControlTestModule rctm(argc, argv);

            RuntimeControlTestDummySystemPart rctdsc;

            RuntimeEnvironment rte;
            rte.add(rctm);
            rte.add(rctdsc);

            TS_ASSERT(!rctdsc.m_setupCalled);
            TS_ASSERT(!rctdsc.m_tearDownCalled);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::RUNTIME_TIMEOUT);

            TS_ASSERT(rctdsc.m_setupCalled);
            TS_ASSERT(rctdsc.m_tearDownCalled);

            sc.tearDown();

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rctm.getConfiguration();
            stringstream sstrConfig;
            sstrConfig << "runtimecontroltestmodule.key1=value1" << endl
                       << "runtimecontroltestmodule.key2=value2" << endl;
            TS_ASSERT(sstrConfigReceived.str() == sstrConfig.str());

            // Check if the application was called 9 times (first cycle is head of app's while-loop).
            TS_ASSERT(rctm.getCycleCounter() == 9);
        }

        void testRuntimeControlCorruptConfigurationRun() {
            // Setup configuration.
            stringstream sstr;
            sstr << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);
            sc.setup(RuntimeControl::TAKE_CONTROL);

            // Setup application.
            string argv0("runtimecontroltestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlTestModule rctm(argc, argv);

            RuntimeControlTestDummySystemPart rctdsc;

            RuntimeEnvironment rte;
            rte.add(rctm);
            rte.add(rctdsc);

            TS_ASSERT(!rctdsc.m_setupCalled);
            TS_ASSERT(!rctdsc.m_tearDownCalled);

            // Catch exception due to malformed configuration.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::EXCEPTION_CAUGHT);

            TS_ASSERT(rctdsc.m_setupCalled);
            TS_ASSERT(rctdsc.m_tearDownCalled);

            sc.tearDown();

            // Check if the received configuration is valid.
            stringstream sstrConfigReceived;
            sstrConfigReceived << rctm.getConfiguration();
            TS_ASSERT(sstrConfigReceived.str() == "");

            // Check if the application was not called at all.
            TS_ASSERT(rctm.getCycleCounter() == 0);
        }

        void testRuntimeControlRegularRunNoSetupCalled() {
            // Setup configuration.
            stringstream sstr;
            sstr << "runtimecontroltestmodule.key1 = value1" << endl
                 << "runtimecontroltestmodule:241280.key2 = value2" << endl
                 << "othermodule.key2 = value2" << endl;

            DirectInterface di("225.0.0.100", 100, sstr.str());
            RuntimeControl sc(di);

            // Setup application.
            string argv0("runtimecontroltestmodule");
            string argv1("--cid=100");
            int32_t argc = 2;
            char **argv;
            argv = new char*[2];
            argv[0] = const_cast<char*>(argv0.c_str());
            argv[1] = const_cast<char*>(argv1.c_str());

            RuntimeControlTestModule rctm(argc, argv);

            RuntimeControlTestDummySystemPart rctdsc;

            RuntimeEnvironment rte;
            rte.add(rctm);
            rte.add(rctdsc);

            TS_ASSERT(rte.getListOfConferenceClientModules().size() == 1);
            TS_ASSERT(rte.getListOfSystemFeedbackComponents().size() == 1);

            TS_ASSERT(!rctdsc.m_setupCalled);
            TS_ASSERT(!rctdsc.m_tearDownCalled);

            // Run application under supervision of RuntimeControl for ten cycles.
            TS_ASSERT(sc.run(rte, 10) == RuntimeControl::SETUP_NOT_CALLED);

            TS_ASSERT(!rctdsc.m_setupCalled);
            TS_ASSERT(!rctdsc.m_tearDownCalled);

            // Try to add more modules after executing (should fail).
            rte.add(rctm);
            rte.add(rctdsc);

            // Adding still possible since call to run failed!
            TS_ASSERT(rte.getListOfConferenceClientModules().size() == 2);
            TS_ASSERT(rte.getListOfSystemFeedbackComponents().size() == 2);

            sc.tearDown();
        }
};

#endif /*CONTEXT_RUNTIMECONTROLTESTSUITE_H_*/
