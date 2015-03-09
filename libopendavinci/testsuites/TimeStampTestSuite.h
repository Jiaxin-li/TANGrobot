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

#ifndef CORE_TIMESTAMPTESTSUITE_H_
#define CORE_TIMESTAMPTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <sstream>
#include <string>

#include "core/data/TimeStamp.h"

using namespace std;
using namespace core::data;

class TimeStampTest : public CxxTest::TestSuite {
    public:
        void testAdd1() {
            TimeStamp ts1(1, 2);
            TimeStamp ts2(3, 4);
            TimeStamp ts = ts1 + ts2;

            TS_ASSERT(ts.getSecond() == 4);
            TS_ASSERT(ts.getFractionalMicroseconds() == 6);
        }

        void testAdd2() {
            TimeStamp ts1(1, 900*1000);
            TimeStamp ts2(3, 200*1000);
            TimeStamp ts = ts1 + ts2;

            TS_ASSERT(ts.getSecond() == 5);
            TS_ASSERT(ts.getFractionalMicroseconds() == 100*1000);
        }

        void testSub1() {
            TimeStamp ts1(3, 5);
            TimeStamp ts2(1, 4);
            TimeStamp ts = ts1 - ts2;

            TS_ASSERT(ts.getSecond() == 2);
            TS_ASSERT(ts.getFractionalMicroseconds() == 1);
        }

        void testSub2() {
            TimeStamp ts1(3, 4);
            TimeStamp ts2(1, 5);
            TimeStamp ts = ts1 - ts2;

            TS_ASSERT(ts.getSecond() == 1);
            TS_ASSERT(ts.getFractionalMicroseconds() == 999999);
        }

        void testTimeStamp28042009() {
            TimeStamp ts(1240926174, 1234);

            TS_ASSERT(ts.getDay() == 28);
            TS_ASSERT(ts.getMonth() == 4);
            TS_ASSERT(ts.getYear() == 2009);
            TS_ASSERT(ts.getHour() == 13);
            TS_ASSERT(ts.getMinute() == 42);
            TS_ASSERT(ts.getSecond() == 54);

            TimeStamp ts2("28042009134254");

            TS_ASSERT(ts2.getDay() == 28);
            TS_ASSERT(ts2.getMonth() == 4);
            TS_ASSERT(ts2.getYear() == 2009);
            TS_ASSERT(ts2.getHour() == 13);
            TS_ASSERT(ts2.getMinute() == 42);
            TS_ASSERT(ts2.getSecond() == 54);
        }
};

#endif /*CORE_TIMESTAMPTESTSUITE_H_*/
