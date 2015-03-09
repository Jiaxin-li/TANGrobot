/**
 * cockpit - Visualization environment
 * Copyright (C) 2012 - 2015 Christian Berger
 * Copyright (C) 2008 - 2011 (as monitor component) Christian Berger, Bernhard Rumpe
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

#include <vector>

#include "core/data/environment/Point3.h"
#include "hesperia/scenegraph/renderer/SceneNodeRenderingConfiguration.h"

#include "plugins/birdseyemap/BirdsEyeMapRenderer.h"

namespace cockpit {
    namespace plugins {
        namespace birdseyemap {

            using namespace std;
            using namespace hesperia::scenegraph::renderer;

            BirdsEyeMapRenderer::BirdsEyeMapRenderer(QPainter *painter, RenderingConfiguration &rc, const uint32_t &pixelPerMeter) :
                AbstractRenderer(),
                m_painter(painter),
                m_renderingConfiguration(rc),
                m_pixelPerMeter(pixelPerMeter) {}

            BirdsEyeMapRenderer::~BirdsEyeMapRenderer() {}

            void BirdsEyeMapRenderer::render(hesperia::scenegraph::primitives::Point *p) {
                if ( (p != NULL) && (m_renderingConfiguration.getSceneNodeRenderingConfiguration(p->getSceneNodeDescriptor()).hasParameter(SceneNodeRenderingConfiguration::ENABLED)) ) {
                    QPen pen;
                    pen.setWidth(p->getWidth()*m_pixelPerMeter/10);
                    pen.setColor(QColor(255*p->getColor().getX(), 255*p->getColor().getY(), 255*p->getColor().getZ()));

                    m_painter->setPen(pen);

                    m_painter->drawPoint(p->getPosition().getX()*m_pixelPerMeter, p->getPosition().getY()*m_pixelPerMeter);
                }
            }

            void BirdsEyeMapRenderer::render(hesperia::scenegraph::primitives::Line *l) {
                if ( (l != NULL) && (m_renderingConfiguration.getSceneNodeRenderingConfiguration(l->getSceneNodeDescriptor()).hasParameter(SceneNodeRenderingConfiguration::ENABLED)) ) {
                    QPen pen;
                    pen.setWidth(l->getWidth()*m_pixelPerMeter/10);
                    pen.setColor(QColor(255*l->getColor().getX(), 255*l->getColor().getY(), 255*l->getColor().getZ()));

                    m_painter->setPen(pen);

                    m_painter->drawLine(l->getA().getX()*m_pixelPerMeter, l->getA().getY()*m_pixelPerMeter,
                                        l->getB().getX()*m_pixelPerMeter, l->getB().getY()*m_pixelPerMeter);
                }
            }

            void BirdsEyeMapRenderer::render(hesperia::scenegraph::primitives::Polygon *p) {
                if ( (p != NULL) && (m_renderingConfiguration.getSceneNodeRenderingConfiguration(p->getSceneNodeDescriptor()).hasParameter(SceneNodeRenderingConfiguration::ENABLED)) ) {
                    QPen pen;
                    pen.setWidth(1*m_pixelPerMeter/10);
                    pen.setColor(QColor(255*p->getColor().getX(), 255*p->getColor().getY(), 255*p->getColor().getZ()));

                    m_painter->setPen(pen);

                    const vector<core::data::environment::Point3>& listOfGroundVertices = p->getListOfGroundVertices();
                    const uint32_t size = listOfGroundVertices.size();

                    for (uint32_t i = 0; i < size - 1; i++) {
                        const core::data::environment::Point3 &p1 = listOfGroundVertices[i];
                        const core::data::environment::Point3 &p2 = listOfGroundVertices[i+1];
                        m_painter->drawLine(p1.getX()*m_pixelPerMeter, p1.getY()*m_pixelPerMeter,
                                            p2.getX()*m_pixelPerMeter, p2.getY()*m_pixelPerMeter);

                    }

                    {
                        const core::data::environment::Point3 &p1 = listOfGroundVertices[size-1];
                        const core::data::environment::Point3 &p2 = listOfGroundVertices[0];

                        // Close polyon.
                        m_painter->drawLine(p1.getX()*m_pixelPerMeter, p1.getY()*m_pixelPerMeter,
                                            p2.getX()*m_pixelPerMeter, p2.getY()*m_pixelPerMeter);
                    }

                }
            }

        }
    }
} // plugins::birdseyemap

