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

#ifndef CORE_SHAREDPOINTERTESTSUITE_H_
#define CORE_SHAREDPOINTERTESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <iostream>
#include <vector>

#include "core/SharedPointer.h"

using namespace std;

class SharedPointerTestData {
    public:
        SharedPointerTestData() :
                val(0) {
            clog << "c'tor." << endl;
        };

        ~SharedPointerTestData() {
            clog << "d'tor." << endl;
        };
        int32_t val;
};

class SharedPointerTest : public CxxTest::TestSuite {
    public:
        void testCreateSharedPointer() {
            using namespace core;
            {
                SharedPointerTestData *sptd = new SharedPointerTestData();
                SharedPointer<SharedPointerTestData> p1(sptd);
                p1->val = 10;

                TS_ASSERT(sptd->val == 10);
                TS_ASSERT(sptd->val == p1->val);
                TS_ASSERT(sptd == p1.operator->());
            }
            clog << endl;
        }

        void testCreateDestroySharedPointer() {
            using namespace core;
            {
                SharedPointerTestData *sptd = new SharedPointerTestData();
                SharedPointer<SharedPointerTestData> p1(sptd);
                p1->val = 10;

                TS_ASSERT(sptd->val == 10);
                TS_ASSERT(sptd->val == p1->val);
                TS_ASSERT(sptd == p1.operator->());

                p1.release();
                TS_ASSERT(!p1.isValid());
            }
            clog << endl;
        }

        void testCreateDestroySharedPointerUsingCopyConstructor() {
            using namespace core;
            // Create instance.
            SharedPointer<SharedPointerTestData> p1(new SharedPointerTestData());
            {
                // Handover instance.
                SharedPointer<SharedPointerTestData> p2(p1);
                p1->val = 11;

                TS_ASSERT(p1->val == 11);
                TS_ASSERT(p2->val == p1->val);
                TS_ASSERT(p1.operator ->() == p2.operator->());

                // Remove first instance.
                p1 = SharedPointer<SharedPointerTestData>();
                TS_ASSERT(!p1.isValid());

                TS_ASSERT(p2->val == 11);
            }
            TS_ASSERT(!p1.isValid());
            clog << endl;
        }

        void testCreateDestroySharedPointerUsingAssignmentOperator() {
            using namespace core;
            // Create instance.
            SharedPointer<SharedPointerTestData> p1(new SharedPointerTestData());
            {
                // Handover instance.
                SharedPointer<SharedPointerTestData> p2 = p1;
                p1->val = 12;

                TS_ASSERT(p1->val == 12);
                TS_ASSERT(p2->val == p1->val);
                TS_ASSERT(p1.operator ->() == p2.operator->());

                // Remove first instance.
                p1 = SharedPointer<SharedPointerTestData>();
                TS_ASSERT(!p1.isValid());

                TS_ASSERT(p2->val == 12);
            }
            TS_ASSERT(!p1.isValid());
            clog << endl;
        }

        void testCreateAndKeepSharedPointerUsingCopyConstructor() {
            using namespace core;

            // Create empty instance.
            SharedPointer<SharedPointerTestData> p1;
            TS_ASSERT(!p1.isValid());
            {
                SharedPointer<SharedPointerTestData> p2(new SharedPointerTestData());
                p2->val = 13;

                TS_ASSERT(p2->val == 13);

                // Hand over to first instance.
                p1 = SharedPointer<SharedPointerTestData>(p2);
                TS_ASSERT(p1->val == 13);
                TS_ASSERT(p2->val == 13);

                // Remove second instance.
                p2 = SharedPointer<SharedPointerTestData>();
                TS_ASSERT(p1->val == 13);
                TS_ASSERT(!p2.isValid());
            }
            TS_ASSERT(p1.isValid());
            TS_ASSERT(p1->val == 13);
            clog << endl;
        }

