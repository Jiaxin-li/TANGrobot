/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2014 - 2015 Christian Berger
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

#include "core/wrapper/MutexFactory.h"
#include "core/wrapper/WIN32/WIN32Thread.h"

namespace core {
    namespace wrapper {
        namespace WIN32Impl {

            using namespace std;

            /**
             * This method encapsulates the runnable.
             */
            void *threadRunner(void *v) {
                Runnable *runnable = static_cast<Runnable*>(v);

                try {
                    runnable->run();
                } catch (string &s) {
                    clog << "Exception caught at " << __FILE__ << ":" << __LINE__ << ": " << s << endl;
                    throw;
                } catch (...) {
                    clog << "Unknown exception caught at " << __FILE__ << ":" << __LINE__ << "." << endl;
                    throw;
                }

                return NULL;
            }

            WIN32Thread::WIN32Thread(Runnable &r) :
                    m_threadStateMutex(NULL),
                    m_threadState(INITIALIZED),
                    m_runnable(r),
                    m_theThread() {
                // Create mutex.
                m_threadStateMutex = MutexFactory::createMutex();
                if (m_threadStateMutex == NULL) {
                    stringstream s;
                    s << "Error while creating mutex at " << __FILE__ << ": " << __LINE__;
                    throw s.str();
                }
            }

            WIN32Thread::~WIN32Thread() {
                stop();

                if (m_threadStateMutex != NULL) {
                    delete m_threadStateMutex;
                }
                m_threadStateMutex = NULL;
            }

            bool WIN32Thread::start() {
                m_threadStateMutex->lock();
                {
                    if (m_threadState == INITIALIZED) {
                        m_theThread = std::thread(threadRunner, &m_runnable);
                        m_threadState = RUNNING;
                    }
                }
                m_threadStateMutex->unlock();

                return isRunning();
            }

            bool WIN32Thread::stop() {
                bool doJoin = false;
                m_threadStateMutex->lock();
                {
                    if (m_threadState == RUNNING) {
                        doJoin = true;
                        m_threadState = STOPPED;
                    }
                }
                m_threadStateMutex->unlock();

                if (doJoin) {
                    if (m_theThread.joinable()) {
                        m_theThread.join();
                    }
                }

                return !isRunning();
            }

            bool WIN32Thread::isRunning() const {
                bool retVal = false;
                m_threadStateMutex->lock();
                {
                    retVal = (m_threadState == RUNNING);
                }
                m_threadStateMutex->unlock();

                return retVal;
            }

        }
    }
} // core::wrapper::WIN32Impl
