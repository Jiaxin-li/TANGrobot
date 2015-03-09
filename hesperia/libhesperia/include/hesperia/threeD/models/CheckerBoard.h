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

#ifndef HESPERIA_CORE_THREED_MODELS_CHECKERBOARD_H_
#define HESPERIA_CORE_THREED_MODELS_CHECKERBOARD_H_

#include <vector>

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/native.h"

#include "hesperia/threeD/Node.h"
#include "core/data/environment/Point3.h"

namespace hesperia {
    namespace threeD {
        namespace models {

            using namespace std;

            /**
             * This class represents a checker board.
             */
            class OPENDAVINCI_API CheckerBoard : public Node {
                public:
                    /**
                     * Constructor for a default checker board.
                     *
                     * @param nodeDescriptor Descriptor for this node.
                     */
                    CheckerBoard(const NodeDescriptor &nodeDescriptor);

                    /**
                     * Constructor.
                     *
                     * @param nodeDescriptor Descriptor for this node.
                     * @param positionA Position A of the checker board.
                     * @param positionB Position B of the checker board.
                     * @param height Checker board's height.
                     */
                    CheckerBoard(const NodeDescriptor &nodeDescriptor, const core::data::environment::Point3 &positionA, const core::data::environment::Point3 &positionB, const float &height);

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    CheckerBoard(const CheckerBoard &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    CheckerBoard& operator=(const CheckerBoard &obj);

                    virtual ~CheckerBoard();

                    virtual void render(RenderingConfiguration &renderingConfiguration);

                private:
                    core::data::environment::Point3 m_positionA;
                    core::data::environment::Point3 m_positionB;
                    float m_height;
            };

        }
    }
} // hesperia::threeD::models

#endif /*HESPERIA_CORE_THREED_MODELS_CHECKERBOARD_H_*/
