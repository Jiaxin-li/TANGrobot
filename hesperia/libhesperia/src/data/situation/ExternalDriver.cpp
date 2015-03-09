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
#include "hesperia/data/situation/ExternalDriver.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;
            using namespace core::base;

            ExternalDriver::ExternalDriver() :
                    Behavior() {
                setType(Behavior::EXTERNALDRIVER);
            }

            ExternalDriver::ExternalDriver(const ExternalDriver &obj) :
                    Behavior(obj) {}

            ExternalDriver::~ExternalDriver() {}

            ExternalDriver& ExternalDriver::operator=(const ExternalDriver &obj) {
                Behavior::operator=(obj);
                return (*this);
            }

            void ExternalDriver::accept(SituationVisitor &visitor) {
                visitor.visit(*this);
            }

            const string ExternalDriver::toString() const {
                stringstream s;
                s << getType();
                return s.str();
            }

            ostream& ExternalDriver::operator<<(ostream &out) const {
                // Serializer super class.
                Behavior::operator<<(out);

//
//                SerializationFactory sf;
//
//                Serializer &s = sf.getSerializer(out);
//
//                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL4('t', 'y', 'p', 'e') >::RESULT,
//                        static_cast<uint32_t>(m_type));

                return out;
            }

            istream& ExternalDriver::operator>>(istream &in) {
                // Deserializer super class.
                Behavior::operator>>(in);

//                SerializationFactory sf;
//
//                Deserializer &d = sf.getDeserializer(in);
//
//                uint32_t type = 0;
//                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL4('t', 'y', 'p', 'e') >::RESULT,
//                       type);
//
//                m_type = static_cast<enum ExternalDriver::BEHAVIORTYPE>(type);

                return in;
            }

        }
    }
} // hesperia::data::situation
