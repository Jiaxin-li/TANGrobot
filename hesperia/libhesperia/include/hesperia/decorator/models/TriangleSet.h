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

#ifndef HESPERIA_CORE_DECORATOR_MODELS_TRIANGLESET_H_
#define HESPERIA_CORE_DECORATOR_MODELS_TRIANGLESET_H_

#include <vector>

// native.h must be included as first header file for definition of _WIN32_WINNT.
#include "core/native.h"

#include "core/data/environment/Point3.h"
#include "hesperia/decorator/models/Material.h"
#include "hesperia/decorator/models/Triangle.h"

namespace hesperia {
    namespace decorator {
        namespace models {

            using namespace std;

            /**
             * This class represents a set of triangles.
             */
            class OPENDAVINCI_API TriangleSet {
                public:
                    TriangleSet();

                    /**
                     * Copy constructor.
                     *
                     * @param obj Reference to an object of this class.
                     */
                    TriangleSet(const TriangleSet &obj);

                    /**
                     * Assignment operator.
                     *
                     * @param obj Reference to an object of this class.
                     * @return Reference to this instance.
                     */
                    TriangleSet& operator=(const TriangleSet &obj);

                    virtual ~TriangleSet();

                    /**
                     * This method adds a new triangle.
                     *
                     * @param triangle Triangle to be added.
                     */
                    void addTriangle(const Triangle &triangle);

                    /**
                     * This method sets the material for this triangle set.
                     *
                     * @param material Material for this triangle set.
                     */
                    void setMaterial(const Material &material);

                    Material m_material;
                    vector<core::data::environment::Point3> m_vertices;
                    vector<core::data::environment::Point3> m_normals;
                    vector<core::data::environment::Point3> m_textureCoordinates;
            };

        }
    }
} // hesperia::decorator::models

#endif /*HESPERIA_CORE_DECORATOR_MODELS_TRIANGLESET_H_*/
