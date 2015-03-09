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

#include "hesperia/threeD/RenderingConfiguration.h"

namespace hesperia {
    namespace threeD {

        RenderingConfiguration::RenderingConfiguration() :
            m_drawTextures(true),
            m_nodesRenderingConfiguration() {}

        RenderingConfiguration::RenderingConfiguration(const RenderingConfiguration &obj) :
        	m_drawTextures(obj.m_drawTextures),
        	m_nodesRenderingConfiguration(obj.m_nodesRenderingConfiguration) {}

        RenderingConfiguration::~RenderingConfiguration() {}

        RenderingConfiguration& RenderingConfiguration::operator=(const RenderingConfiguration &obj) {
        	m_drawTextures = obj.m_drawTextures;
        	m_nodesRenderingConfiguration = obj.m_nodesRenderingConfiguration;

        	return (*this);
        }

        bool RenderingConfiguration::hasDrawTextures() const {
            return m_drawTextures;
        }

        void RenderingConfiguration::setDrawTextures(const bool &drawTextures) {
            m_drawTextures = drawTextures;
        }

        const NodeRenderingConfiguration& RenderingConfiguration::getNodeRenderingConfiguration(const NodeDescriptor &nd) {
            return m_nodesRenderingConfiguration[nd];
        }

        void RenderingConfiguration::setNodeRenderingConfiguration(const NodeDescriptor &nd, const NodeRenderingConfiguration &nrc) {
            m_nodesRenderingConfiguration[nd] = nrc;
        }

    }
} // hesperia::threeD
