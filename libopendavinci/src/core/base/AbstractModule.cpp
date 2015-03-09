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

#include <cstdlib>
#include <csignal>

#include "core/base/AbstractModule.h"
#include "core/base/CommandLineParser.h"
#include "core/base/Lock.h"
#include "core/base/Thread.h"
#include "core/wrapper/DisposalService.h"

namespace core {
    namespace base {

        using namespace std;
        using namespace exceptions;

        // Signal handler structs.
#ifndef WIN32
        struct sigaction ourSignalHandler;
        struct sigaction ignoreSignal;
#endif

        // Initialization of the static attribute.
        vector<AbstractModule*> AbstractModule::m_listOfModules;

        void signalHandler(int32_t signal) {
            clog << "Module caught signal number " << signal << "." << endl;

            vector<AbstractModule*>::iterator it = AbstractModule::getListOfModules().begin();
            while (it != AbstractModule::getListOfModules().end()) {
                AbstractModule *m = *it++;
                if (m != NULL) {
                    m->setModuleState(ModuleState::NOT_RUNNING);
                }
            }
        }

        void finalize() {
            if (AbstractModule::getListOfModules().size() == 0) {
                clog << "Calling disposal service..." << endl;
                wrapper::DisposalService &ds = wrapper::DisposalService::getInstance();
                ds.cleanUpFinally();
            }
        }

        AbstractModule::AbstractModule() :
                m_moduleStateMutex(),
                m_moduleState(ModuleState::NOT_RUNNING) {
            m_listOfModules.push_back(this);

            atexit(finalize);

#ifdef WIN32 /* The following code is for Windows. */
            // Catch signal SIGINT (ctrl-c) from console.
            if (::signal(SIGINT, &signalHandler) == SIG_ERR) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(InvalidArgumentException, "Failed to register signal SIGINT.");
            }
#endif

#ifndef WIN32 /* The following code is only supported on POSIX. */
            memset(&ourSignalHandler, 0, sizeof(ourSignalHandler));
            ourSignalHandler.sa_handler = &signalHandler;

            // Catch signal SIGINT (ctrl-c) from console.
            if (::sigaction(SIGINT, &ourSignalHandler, NULL) < 0) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(InvalidArgumentException, "Failed to register signal SIGINT.");
            }

            memset(&ignoreSignal, 0, sizeof(ignoreSignal));
            ignoreSignal.sa_handler = SIG_IGN;

            // Ignore signal SIGPIPE that is triggered in the case that a socket connection is closed.
            // If this called would be missing the program would simply be closed (which is the unintended behavior).
            if (::sigaction(SIGPIPE, &ignoreSignal, NULL) < 0) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(InvalidArgumentException, "Failed to ignore signal SIGPIPE.");
            }
#endif
		}

        AbstractModule::~AbstractModule() {
            vector<AbstractModule*>::iterator it = AbstractModule::getListOfModules().begin();
            while (it != AbstractModule::getListOfModules().end()) {
                AbstractModule *m = *it;
                if (m == this) {
                    break;
                }
                it++;
            }

            if (it != AbstractModule::getListOfModules().end()) {
                AbstractModule::getListOfModules().erase(it);
            }
        }

        vector<AbstractModule*>& AbstractModule::getListOfModules() {
            return m_listOfModules;
        }

        void AbstractModule::setModuleState(const ModuleState::MODULE_STATE &s) {
            Lock l(m_moduleStateMutex);
            m_moduleState = s;
        }

        void AbstractModule::wait() {
            Thread::usleep(25);
        }

        void AbstractModule::calledGetModuleState() {
            wait();
        }

        ModuleState::MODULE_STATE AbstractModule::getModuleState() {
            calledGetModuleState();

            Lock l(m_moduleStateMutex);
            return m_moduleState;
        }

    }
} // core::base
