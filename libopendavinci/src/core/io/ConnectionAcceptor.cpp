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

#include "core/base/Lock.h"
#include "core/io/ConnectionAcceptor.h"
#include "core/exceptions/Exceptions.h"
#include "core/wrapper/TCPFactory.h"

namespace core {
    namespace io {
        using namespace std;
        using namespace core::base;

        ConnectionAcceptor::ConnectionAcceptor(const uint32_t port)
                : m_listener(NULL),
                m_acceptor(NULL),
                m_listenerMutex() {
            try {
                m_acceptor = core::wrapper::TCPFactory::createTCPAcceptor(port);
            } catch (std::string s) {
                OPENDAVINCI_CORE_THROW_EXCEPTION(ConnectionAcceptorException , s);
            }

            m_acceptor->setAcceptorListener(this);
        }

        ConnectionAcceptor::~ConnectionAcceptor() {
            m_acceptor->stop();
            m_acceptor->setAcceptorListener(NULL);
            OPENDAVINCI_CORE_DELETE_POINTER(m_acceptor);
        }

        void ConnectionAcceptor::setConnectionAcceptorListener(ConnectionAcceptorListener* listener) {
            Lock l(m_listenerMutex);
            m_listener = listener;
        }

        void ConnectionAcceptor::onNewConnection(core::wrapper::TCPConnection* connection) {
            Lock l(m_listenerMutex);
            if (m_listener != NULL) {
                m_listener->onNewConnection(new core::io::Connection(connection));
            } else {
                clog << "[ConnectionAcceptor|" << this << "] no listener to call" << endl;
            }
        }

        void ConnectionAcceptor::start() {
            m_acceptor->start();
        }

        void ConnectionAcceptor::stop() {
            m_acceptor->stop();
        }
    }
}
