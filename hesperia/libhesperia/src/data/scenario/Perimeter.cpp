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

#include "core/macros.h"
#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"
#include "hesperia/data/scenario/Perimeter.h"

namespace hesperia {
    namespace data {
        namespace scenario {

            using namespace std;
            using namespace core::base;
            using namespace scenario;

            Perimeter::Perimeter() :
                    m_listOfIdentifiableVertices() {}

            Perimeter::Perimeter(const Perimeter &obj) :
                    SerializableData(),
                    m_listOfIdentifiableVertices() {
                // Create deep copy.
                deepCopy(obj);
            }

            Perimeter::~Perimeter() {}

            Perimeter& Perimeter::operator=(const Perimeter &obj) {
                // Create deep copy.
                deepCopy(obj);

                return (*this);
            }

            void Perimeter::deepCopy(const Perimeter &obj) {
                m_listOfIdentifiableVertices.clear();
                const vector<IDVertex3>& list = obj.getListOfIdentifiableVertices();
                vector<IDVertex3>::const_iterator it = list.begin();
                while (it != list.end()) {
                    addIdentifiableVertex((*it++));
                }
            }

            void Perimeter::accept(ScenarioVisitor &visitor) {
                visitor.visit(*this);

                vector<IDVertex3>::iterator it = m_listOfIdentifiableVertices.begin();
                while (it != m_listOfIdentifiableVertices.end()) {
                    (*it++).accept(visitor);
                }
            }

            const vector<IDVertex3>& Perimeter::getListOfIdentifiableVertices() const {
                return m_listOfIdentifiableVertices;
            }

            void Perimeter::addIdentifiableVertex(const IDVertex3 &idV) {
                m_listOfIdentifiableVertices.push_back(idV);
            }

            const string Perimeter::toString() const {
                return "";
            }

            ostream& Perimeter::operator<<(ostream &out) const {
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                uint32_t numberOfIDVertices = static_cast<uint32_t>(m_listOfIdentifiableVertices.size());
                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('n', 'u', 'm', 'i', 'd', 'v', 'e', 'x') >::RESULT,
                        numberOfIDVertices);

                // Write lanes to stringstream.
                stringstream sstr;
                for (uint32_t i = 0; i < numberOfIDVertices; i++) {
                    // Write data to stringstream.
                    sstr << m_listOfIdentifiableVertices.at(i);
                }

                // Write lanes.
                if (numberOfIDVertices > 0) {
                    s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('i', 'd', 'v', 'e', 'x') >::RESULT,
                            sstr.str());
                }

                return out;
            }

            istream& Perimeter::operator>>(istream &in) {
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                // Clean up.
                m_listOfIdentifiableVertices.clear();

                uint32_t numberOfIDVertices = 0;
                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('n', 'u', 'm', 'i', 'd', 'v', 'e', 'x') >::RESULT,
                       numberOfIDVertices);

                if (numberOfIDVertices > 0) {
                    string str;
                    // Read lanes into stringstream.
                    d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('i', 'd', 'v', 'e', 'x') >::RESULT,
                           str);

                    stringstream sstr(str);

                    // Read lanes from stringstream.
                    for (uint32_t i = 0; i < numberOfIDVertices; i++) {
                        IDVertex3 idV;
                        sstr >> idV;
                        addIdentifiableVertex(idV);
                    }
                }

                return in;
            }

        }
    }
} // hesperia::data::scenario
