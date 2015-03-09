/**
 * hesperia - Simulation environment
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

#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"
#include "hesperia/data/situation/Situation.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;
            using namespace core::base;

            Situation::Situation() :
                    m_header(),
                    m_listOfObjects() {}

            Situation::Situation(const Situation &obj) :
                    SerializableData(),
                    m_header(obj.m_header),
                    m_listOfObjects() {
                // Create deep copy.
                deepCopy(obj);
            }

            Situation::~Situation() {}

            Situation& Situation::operator=(const Situation &obj) {
                setHeader(obj.getHeader());

                // Create deep copy.
                deepCopy(obj);

                return (*this);
            }

            void Situation::deepCopy(const Situation &obj) {
                m_listOfObjects.clear();
                const vector<Object>& list = obj.getListOfObjects();
                vector<Object>::const_iterator it = list.begin();
                while (it != list.end()) {
                    addObject((*it++));
                }
            }

            void Situation::accept(SituationVisitor &visitor) {
                visitor.visit(*this);

                m_header.accept(visitor);

                vector<Object>::iterator it = m_listOfObjects.begin();
                while (it != m_listOfObjects.end()) {
                    (*it++).accept(visitor);
                }
            }

            const Header& Situation::getHeader() const {
                return m_header;
            }

            void Situation::setHeader(const Header &h) {
                m_header = h;
            }

            const vector<Object>& Situation::getListOfObjects() const {
                return m_listOfObjects;
            }

            void Situation::addObject(const Object &o) {
                m_listOfObjects.push_back(o);
            }

            const string Situation::toString() const {
                return m_header.toString();
            }

            ostream& Situation::operator<<(ostream &out) const {
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('h', 'e', 'a', 'd', 'e', 'r') >::RESULT,
                        m_header);

                uint32_t numberOfObjects = static_cast<uint32_t>(m_listOfObjects.size());
                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('n', 'u', 'm', 'o', 'b', 's') >::RESULT,
                        numberOfObjects);

                // Write roads to stringstream.
                stringstream sstr;
                for (uint32_t i = 0; i < numberOfObjects; i++) {
                    // Write data to stringstream.
                    sstr << m_listOfObjects.at(i);
                }

                // Write objects.
                if (numberOfObjects > 0) {
                    s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('o', 'b', 'j', 'e', 'c', 't', 's') >::RESULT,
                            sstr.str());
                }

                return out;
            }

            istream& Situation::operator>>(istream &in) {
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('h', 'e', 'a', 'd', 'e', 'r') >::RESULT,
                       m_header);

                uint32_t numberOfObjects = 0;
                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('n', 'u', 'm', 'o', 'b', 's') >::RESULT,
                       numberOfObjects);

                if (numberOfObjects > 0) {
                    string str;
                    // Read layers into stringstream.
                    d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('o', 'b', 'j', 'e', 'c', 't', 's') >::RESULT,
                           str);

                    stringstream sstr(str);

                    // Read layer from stringstream.
                    for (uint32_t i = 0; i < numberOfObjects; i++) {
                        Object o;
                        sstr >> o;
                        addObject(o);
                    }
                }

                return in;
            }

        }
    }
} // hesperia::data::situation
