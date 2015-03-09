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
#include "hesperia/data/situation/BoundingBox.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;
            using namespace core::base;

            BoundingBox::BoundingBox() :
                    m_upperLeft(),
                    m_upperRight(),
                    m_lowerRight(),
                    m_lowerLeft() {}

            BoundingBox::BoundingBox(const BoundingBox &obj) :
                    SerializableData(),
                    m_upperLeft(obj.m_upperLeft),
                    m_upperRight(obj.m_upperRight),
                    m_lowerRight(obj.m_lowerRight),
                    m_lowerLeft(obj.m_lowerLeft) {}

            BoundingBox::~BoundingBox() {}

            BoundingBox& BoundingBox::operator=(const BoundingBox &obj) {
                setUpperLeft(obj.getUpperLeft());
                setUpperRight(obj.getUpperRight());
                setLowerRight(obj.getLowerRight());
                setLowerLeft(obj.getLowerLeft());
                return (*this);
            }

            void BoundingBox::accept(SituationVisitor &visitor) {
                visitor.visit(*this);
            }

            const Vertex3& BoundingBox::getUpperLeft() const {
                return m_upperLeft;
            }

            void BoundingBox::setUpperLeft(const Vertex3 &ul) {
                m_upperLeft = ul;
            }

            const Vertex3& BoundingBox::getUpperRight() const {
                return m_upperRight;
            }

            void BoundingBox::setUpperRight(const Vertex3 &ur) {
                m_upperRight = ur;
            }

            const Vertex3& BoundingBox::getLowerRight() const {
                return m_lowerRight;
            }

            void BoundingBox::setLowerRight(const Vertex3 &lr) {
                m_lowerRight = lr;
            }

            const Vertex3& BoundingBox::getLowerLeft() const {
                return m_lowerLeft;
            }

            void BoundingBox::setLowerLeft(const Vertex3 &ll) {
                m_lowerLeft = ll;
            }

            const string BoundingBox::toString() const {
                stringstream s;
                s << "Upper/left: " << m_upperLeft.toString() << ", Upper/right: " << m_upperRight.toString() << ", Lower/right: " << m_lowerRight.toString() << ", Lower/left: " << m_lowerLeft.toString();
                return s.str();
            }

            ostream& BoundingBox::operator<<(ostream &out) const {
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('u', 'p', 'l', 'e', 'f', 't') >::RESULT,
                        m_upperLeft);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('u', 'p', 'r', 'i', 'g', 'h', 't') >::RESULT,
                        m_upperRight);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('l', 'o', 'r', 'i', 'g', 'h', 't') >::RESULT,
                        m_lowerRight);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('l', 'o', 'l', 'e', 'f', 't') >::RESULT,
                        m_lowerLeft);

                return out;
            }

            istream& BoundingBox::operator>>(istream &in) {
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('u', 'p', 'l', 'e', 'f', 't') >::RESULT,
                       m_upperLeft);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('u', 'p', 'r', 'i', 'g', 'h', 't') >::RESULT,
                       m_upperRight);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL7('l', 'o', 'r', 'i', 'g', 'h', 't') >::RESULT,
                       m_lowerRight);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL6('l', 'o', 'l', 'e', 'f', 't') >::RESULT,
                       m_lowerLeft);

                return in;
            }

        }
    }
} // hesperia::data::situation
