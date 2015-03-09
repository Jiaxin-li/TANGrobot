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

#include "hesperia/data/planning/Route.h"

namespace hesperia {
    namespace data {
        namespace planning {

            using namespace std;
            using namespace core::base;
            using namespace core::data::environment;

            Route::Route() :
                m_listOfVertices() {}

            Route::Route(const vector<Point3> &vertices) :
                m_listOfVertices(vertices) {}

            Route::Route(const Route &obj) :
                m_listOfVertices(obj.m_listOfVertices) {}

            Route::~Route() {}

            Route& Route::operator=(const Route &obj) {
                m_listOfVertices = obj.m_listOfVertices;

                return (*this);
            }

            void Route::add(const Point3 &p) {
                m_listOfVertices.push_back(p);
            }

            uint32_t Route::getSize() const {
                return m_listOfVertices.size();
            }

            vector<Point3> Route::getListOfPoints() const {
                return m_listOfVertices;
            }

            double Route::getLength() const {
                double length = 0;

                if (m_listOfVertices.size() > 1) {
                    for (uint32_t i = 0; i < m_listOfVertices.size()-1; i++) {
                        length += m_listOfVertices.at(i).getDistanceTo(m_listOfVertices.at(i+1));
                    }
                }

                return length;
            }

            const string Route::toString() const {
                stringstream sstr;
                sstr << "Route:";
                for (uint32_t i = 0; i < m_listOfVertices.size(); i++) {
                    sstr << " " << m_listOfVertices.at(i).toString();
                }
                return sstr.str();
            }

            ostream& Route::operator<<(ostream &out) const {
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

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

            istream& Route::operator>>(istream &in) {
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                // Clean up.
                m_listOfVertices.clear();

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
                        Point3 p;
                        sstr >> p;
                        add(p);
                    }
                }

                return in;
            }

        }
    }
} // hesperia::data::planning
