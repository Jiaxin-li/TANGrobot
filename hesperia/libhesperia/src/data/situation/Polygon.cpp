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

#include <sstream>

#include "core/base/Hash.h"
#include "core/base/Deserializer.h"
#include "core/base/SerializationFactory.h"
#include "core/base/Serializer.h"
#include "hesperia/data/situation/Polygon.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;
            using namespace core::base;

            Polygon::Polygon() :
                    m_height(0),
                    m_color(),
                    m_listOfVertices() {
                Shape::setType(Shape::POLYGON);
            }

            Polygon::Polygon(const Polygon &obj) :
                    Shape(obj),
                    m_height(obj.m_height),
                    m_color(obj.m_color),
                    m_listOfVertices() {
                deepCopy(obj);
            }

            Polygon::~Polygon() {}

            Polygon& Polygon::operator=(const Polygon &obj) {
                Shape::operator=(obj);
                setHeight(obj.getHeight());
                setColor(obj.getColor());
                deepCopy(obj);
                return (*this);
            }

            void Polygon::deepCopy(const Polygon &obj) {
                m_listOfVertices.clear();
                const vector<Vertex3> &listOfVertices = obj.getListOfVertices();
                vector<Vertex3>::const_iterator it = listOfVertices.begin();
                while (it != listOfVertices.end()) {
                    add((*it++));
                }
            }

            void Polygon::accept(SituationVisitor &visitor) {
                visitor.visit(*this);

                vector<Vertex3>::iterator it = m_listOfVertices.begin();
                while (it != m_listOfVertices.end()) {
                    (*it).accept(visitor);
                    it++;
                }
            }

            double Polygon::getHeight() const {
                return m_height;
            }

            void Polygon::setHeight(const double &h) {
                m_height = h;
            }

            const Vertex3& Polygon::getColor() const {
                return m_color;
            }

            void Polygon::setColor(const Vertex3 &c) {
                m_color = c;
            }

            const vector<Vertex3>& Polygon::getListOfVertices() const {
                return m_listOfVertices;
            }

            void Polygon::add(const Vertex3 &v) {
                m_listOfVertices.push_back(v);
            }

            const string Polygon::toString() const {
                stringstream s;
                s << Shape::toString() << ", Height: " << m_height << ", Color: " << m_color.toString();
                return s.str();
            }

            ostream& Polygon::operator<<(ostream &out) const {
                // Serializer super class.
                Shape::operator<<(out);

                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('h', 'e', 'i', 'g', 'h', 't') >::RESULT,
                        m_height);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('c', 'o', 'l', 'o', 'r') >::RESULT,
                        m_color);

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

            istream& Polygon::operator>>(istream &in) {
                // Deserializer super class.
                Shape::operator>>(in);

                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                // Clean up.
                m_listOfVertices.clear();

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('h', 'e', 'i', 'g', 'h', 't') >::RESULT,
                       m_height);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL5('c', 'o', 'l', 'o', 'r') >::RESULT,
                       m_color);

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
