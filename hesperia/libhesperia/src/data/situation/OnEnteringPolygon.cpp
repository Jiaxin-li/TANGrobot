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
#include "hesperia/data/situation/OnEnteringPolygon.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;
            using namespace core::base;

            OnEnteringPolygon::OnEnteringPolygon() :
                    StartType(),
                    m_id(0),
                    m_listOfVertices() {
                setType(StartType::ONENTERINGPOLYGON);
            }

            OnEnteringPolygon::OnEnteringPolygon(const OnEnteringPolygon &obj) :
                    StartType(obj),
                    m_id(obj.getID()),
                    m_listOfVertices() {
                deepCopy(obj);
            }

            OnEnteringPolygon::~OnEnteringPolygon() {}

            OnEnteringPolygon& OnEnteringPolygon::operator=(const OnEnteringPolygon &obj) {
                StartType::operator=(obj);
                setID(obj.getID());
                deepCopy(obj);
                return (*this);
            }

            void OnEnteringPolygon::deepCopy(const OnEnteringPolygon &obj) {
                m_listOfVertices.clear();
                const vector<Vertex3> &listOfVertices = obj.getListOfVertices();
                vector<Vertex3>::const_iterator it = listOfVertices.begin();
                while (it != listOfVertices.end()) {
                    add((*it++));
                }
            }

            void OnEnteringPolygon::accept(SituationVisitor &visitor) {
                visitor.visit(*this);

                vector<Vertex3>::iterator it = m_listOfVertices.begin();
                while (it != m_listOfVertices.end()) {
                    (*it).accept(visitor);
                    it++;
                }
            }

            uint32_t OnEnteringPolygon::getID() const {
                return m_id;
            }

            void OnEnteringPolygon::setID(const uint32_t &id) {
                m_id = id;
            }

            const vector<Vertex3>& OnEnteringPolygon::getListOfVertices() const {
                return m_listOfVertices;
            }

            void OnEnteringPolygon::add(const Vertex3 &v) {
                m_listOfVertices.push_back(v);
            }

            const string OnEnteringPolygon::toString() const {
                stringstream s;
                s << "Type: " << getType() << " on entering ID: " << getID();
                return s.str();
            }

            ostream& OnEnteringPolygon::operator<<(ostream &out) const {
                // Serializer super class.
                StartType::operator<<(out);

                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL2('i', 'd') >::RESULT,
                        getID());

                // Write number of vertices.
                uint32_t numberOfVertices = static_cast<uint32_t>(m_listOfVertices.size());
                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('n', 'u', 'm', 'v', 'e', 'r', 't', 's') >::RESULT,
                        numberOfVertices);

                // Write actual vertices to stringstream.
                stringstream sstr;
                for (uint32_t i = 0; i < numberOfVertices; i++) {
                    sstr << m_listOfVertices.at(i);
                }

                // Write string of vertices.
                if (numberOfVertices > 0) {
                    s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('v', 'e', 'r', 't', 'i', 'c', 'e', 's') >::RESULT,
                            sstr.str());
                }

                return out;
            }

            istream& OnEnteringPolygon::operator>>(istream &in) {
                // Deserializer super class.
                StartType::operator>>(in);

                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL2('i', 'd') >::RESULT,
                       m_id);

                // Read number of vertices.
                uint32_t numberOfVertices = 0;
                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('n', 'u', 'm', 'v', 'e', 'r', 't', 's') >::RESULT,
                       numberOfVertices);

                if (numberOfVertices > 0) {
                    // Read string of vertices.
                    string vertices;
                    d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('v', 'e', 'r', 't', 'i', 'c', 'e', 's') >::RESULT,
                           vertices);

                    stringstream sstr(vertices);

                    // Read actual vertices from stringstream.
                    for (uint32_t i = 0; i < numberOfVertices; i++) {
                        Vertex3 v;
                        sstr >> v;
                        add(v);
                    }
                }

                return in;
            }

        }
    }
} // hesperia::data::situation
