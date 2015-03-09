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

#ifndef HESPERIA_CORE_DATA_SITUATION_STARTTYPE_H_
#define HESPERIA_CORE_DATA_SITUATION_STARTTYPE_H_

#include <string>

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/native.h"
#include "core/data/SerializableData.h"
#include "hesperia/data/situation/SituationNode.h"
#include "hesperia/data/situation/SituationVisitor.h"

namespace hesperia {
    namespace data {
        namespace situation {

            using namespace std;

            /**
             * This class represents an abstract start type (IMMEDIATELY, ONMOVING, ONENTERINGPOLYGON).
             */
            class OPENDAVINCI_API StartType : public core::data::SerializableData, public SituationNode {
                public:
                    enum STARTTYPE {
                        UNDEFINED,
                        IMMEDIATELY,
                        ONMOVING,
                        ONENTERINGPOLYGON
                    };

                protected:
                    StartType();

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    StartType(const StartType &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    StartType& operator=(const StartType &obj);

                public:
                    virtual ~StartType();

                    virtual void accept(SituationVisitor &visitor) = 0;

                    /**
                     * This method returns the type of the starttype.
                     *
                     * @return Type of starttype.
                     */
                    enum STARTTYPE getType() const;

                    /**
                     * This method sets the starttype's type.
                     *
                     * @param type Type of the starttype.
                     */
                    void setType(const enum StartType::STARTTYPE &type);

                    virtual ostream& operator<<(ostream &out) const;
                    virtual istream& operator>>(istream &in);

                    virtual const string toString() const;

                private:
                    enum STARTTYPE m_type;
            };

        }
    }
} // core::data::situation

#endif /*HESPERIA_CORE_DATA_SITUATION_STARTTYPE_H_*/
