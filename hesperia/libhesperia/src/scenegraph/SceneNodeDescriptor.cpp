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

#include "hesperia/scenegraph/SceneNodeDescriptor.h"

namespace hesperia {
    namespace scenegraph {

        using namespace std;

        SceneNodeDescriptor::SceneNodeDescriptor() :
            m_name() {}

        SceneNodeDescriptor::SceneNodeDescriptor(const string &name) :
            m_name(name) {}

        SceneNodeDescriptor::~SceneNodeDescriptor() {}

        SceneNodeDescriptor::SceneNodeDescriptor(const SceneNodeDescriptor &obj) :
            m_name(obj.m_name) {}

        SceneNodeDescriptor& SceneNodeDescriptor::operator=(const SceneNodeDescriptor &obj) {
            m_name = obj.m_name;

            return (*this);
        }

        const string SceneNodeDescriptor::getName() const {
            return m_name;
        }
    }
} // hesperia::scenegraph
