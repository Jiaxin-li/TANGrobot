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

#include "hesperia/data/sensor/ContouredObject.h"

namespace hesperia {
    namespace data {
        namespace sensor {

            using namespace std;
            using namespace core::base;
            using namespace core::data::environment;

            ContouredObject::ContouredObject() :
                    PointShapedObject(), m_contour() {}

            ContouredObject::ContouredObject(const Point3 &position, const Point3 &rotation,
                                             const Point3 &velocity, const Point3 &acceleration) :
                    PointShapedObject(position, rotation, velocity, acceleration), m_contour() {}

            ContouredObject::ContouredObject(const ContouredObject &obj) :
                    PointShapedObject(obj),
                    m_contour(obj.getContour()) {}

            ContouredObject::~ContouredObject() {}

            ContouredObject& ContouredObject::operator=(const ContouredObject &obj) {
                PointShapedObject::operator=(obj);
                setContour(obj.getContour());

                return (*this);
            }

            const vector<Point3> ContouredObject::getContour() const {
                return m_contour;
            }

            void ContouredObject::setContour(const vector<Point3> &contour) {
                m_contour = contour;
            }

            const string ContouredObject::toString() const {
                stringstream s;
                s << PointShapedObject::toString() << " consisting of " << static_cast<uint32_t>(getContour().size()) << " contour points.";
                return s.str();
            }

            void ContouredObject::reset() {
                setPosition(Point3(0, 0, 0));
                setRotation(Point3(0, 0, 0));
                setVelocity(Point3(0, 0, 0));
                setAcceleration(Point3(0, 0, 0));
                m_contour.clear();
            }

            ostream& ContouredObject::operator<<(ostream &out) const {
                // Serializer super class.
                PointShapedObject::operator<<(out);

                // Serialize this class.
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                // Write contour.
                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('c', 'o', 'n', 't', 's', 'i', 'z', 'e') >::RESULT,
                        static_cast<uint32_t>(m_contour.size()));

                // Coordinates.
                stringstream sstr;
                vector<Point3>::const_iterator it = m_contour.begin();
                while (it != m_contour.end()) {
                    sstr << (*it++);
                }
                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('c', 'o', 'n', 't', 'o', 'u', 'r') >::RESULT,
                        sstr.str());

                return out;
            }

            istream& ContouredObject::operator>>(istream &in) {
                // Deserializer super class.
                PointShapedObject::operator>>(in);

                // Deserialize this class.
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                // Read contour.
                uint32_t numberOfContourPoints = 0;
                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL8('c', 'o', 'n', 't', 's', 'i', 'z', 'e') >::RESULT,
                       numberOfContourPoints);
                m_contour.clear();

                string contour;
                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('c', 'o', 'n', 't', 'o', 'u', 'r') >::RESULT,
                       contour);
                stringstream sstr;
                sstr.str(contour);
                while (numberOfContourPoints > 0) {
                    Point3 cc;
                    sstr >> cc;
                    m_contour.push_back(cc);
                    numberOfContourPoints--;
                }

                return in;
            }

        }
    }
} // hesperia::data::sensor
