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

#ifndef HESPERIA_CORE_DATA_SENSOR_CONTOUREDOBJECTS_H_
#define HESPERIA_CORE_DATA_SENSOR_CONTOUREDOBJECTS_H_

#include <vector>

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/native.h"
#include "core/data/SerializableData.h"

#include "hesperia/data/sensor/ContouredObject.h"

namespace hesperia {
    namespace data {
        namespace sensor {

            using namespace std;

            /**
             * This class contains all data from one specific sensor.
             */
            class OPENDAVINCI_API ContouredObjects : public core::data::SerializableData {
                public:
                    enum COLOR {
                        RED,
                        GREEN,
                        BLUE,
                        YELLOW
                    };

                public:
                    ContouredObjects();

                    virtual ~ContouredObjects();

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    ContouredObjects(const ContouredObjects &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    ContouredObjects& operator=(const ContouredObjects &obj);

                    /**
                     * This method returns all contoured objects.
                     *
                     * @return Set of contoured objects.
                     */
                    const vector<ContouredObject> getContouredObjects() const;

                    /**
                     * This method adds a contoured object.
                     *
                     * @param contouredObject New contoured object to be added.
                     */
                    void add(const ContouredObject &contouredObject);

                    /**
                     * This method returns the color.
                     *
                     * @return Color.
                     */
                    enum COLOR getColor() const;

                    /**
                     * This method sets the color.
                     *
                     * @param color The new color.
                     */
                    void setColor(const enum COLOR &color);

                    /**
                     * This method resets all current contoured objects.
                     */
                    void reset();

                    virtual ostream& operator<<(ostream &out) const;
                    virtual istream& operator>>(istream &in);

                    virtual const string toString() const;

                private:
                    vector<ContouredObject> m_contouredObjects;
                    enum COLOR m_color;
            };

        }
    }
} // hesperia::data::sensor

#endif /*HESPERIA_CORE_DATA_SENSOR_CONTOUREDOBJECTS_H_*/
