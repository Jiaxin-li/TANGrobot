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

#ifndef CORE_PULSEACKCONTAINERSMESSAGETESTSUITE_H_
#define CORE_PULSEACKCONTAINERSMESSAGETESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <sstream>

#include "core/data/Container.h"
#include "core/data/TimeStamp.h"
#include "core/data/dmcp/PulseMessage.h"
#include "core/data/dmcp/PulseAckContainersMessage.h"

using namespace std;
using namespace core::data;
using namespace core::data::dmcp;

class PulseAckContainersTest : public CxxTest::TestSuite {
    public:
        void testPulseEmpty() {
            PulseMessage pm;
            TS_ASSERT(pm.getListOfContainers().size() == 0);

            stringstream sstr;
            sstr << pm;

            PulseMessage pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 0);
            sstr >> pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 0);
        }

        void testPulseContainers() {
            TimeStamp ts1(1,2);
            Container c1(Container::TIMESTAMP, ts1);

            TimeStamp ts2(3,4);
            Container c2(Container::TIMESTAMP, ts2);

            TimeStamp ts3(5,6);
            Container c3(Container::TIMESTAMP, ts3);

            PulseMessage pm;
            TS_ASSERT(pm.getListOfContainers().size() == 0);

            pm.addContainer(c1);
            TS_ASSERT(pm.getListOfContainers().size() == 1);

            pm.addContainer(c2);
            TS_ASSERT(pm.getListOfContainers().size() == 2);

            pm.addContainer(c3);
            TS_ASSERT(pm.getListOfContainers().size() == 3);

            stringstream sstr;
            sstr << pm;

            PulseMessage pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 0);
            sstr >> pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 3);

            vector<Container> listOfContainers = pm2.getListOfContainers();
            TS_ASSERT(listOfContainers.size() == 3);

            Container c4 = listOfContainers.at(0);
            Container c5 = listOfContainers.at(1);
            Container c6 = listOfContainers.at(2);

            TimeStamp ts4 = c4.getData<TimeStamp>();
            TimeStamp ts5 = c5.getData<TimeStamp>();
            TimeStamp ts6 = c6.getData<TimeStamp>();

            TS_ASSERT(ts1.toMicroseconds() == ts4.toMicroseconds());
            TS_ASSERT(ts2.toMicroseconds() == ts5.toMicroseconds());
            TS_ASSERT(ts3.toMicroseconds() == ts6.toMicroseconds());
        }

        void testPulseContainersSetValues() {
            TimeStamp ts1(1,2);
            Container c1(Container::TIMESTAMP, ts1);

            TimeStamp ts2(3,4);
            Container c2(Container::TIMESTAMP, ts2);

            TimeStamp ts3(5,6);
            Container c3(Container::TIMESTAMP, ts3);

            vector<Container> l;
            l.push_back(c1);
            l.push_back(c2);
            l.push_back(c3);

            PulseMessage pm;
            TS_ASSERT(pm.getListOfContainers().size() == 0);

            pm.setListOfContainers(l);
            TS_ASSERT(pm.getListOfContainers().size() == 3);

            stringstream sstr;
            sstr << pm;

            PulseMessage pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 0);
            sstr >> pm2;
            TS_ASSERT(pm2.getListOfContainers().size() == 3);

            vector<Container> listOfContainers = pm2.getListOfContainers();
            TS_ASSERT(listOfContainers.size() == 3);

            Container c4 = listOfContainers.at(0);
            Container c5 = listOfContainers.at(1);
            Container c6 = listOfContainers.at(2);

            TimeStamp ts4 = c4.getData<TimeStamp>();
            TimeStamp ts5 = c5.getData<TimeStamp>();
            TimeStamp ts6 = c6.getData<TimeStamp>();

            TS_ASSERT(ts1.toMicroseconds() == ts4.toMicroseconds());
            TS_ASSERT(ts2.toMicroseconds() == ts5.toMicroseconds());
            TS_ASSERT(ts3.toMicroseconds() == ts6.toMicroseconds());
        }

        void testPulseAckContainersEmpty() {
            PulseAckContainersMessage pac;
            TS_ASSERT(pac.getListOfContainers().size() == 0);

            stringstream sstr;
            sstr << pac;

            PulseAckContainersMessage pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 0);
            sstr >> pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 0);
        }

        void testPulseAckContainers() {
            TimeStamp ts1(1,2);
            Container c1(Container::TIMESTAMP, ts1);

            TimeStamp ts2(3,4);
            Container c2(Container::TIMESTAMP, ts2);

            TimeStamp ts3(5,6);
            Container c3(Container::TIMESTAMP, ts3);

            PulseAckContainersMessage pac;
            TS_ASSERT(pac.getListOfContainers().size() == 0);

            pac.addContainer(c1);
            TS_ASSERT(pac.getListOfContainers().size() == 1);

            pac.addContainer(c2);
            TS_ASSERT(pac.getListOfContainers().size() == 2);

            pac.addContainer(c3);
            TS_ASSERT(pac.getListOfContainers().size() == 3);

            stringstream sstr;
            sstr << pac;

            PulseAckContainersMessage pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 0);
            sstr >> pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 3);

            vector<Container> listOfContainers = pac2.getListOfContainers();
            TS_ASSERT(listOfContainers.size() == 3);

            Container c4 = listOfContainers.at(0);
            Container c5 = listOfContainers.at(1);
            Container c6 = listOfContainers.at(2);

            TimeStamp ts4 = c4.getData<TimeStamp>();
            TimeStamp ts5 = c5.getData<TimeStamp>();
            TimeStamp ts6 = c6.getData<TimeStamp>();

            TS_ASSERT(ts1.toMicroseconds() == ts4.toMicroseconds());
            TS_ASSERT(ts2.toMicroseconds() == ts5.toMicroseconds());
            TS_ASSERT(ts3.toMicroseconds() == ts6.toMicroseconds());
        }

        void testPulseAckContainers_from_list() {
            TimeStamp ts1(1,2);
            Container c1(Container::TIMESTAMP, ts1);

            TimeStamp ts2(3,4);
            Container c2(Container::TIMESTAMP, ts2);

            TimeStamp ts3(5,6);
            Container c3(Container::TIMESTAMP, ts3);

            vector<Container> l;
            l.push_back(c1);
            l.push_back(c2);
            l.push_back(c3);

            PulseAckContainersMessage pac;
            TS_ASSERT(pac.getListOfContainers().size() == 0);

            pac.setListOfContainers(l);
            TS_ASSERT(pac.getListOfContainers().size() == 3);

            stringstream sstr;
            sstr << pac;

            PulseAckContainersMessage pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 0);
            sstr >> pac2;
            TS_ASSERT(pac2.getListOfContainers().size() == 3);

            vector<Container> listOfContainers = pac2.getListOfContainers();
            TS_ASSERT(listOfContainers.size() == 3);

            Container c4 = listOfContainers.at(0);
            Container c5 = listOfContainers.at(1);
            Container c6 = listOfContainers.at(2);

            TimeStamp ts4 = c4.getData<TimeStamp>();
            TimeStamp ts5 = c5.getData<TimeStamp>();
            TimeStamp ts6 = c6.getData<TimeStamp>();

            TS_ASSERT(ts1.toMicroseconds() == ts4.toMicroseconds());
            TS_ASSERT(ts2.toMicroseconds() == ts5.toMicroseconds());
            TS_ASSERT(ts3.toMicroseconds() == ts6.toMicroseconds());
        }
};

#endif /*CORE_PULSEACKCONTAINERSMESSAGETESTSUITE_H_*/
