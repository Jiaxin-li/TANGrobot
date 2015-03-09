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

#ifndef HESPERIA_CORE_THREED_MODELS_HEIGHTGRID_H_
#define HESPERIA_CORE_THREED_MODELS_HEIGHTGRID_H_

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/native.h"
#include "core/wrapper/Image.h"

#include "core/data/environment/Point3.h"
#include "hesperia/threeD/Node.h"
#include "hesperia/threeD/NodeDescriptor.h"
#include "hesperia/threeD/TransformGroup.h"


namespace hesperia {
    namespace threeD {
        namespace models {

            using namespace std;

            /**
             * This class represents the actual renderer.
             */
            class OPENDAVINCI_API HeightGridRenderer : public Node {
                public:
                    /**
                     * Constructor.
                     *
                    * @param nodeDesciptor Description for this node.
                     * @param callList Handle for the height grid's call list.
                     */
                    HeightGridRenderer(const NodeDescriptor &nodeDescriptor,
                                       const uint32_t &callList);

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    HeightGridRenderer(const HeightGridRenderer &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    HeightGridRenderer& operator=(const HeightGridRenderer &obj);

                    virtual ~HeightGridRenderer();

                    virtual void render(RenderingConfiguration &renderingConfiguration);

                private:
                    uint32_t m_callList;
            };

            /**
             * This class represents a height grid.
             */
            class OPENDAVINCI_API HeightGrid : public Node {
                public:
                    /**
                     * Constructor.
                     *
                    * @param nodeDesciptor Description for this node.
                     * @param heightImage Image to be used as height image.
                     * @param originPixelXY Origin pixel inside the image.
                     * @param scalingPixelXY Scaling for the pixels.
                     * @param rotationZ Rotation of the image around the Z-axis.
                     * @param ground Value for the ground.
                     * @param min Minimum height.
                     * @param max Minimum height.
                     */
                    HeightGrid(const NodeDescriptor &nodeDescriptor,
                               const core::wrapper::Image *heightImage,
                               const core::data::environment::Point3 &originPixelXY,
                               const core::data::environment::Point3 &scalingPixelXY,
                               const float &rotationZ,
                               const float &ground,
                               const float &min,
                               const float &max);

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    HeightGrid(const HeightGrid &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    HeightGrid& operator=(const HeightGrid &obj);

                    virtual ~HeightGrid();

                    virtual void render(RenderingConfiguration &renderingConfiguration);

                private:
                    const core::wrapper::Image *m_heightImage;
                    core::data::environment::Point3 m_originPixelXY;
                    core::data::environment::Point3 m_scalingPixelXY;
                    float m_rotationZ;
                    float m_ground;
                    float m_min;
                    float m_max;

                    uint32_t m_callList;
                    TransformGroup *m_heightImageNode;
                    HeightGridRenderer *m_heightImageRenderer;

                    /**
                     * This method initializes the height image.
                     */
                    void init();
            };

        }
    }
} // hesperia::threeD::models

#endif /*HESPERIA_CORE_THREED_MODELS_HEIGHTGRID_H_*/