        void testCreateAndKeepSharedPointerUsingAssignmentOperator() {
            using namespace core;

            // Create empty instance.
            SharedPointer<SharedPointerTestData> p1;
            TS_ASSERT(!p1.isValid());
            {
                // Handover instance.
                SharedPointer<SharedPointerTestData> p2(new SharedPointerTestData());
                p2->val = 14;

                TS_ASSERT(p2->val == 14);

                // Hand over to first instance.
                p1 = p2;
                TS_ASSERT(p1->val == 14);
                TS_ASSERT(p2->val == 14);

                // Remove second instance.
                p2 = SharedPointer<SharedPointerTestData>();
                TS_ASSERT(p1->val == 14);
                TS_ASSERT(!p2.isValid());
            }
            TS_ASSERT(p1.isValid());
            TS_ASSERT(p1->val == 14);
            clog << endl;
        }

        void testSharedPointerInsideSTL() {
            using namespace core;

            vector<SharedPointer<SharedPointerTestData> > listOfPtrs;
            TS_ASSERT(listOfPtrs.empty());

            {
                SharedPointer<SharedPointerTestData> p1(new SharedPointerTestData());
                listOfPtrs.push_back(p1);
                p1->val = 42;

                SharedPointer<SharedPointerTestData> p2(new SharedPointerTestData());
                listOfPtrs.push_back(p2);
                p2->val = 43;

                SharedPointer<SharedPointerTestData> p3(new SharedPointerTestData());
                listOfPtrs.push_back(p3);
                p3->val = 44;

                SharedPointer<SharedPointerTestData> p4(new SharedPointerTestData());
                listOfPtrs.push_back(p4);
                p4->val = 45;
            }

            TS_ASSERT(!listOfPtrs.empty());

            vector<SharedPointer<SharedPointerTestData> >::iterator it = listOfPtrs.begin();
            int32_t counter = 42;
            while (it != listOfPtrs.end()) {
                TS_ASSERT( (*it++)->val == counter );
                counter++;
            }
            clog << endl;
        }

        void testSharedPointerInsideSTLAndCopy() {
            using namespace core;

            vector<SharedPointer<SharedPointerTestData> > listOfPtrs;
            TS_ASSERT(listOfPtrs.empty());

            vector<SharedPointer<SharedPointerTestData> > listOfPtrs2;
            TS_ASSERT(listOfPtrs2.empty());

            {
                SharedPointer<SharedPointerTestData> p1(new SharedPointerTestData());
                listOfPtrs.push_back(p1);
                p1->val = 42;

                SharedPointer<SharedPointerTestData> p2(new SharedPointerTestData());
                listOfPtrs.push_back(p2);
                p2->val = 43;

                SharedPointer<SharedPointerTestData> p3(new SharedPointerTestData());
                listOfPtrs.push_back(p3);
                p3->val = 44;

                SharedPointer<SharedPointerTestData> p4(new SharedPointerTestData());
                listOfPtrs.push_back(p4);
                p4->val = 45;
            }

            TS_ASSERT(!listOfPtrs.empty());

            vector<SharedPointer<SharedPointerTestData> >::iterator it = listOfPtrs.begin();
            int32_t counter = 42;
            while (it != listOfPtrs.end()) {
                TS_ASSERT( (*it++)->val == counter );
                counter++;
            }

            listOfPtrs2 = listOfPtrs;

            TS_ASSERT(!listOfPtrs2.empty());

            vector<SharedPointer<SharedPointerTestData> >::iterator jt = listOfPtrs2.begin();
            counter = 42;
            while (jt != listOfPtrs2.end()) {
                TS_ASSERT( (*jt++)->val == counter );
                counter++;
            }

            TS_ASSERT((*(listOfPtrs.begin()))->val == 42);
            TS_ASSERT((*(listOfPtrs2.begin()))->val == 42);

            // Change first entry of first list.
            // The first entry of the second list should have changed too.
            (*(listOfPtrs.begin()))->val = 24;
            TS_ASSERT((*(listOfPtrs.begin()))->val == 24);
            TS_ASSERT((*(listOfPtrs2.begin()))->val == 24);
        }
};

#endif /*CORE_SHAREDPOINTERTESTSUITE_H_*/
