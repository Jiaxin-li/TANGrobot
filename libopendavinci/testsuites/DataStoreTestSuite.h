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

#ifndef CORE_DATASTORETESTSUITE_H_
#define CORE_DATASTORETESTSUITE_H_

#include "cxxtest/TestSuite.h"

#include <cstdlib>

#include <fstream>
#include <string>

#include "core/base/Hash.h"
#include "core/base/KeyValueDataStore.h"
#include "core/base/Serializable.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Service.h"
#include "core/base/Thread.h"
#include "core/data/SerializableData.h"
#include "core/data/TimeStamp.h"
#include "core/wrapper/KeyValueDatabaseFactory.h"

using namespace std;
using namespace core::base;
using namespace core::data;

class DataStoreTestService : public Service {
    public:
        DataStoreTestService(KeyValueDataStore &ds) :
                m_ds(ds), m_found(false) {
        }

        void beforeStop() {}

        void run() {
            serviceReady();
            while (isRunning()) {
                Container c = m_ds.get(0);
                m_found |= (c.getDataType() == Container::TIMESTAMP);

                // Yield other threads.
                Thread::usleep(10);
            }
        }

        bool hasFound() {
            return m_found;
        }

    private:
        KeyValueDataStore &m_ds;
        bool m_found;
};

class DataStoreTestNestedData : public core::data::SerializableData {
    public:
        DataStoreTestNestedData() :
                m_double(0) {
        }

        double m_double;

        ostream& operator<<(ostream &out) const {
            SerializationFactory sf;

            Serializer &s = sf.getSerializer(out);

            s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('m', '_', 'd', 'o', 'u',
                    'b', 'l', 'e') >::RESULT, m_double);

            return out;
        }

        istream& operator>>(istream &in) {
            SerializationFactory sf;

            Deserializer &d = sf.getDeserializer(in);

            d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('m', '_', 'd', 'o', 'u',
                    'b', 'l', 'e') >::RESULT, m_double);

            return in;
        }

        const string toString() const {
            stringstream s;
            s << m_double << endl;
            return s.str();
        }
};

class DataStoreTestSampleData : public core::data::SerializableData {
    public:
        DataStoreTestSampleData() :
                m_bool(false), m_int(0), m_string(""), m_nestedData() {
        }

        bool m_bool;
        int32_t m_int;
        string m_string;
        DataStoreTestNestedData m_nestedData;

        ostream& operator<<(ostream &out) const {
            SerializationFactory sf;

            Serializer &s = sf.getSerializer(out);

            s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('m', '_', 'n', 'e', 's',
                    't', 'e', 'd') >::RESULT, m_nestedData);

            s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('m', '_', 'b', 'o', 'o',
                    'l') >::RESULT, m_bool);

            s.write(
                CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 'i', 'n', 't') >::RESULT,
                m_int);

            s.write(
                CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 's', 't', 'r') >::RESULT,
                m_string);

            return out;
        }

        istream& operator>>(istream &in) {
            SerializationFactory sf;

            Deserializer &d = sf.getDeserializer(in);

            d.read(
                CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 's', 't', 'r') >::RESULT,
                m_string);

            d.read(
                CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('m', '_', 'i', 'n', 't') >::RESULT,
                m_int);

            d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('m', '_', 'n', 'e', 's',
                    't', 'e', 'd') >::RESULT, m_nestedData);

            d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('m', '_', 'b', 'o', 'o',
                    'l') >::RESULT, m_bool);

            return in;
        }

        const string toString() const {
            stringstream s;
            s << m_bool << endl << m_int << endl << m_string << endl
            << m_nestedData.toString();
            return s.str();
        }
};

class DataStoreTest : public CxxTest::TestSuite {
    public:

        void testDataStore() {
            KeyValueDataStore *ds = new KeyValueDataStore(core::wrapper::KeyValueDatabaseFactory::createKeyValueDatabase(""));
            int32_t key1 = 1;
            TimeStamp ts1(0, 35);
            Container v1(Container::TIMESTAMP, ts1);
            ds->put(key1, v1);

            Container v2;
            v2 = ds->get(key1);
            TimeStamp ts2;
            ts2 = v2.getData<TimeStamp>();
            TS_ASSERT(ts1.toString() == ts2.toString());

            // Test for no entry.
            Container v3 = ds->get(key1 - 1);
            TS_ASSERT(v3.getDataType() == Container::UNDEFINEDDATA);

            // Clean up.
            delete ds;
        }

