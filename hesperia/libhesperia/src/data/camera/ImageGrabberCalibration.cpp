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

#include "hesperia/data/camera/ImageGrabberCalibration.h"

namespace hesperia {
    namespace data {
        namespace camera {

            using namespace std;
            using namespace core::data;
            using namespace core::base;

            ImageGrabberCalibration::ImageGrabberCalibration() :
                    m_extrinsicParameters(),
                    m_intrinsicParameters() {}

            ImageGrabberCalibration::ImageGrabberCalibration(const ImageGrabberCalibration &obj) :
                    m_extrinsicParameters(obj.getExtrinsicParameters()),
                    m_intrinsicParameters(obj.getIntrinsicParameters()) {}

            ImageGrabberCalibration::~ImageGrabberCalibration() {}

            ImageGrabberCalibration& ImageGrabberCalibration::operator=(const ImageGrabberCalibration &obj) {
                setExtrinsicParameters(obj.getExtrinsicParameters());
                setIntrinsicParameters(obj.getIntrinsicParameters());

                return (*this);
            }

            const ExtrinsicParameters ImageGrabberCalibration::getExtrinsicParameters() const {
                return m_extrinsicParameters;
            }

            void ImageGrabberCalibration::setExtrinsicParameters(const ExtrinsicParameters &extrinsicParameters) {
                m_extrinsicParameters = extrinsicParameters;
            }

            const IntrinsicParameters ImageGrabberCalibration::getIntrinsicParameters() const {
                return m_intrinsicParameters;
            }

            void ImageGrabberCalibration::setIntrinsicParameters(const IntrinsicParameters &intrinsicParameters) {
                m_intrinsicParameters = intrinsicParameters;
            }

            ostream& ImageGrabberCalibration::operator<<(ostream &out) const {
                SerializationFactory sf;

                Serializer &s = sf.getSerializer(out);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('e', 'x', 't') >::RESULT,
                        m_extrinsicParameters);

                s.write(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('i', 'n', 't') >::RESULT,
                        m_intrinsicParameters);

                return out;
            }

            istream& ImageGrabberCalibration::operator>>(istream &in) {
                SerializationFactory sf;

                Deserializer &d = sf.getDeserializer(in);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('e', 'x', 't') >::RESULT,
                       m_extrinsicParameters);

                d.read(CRC32 < OPENDAVINCI_CORE_STRINGLITERAL3('i', 'n', 't') >::RESULT,
                       m_intrinsicParameters);

                return in;
            }

            const string ImageGrabberCalibration::toString() const {
                stringstream sstr;
                sstr << "Extrinsic: " << m_extrinsicParameters.toString() << "/Intrinsic: " << m_intrinsicParameters.toString();
                return sstr.str();
            }
        }
    }
} // hesperia::data::camera
