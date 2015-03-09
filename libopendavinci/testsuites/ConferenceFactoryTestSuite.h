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

#ifndef CONTEXT_CONFERENCEFACTORYTESTSUITE_H_
#define CONTEXT_CONFERENCEFACTORYTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <iostream>

#include "core/macros.h"
#include "core/base/FIFOQueue.h"
#include "core/base/Thread.h"
#include "core/data/Container.h"
#include "core/data/TimeStamp.h"
#include "core/io/ContainerConferenceFactory.h"
#include "core/io/ContainerListener.h"
#include "core/io/UDPMultiCastContainerConference.h"
#include "context/base/ControlledContainerConference.h"
#include "context/base/ControlledContainerConferenceForSystemUnderTest.h"
#include "context/base/ControlledContainerConferenceFactory.h"

using namespace std;
using namespace core::base;
using namespace core::data;
using namespace core::io;
using namespace context::base;

class ConferenceFactoryTestContainerListener : public ContainerListener {
    public:
        ConferenceFactoryTestContainerListener() :
            m_receivedContainer() {}

        virtual ~ConferenceFactoryTestContainerListener() {}

        virtual void nextContainer(Container &c) {
            m_receivedContainer = c;
        }

        Container getReceivedContainer() {
            return m_receivedContainer;
        }

    private:
        Container m_receivedContainer;
};

class ConferenceFactoryTestContainerListenerForContainerFromSystemsUnderTest : public ContainerListener {
    public:
        ConferenceFactoryTestContainerListenerForContainerFromSystemsUnderTest() :
            m_fifo() {}

        virtual ~ConferenceFactoryTestContainerListenerForContainerFromSystemsUnderTest() {}

        virtual void nextContainer(Container &c) {
            m_fifo.add(c);
        }

        FIFOQueue& getFIFO() {
            return m_fifo;
        }

    private:
        FIFOQueue m_fifo;
};

class ConferenceFactoryTest : public CxxTest::TestSuite {
    public:
        void testControlledContainerFactoryTestSuite() {
            // Destroy any existing ContainerConferenceFactory.
            ContainerConferenceFactory &ccf = ContainerConferenceFactory::getInstance();
            ContainerConferenceFactory *ccf2 = &ccf;
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);

            // Create regular ContainerConference.
            const string group = "225.0.0.200";
            ContainerConference *udpCF = ContainerConferenceFactory::getInstance().getContainerConference(group);
            TS_ASSERT(udpCF != NULL);
            bool castIntoUDPCFSuccessful = false;
            try {
                UDPMultiCastContainerConference *udpmccf = dynamic_cast<UDPMultiCastContainerConference*>(udpCF);
                if (udpmccf != NULL) {
                    castIntoUDPCFSuccessful = true;
                }
            }
            catch(...) {}
            TS_ASSERT(castIntoUDPCFSuccessful);

            // Destroy existing ContainerConferenceFactory.
            ContainerConferenceFactory &ccfDestroy = ContainerConferenceFactory::getInstance();
            ccf2 = &ccfDestroy;
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);
            OPENDAVINCI_CORE_DELETE_POINTER(udpCF);

            // Exchange ContainerConferenceFactory.
            ControlledContainerConferenceFactory *controlledCF = new ControlledContainerConferenceFactory();
            ContainerConferenceFactory &controlledccf = ContainerConferenceFactory::getInstance();
            ccf2 = &controlledccf;
            TS_ASSERT(ccf2 == controlledCF);

            // Get ControlledContainerConference.
            ContainerConference *cf = controlledccf.getContainerConference(group);
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

            // Register ContainerListener and send some data.
            ConferenceFactoryTestContainerListener listener;
            controlledConferenceForSystemUnderTest->setContainerListener(&listener);

            // Register ContainerListener to get Container sent by Systems Under Test.
            ConferenceFactoryTestContainerListenerForContainerFromSystemsUnderTest cftctlfcfsut;
            controlledCF->add(&cftctlfcfsut);

            TimeStamp tsSendFromSimulatorToContainerConference(1, 2);
            Container c = Container(Container::TIMESTAMP, tsSendFromSimulatorToContainerConference);
            controlledCF->sendToSystemsUnderTest(c);

            TS_ASSERT(listener.getReceivedContainer().getDataType() == c.getDataType());
            TimeStamp tsCheckReceivedTimeStampFromSimulator = listener.getReceivedContainer().getData<TimeStamp>();
            TS_ASSERT(tsCheckReceivedTimeStampFromSimulator.toMicroseconds() == tsSendFromSimulatorToContainerConference.toMicroseconds());
            TS_ASSERT(tsCheckReceivedTimeStampFromSimulator.toString() == tsSendFromSimulatorToContainerConference.toString());

            controlledConferenceForSystemUnderTest->getBlockableContainerReceiver().setNextContainerAllowed(true);
            // Send some data from application to controlled container conference.
            TimeStamp tsSendFromApplicationToContainerConference(3, 4);
            c = Container(Container::TIMESTAMP, tsSendFromApplicationToContainerConference);
            controlledConferenceForSystemUnderTest->send(c);
            controlledConferenceForSystemUnderTest->getBlockableContainerReceiver().setNextContainerAllowed(false);

            FIFOQueue &queueContainingContainersSentToConference = cftctlfcfsut.getFIFO();
            TS_ASSERT(queueContainingContainersSentToConference.getSize() == 2);
            if (queueContainingContainersSentToConference.getSize() == 2) {
                c = queueContainingContainersSentToConference.leave(); // Sent from SCC.
                c = queueContainingContainersSentToConference.leave(); // Sent from APP.
                TimeStamp tsCheckReceivedTimeStampFromApplication = c.getData<TimeStamp>();
                TS_ASSERT(tsCheckReceivedTimeStampFromApplication.toMicroseconds() == tsSendFromApplicationToContainerConference.toMicroseconds());
                TS_ASSERT(tsCheckReceivedTimeStampFromApplication.toString() == tsSendFromApplicationToContainerConference.toString());
            }

            // Destroy existing ContainerConferenceFactory.
            OPENDAVINCI_CORE_DELETE_POINTER(ccf2);
            OPENDAVINCI_CORE_DELETE_POINTER(controlledConferenceForSystemUnderTest);
        }
};

#endif /*CONTEXT_CONFERENCEFACTORYTESTSUITE_H_*/