        void testMassData() {
            KeyValueDataStore *ds = new KeyValueDataStore(core::wrapper::KeyValueDatabaseFactory::createKeyValueDatabase(""));

            const uint32_t size = 4096;
            // Generate data.
            Container c[size];
            for (uint32_t i = 0; i < size; i++) {
                TimeStamp ts;
                c[i] = Container(Container::TIMESTAMP, ts);
                Thread::usleep(10);
            }
#ifndef WIN32
            long start = clock();
#endif
            for (uint32_t i = 0; i < size; i++) {
                ds->put(i, c[i]);
            }
#ifndef WIN32
            long end = clock();
            double duration = ((end - start) * 1000) / CLOCKS_PER_SEC;
            clog << size << " entries of type TIMESTAMP each containing "
                 << sizeof(c[0]) << " bytes stored in " << duration
                 << "ms." << endl;
#endif

#ifndef WIN32
            start = clock();
#endif
            for (uint32_t i = 0; i < size; i++) {
                TimeStamp retrievedTimeStamp = ds->get(i).getData<TimeStamp>();
                TimeStamp originalTimeStamp = c[i].getData<TimeStamp>();
                TS_ASSERT(originalTimeStamp.toMicroseconds() == retrievedTimeStamp.toMicroseconds());
            }
#ifndef WIN32
            end = clock();
            duration = ((end - start) * 1000) / CLOCKS_PER_SEC;
            clog << size
                 << " entries each containing " << sizeof(c[0]) << " bytes retrieved and compared in "
                 << duration << "ms." << endl;
#endif

            // Clean up.
            delete ds;
        }

        void testSerializationDeserialization() {
            KeyValueDataStore *ds = new KeyValueDataStore(core::wrapper::KeyValueDatabaseFactory::createKeyValueDatabase(""));

            DataStoreTestSampleData sd1;
            sd1.m_bool = true;
            sd1.m_int = 42;
            sd1.m_nestedData.m_double = -21.54;
            sd1.m_string = "This is an example for marshalling data.";

            // This is only undefined data.
            Container c(Container::UNDEFINEDDATA, sd1);
            ds->put(1, c);

            Container c2 = ds->get(1);
            DataStoreTestSampleData sd2;
            sd2 = c2.getData<DataStoreTestSampleData>();

            TS_ASSERT(sd1.m_bool == sd2.m_bool);
            TS_ASSERT(sd1.m_int == sd2.m_int);
            TS_ASSERT(sd1.m_string == sd2.m_string);
            TS_ASSERT_DELTA(sd1.m_nestedData.m_double, sd2.m_nestedData.m_double, 1e-4);

            // Clean up.
            delete ds;
        }

        /**
         * This test case uses the BerkeleyDB for data interchange.
         */
        void testDataStoreForDataExchange() {
            bool failed = true;
            try {
                KeyValueDataStore *ds = new KeyValueDataStore(core::wrapper::KeyValueDatabaseFactory::createKeyValueDatabase(""));

                DataStoreTestService s(*ds);
                s.start();

                TimeStamp ts;
                Container timeStampContainer(Container::TIMESTAMP, ts);
                DataStoreTestSampleData sd;
                Container undefinedDataContainer(Container::UNDEFINEDDATA, sd);

                for (int32_t i = 0; i < 10240; i++) {
                    Thread::usleep(10); // Allow the service to check for data.
#if defined(WIN32)
                    if (i < 1024*3) {
                        ds->put(0, timeStampContainer);
                    } else {
                        ds->put(0, undefinedDataContainer);
                    }
#else
                    double r = (random() % 100);
                    if (r < 51) {
                        ds->put(0, timeStampContainer);
                    } else {
                        ds->put(0, undefinedDataContainer);
                    }
#endif
                }

                s.stop();

                // Clean up.
                delete ds;

                TS_ASSERT(s.hasFound());

                failed = false;
            } catch (...) {

            }
            TS_ASSERT(!failed);
        }

       void testSerializationDeserializationWithFileBackend() {
           unlink("log.0000000001");
           unlink("test.db");

           KeyValueDataStore *ds = new KeyValueDataStore(core::wrapper::KeyValueDatabaseFactory::createKeyValueDatabase("test.db"));

           DataStoreTestSampleData sd1;
           sd1.m_bool = true;
           sd1.m_int = 42;
           sd1.m_nestedData.m_double = -21.54;
           sd1.m_string = "This is an example for marshalling data.";

           // This is only undefined data.
           Container c(Container::UNDEFINEDDATA, sd1);
           ds->put(1, c);

           Container c2 = ds->get(1);
           DataStoreTestSampleData sd2;
           sd2 = c2.getData<DataStoreTestSampleData>();

           TS_ASSERT(sd1.m_bool == sd2.m_bool);
           TS_ASSERT(sd1.m_int == sd2.m_int);
           TS_ASSERT(sd1.m_string == sd2.m_string);
           TS_ASSERT_DELTA(sd1.m_nestedData.m_double, sd2.m_nestedData.m_double, 1e-4);

           // Clean up.
           delete ds;
           
           // Test existence of files.
// Log file creation disabled in BerkeleyDB libraries.
//           fstream logFile("log.0000000001", ios::in);
//           TS_ASSERT(logFile.good());
//           logFile.close();
//           unlink("log.0000000001");

           fstream dbFile("test.db", ios::in);
           TS_ASSERT(dbFile.good());
           dbFile.close();
           unlink("test.db");
       }
};

#endif /*CORE_DATASTORETESTSUITE_H_*/
