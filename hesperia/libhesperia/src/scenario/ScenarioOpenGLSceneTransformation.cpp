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

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "core/data/Constants.h"
#include "core/data/environment/Point3.h"
#include "hesperia/data/scenario/IDVertex3.h"
#include "hesperia/data/scenario/Arc.h"
#include "hesperia/data/scenario/Clothoid.h"
#include "hesperia/data/scenario/LaneAttribute.h"
#include "hesperia/data/scenario/LaneModel.h"
#include "hesperia/data/scenario/Layer.h"
#include "hesperia/data/scenario/Polygon.h"
#include "hesperia/data/scenario/Shape.h"
#include "hesperia/data/scenario/TrafficSign.h"
#include "hesperia/data/scenario/IDVertex3.h"
#include "hesperia/data/scenario/Vertex3.h"
#include "hesperia/data/scenario/Zone.h"
#include "hesperia/data/scenario/Perimeter.h"
#include "hesperia/scenario/ScenarioOpenGLSceneTransformation.h"
#include "hesperia/threeD/NodeDescriptor.h"
#include "hesperia/threeD/models/Line.h"
#include "hesperia/threeD/models/Point.h"
#include "hesperia/threeD/models/Polygon.h"

namespace hesperia {
    namespace scenario {

        using namespace std;
        using namespace core::data;
        using namespace core::data::environment;
        using namespace data::scenario;

        ScenarioOpenGLSceneTransformation::ScenarioOpenGLSceneTransformation() :
                m_root(NULL),
                m_renderLaneConnectors(true) {
            m_root = new threeD::TransformGroup();
        }

        ScenarioOpenGLSceneTransformation::ScenarioOpenGLSceneTransformation(const bool &renderLaneConnectors) :
                m_root(NULL),
                m_renderLaneConnectors(renderLaneConnectors) {
            m_root = new threeD::TransformGroup();
        }

        ScenarioOpenGLSceneTransformation::~ScenarioOpenGLSceneTransformation() {}

        threeD::TransformGroup* ScenarioOpenGLSceneTransformation::getRoot() const {
            return m_root;
        }

        void ScenarioOpenGLSceneTransformation::transform(Surroundings &surroundings, threeD::TransformGroup &tg) {
            vector<Shape*> listOfShapes = surroundings.getListOfShapes();
            vector<Shape*>::const_iterator it = listOfShapes.begin();
            while (it != listOfShapes.end()) {
                Shape *s = (*it++);
                if ( (s != NULL) && (s->getType() == Shape::POLYGON) ) {
                    Polygon *p = dynamic_cast<Polygon*>(s);
                    if (p != NULL) {
                        vector<Point3> vertices;

                        // Convert vertices.
                        const vector<Vertex3> &originalVertices = p->getListOfVertices();
                        vector<Vertex3>::const_iterator jt = originalVertices.begin();
                        while (jt != originalVertices.end()) {
                            Vertex3 v3 = (*jt++);
                            vertices.push_back(v3);
                        }

                        Point3 color(p->getColor());
                        tg.addChild(new threeD::models::Polygon(threeD::NodeDescriptor(p->getName()), vertices, color, static_cast<float>(p->getHeight())));
                    }
                }
            }
        }

        void ScenarioOpenGLSceneTransformation::transform(const Layer &layer, const Road &road, const Lane &lane, const PointModel &pointModel, threeD::TransformGroup &layerTG) {
            const vector<IDVertex3> &listOfVertices = pointModel.getListOfIdentifiableVertices();
            const uint32_t SIZE = listOfVertices.size();
            if (SIZE > 1) {
                for (uint32_t i = 0; i < (SIZE-1); i++) {
                    // Get to adjacent vertices, determine direction, construct orthogonal direction
                    Point3 ptA = listOfVertices.at(i);
                    ptA.setZ(layer.getHeight());

                    Point3 ptB = listOfVertices.at(i+1);
                    ptB.setZ(layer.getHeight());

                    stringstream namePtA;
                    namePtA << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << listOfVertices.at(i).getID();

                    stringstream namePtB;
                    namePtB << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << listOfVertices.at(i+1).getID();

                    if (m_renderLaneConnectors) {
                        layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtA.str()), ptA, Point3(1, 0, 0), 5));
                        layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtB.str()), ptB, Point3(1, 0, 0), 5));
                    }
/*
                    Point3 colorSkeleton(0.05, 0.05, 0.05);
                    stringstream nameLane;
                    nameLane << "Lane: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(nameLane.str()), ptA, ptB, colorSkeleton, 1));
*/
                    const double halfWidth = pointModel.getLaneAttribute().getWidth() / 2.0;
                    if (halfWidth > 1) {
                        Point3 orthonormalDirection = (ptB - ptA);
                        orthonormalDirection.normalizeXY();
                        orthonormalDirection.setZ(0);
                        orthonormalDirection.rotateZ(Constants::PI/2.0);

                        Point3 colorLeftMarking;
                        switch (pointModel.getLaneAttribute().getLeftLaneMarking()) {
                            case LaneAttribute::SOLID_YELLOW:
                            case LaneAttribute::DOUBLE_YELLOW:
                                colorLeftMarking = Point3(1, 1, 0);
                            break;

                            case LaneAttribute::BROKEN_WHITE:
                            case LaneAttribute::SOLID_WHITE:
                                colorLeftMarking = Point3(1, 1, 1);
                            break;

                            case LaneAttribute::UNDEFINED:
                            case LaneAttribute::CROSSWALK:
                                colorLeftMarking = Point3(0, 0, 0);
                            break;
                        }
                        Point3 leftLanePtA = ptA + (orthonormalDirection * halfWidth);
                        Point3 leftLanePtB = ptB + (orthonormalDirection * halfWidth);

                        stringstream leftLaneMarkingName;
                        leftLaneMarkingName << "Left lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                        layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(leftLaneMarkingName.str()), leftLanePtA, leftLanePtB, colorLeftMarking, 5));

                        Point3 colorRightMarking;
                        switch (pointModel.getLaneAttribute().getRightLaneMarking()) {
                            case LaneAttribute::SOLID_YELLOW:
                            case LaneAttribute::DOUBLE_YELLOW:
                                colorRightMarking = Point3(1, 1, 0);
                            break;

                            case LaneAttribute::BROKEN_WHITE:
                            case LaneAttribute::SOLID_WHITE:
                                colorRightMarking = Point3(1, 1, 1);
                            break;

                            case LaneAttribute::UNDEFINED:
                            case LaneAttribute::CROSSWALK:
                                colorRightMarking = Point3(0, 0, 0);
                            break;
                        }
                        Point3 rightLanePtA = ptA - (orthonormalDirection * halfWidth);
                        Point3 rightLanePtB = ptB - (orthonormalDirection * halfWidth);

                        stringstream rightLaneMarkingName;
                        rightLaneMarkingName << "Right lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                        layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(rightLaneMarkingName.str()), rightLanePtA, rightLanePtB, colorRightMarking, 5));
                    }
                }
            }
        }

        void ScenarioOpenGLSceneTransformation::transform(const Layer &layer, const Road &road, const Lane &lane, const StraightLine &straightLine, threeD::TransformGroup &layerTG) {
            // Get to adjacent vertices, determine direction, construct orthogonal direction
            Point3 ptA = straightLine.getStart();
            ptA.setZ(layer.getHeight());

            Point3 ptB = straightLine.getEnd();
            ptB.setZ(layer.getHeight());

            stringstream namePtA;
            namePtA << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << straightLine.getStart().getID();

            stringstream namePtB;
            namePtB << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << straightLine.getEnd().getID();

            if (m_renderLaneConnectors) {
                layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtA.str()), ptA, Point3(1, 0, 0), 5));
                layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtB.str()), ptB, Point3(1, 0, 0), 5));
            }
/*
            Point3 colorSkeleton(0.01, 0.01, 0.01);
            stringstream nameLane;
            nameLane << "Lane: " << layer.getID() << "." << road.getID() << "." << lane.getID();
            layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(nameLane.str()), ptA, ptB, colorSkeleton, 1));
*/
            bool brokenLeft = true;
            bool brokenRight = true;
            bool showLeft = true;
            bool showRight = true;
            const double halfWidth = straightLine.getLaneAttribute().getWidth() / 2.0;
            if (halfWidth > 1) {
                Point3 direction = (ptB - ptA);
                const double length = direction.lengthXY();

                direction.normalizeXY();
                direction.setZ(0);

                Point3 orthonormalDirection = direction;
                orthonormalDirection.rotateZ(Constants::PI/2.0);

                const double lanePart = 2.0;
                const uint32_t steps = (uint32_t) round(length/lanePart);
                Point3 leftLanePtAOld = ptA + (orthonormalDirection * halfWidth);
                Point3 rightLanePtAOld = ptA - (orthonormalDirection * halfWidth);

                Point3 colorLeftMarking;
                Point3 colorRightMarking;

                for(uint32_t i = 0; i < steps - 1; i++) {
                    switch (straightLine.getLaneAttribute().getLeftLaneMarking()) {
                        case LaneAttribute::SOLID_YELLOW:
                        case LaneAttribute::DOUBLE_YELLOW:
                            colorLeftMarking = Point3(1, 1, 0);
                        break;

                        case LaneAttribute::BROKEN_WHITE:
                            colorLeftMarking = Point3(1, 1, 1);
                            brokenLeft = !brokenLeft;
                        break;

                        case LaneAttribute::SOLID_WHITE:
                            colorLeftMarking = Point3(1, 1, 1);
                        break;

                        case LaneAttribute::UNDEFINED:
                            showLeft = false;
                        break;

                        case LaneAttribute::CROSSWALK:
                            colorLeftMarking = Point3(0, 0, 0);
                        break;
                    }

                    Point3 leftLanePtA = (ptA + direction * (i + 1) * lanePart) + (orthonormalDirection * halfWidth);
                    if (brokenLeft && showLeft) {
                        stringstream leftLaneMarkingName;
                        leftLaneMarkingName << "Left lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                        layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(leftLaneMarkingName.str()), leftLanePtAOld, leftLanePtA, colorLeftMarking, 5));
                    }
                    leftLanePtAOld = leftLanePtA;

                    switch (straightLine.getLaneAttribute().getRightLaneMarking()) {
                        case LaneAttribute::SOLID_YELLOW:
                        case LaneAttribute::DOUBLE_YELLOW:
                            colorRightMarking = Point3(1, 1, 0);
                        break;

                        case LaneAttribute::BROKEN_WHITE:
                            colorRightMarking = Point3(1, 1, 1);
                            brokenRight = !brokenRight;
                        break;

                        case LaneAttribute::SOLID_WHITE:
                            colorRightMarking = Point3(1, 1, 1);
                        break;

                        case LaneAttribute::UNDEFINED:
                            showRight = false;
                        break;

                        case LaneAttribute::CROSSWALK:
                            colorRightMarking = Point3(0, 0, 0);
                        break;
                    }

                    Point3 rightLanePtA = (ptA + direction * (i + 1) * lanePart) - (orthonormalDirection * halfWidth);
                    if (brokenRight && showRight) {
                        stringstream rightLaneMarkingName;
                        rightLaneMarkingName << "Right lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                        layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(rightLaneMarkingName.str()), rightLanePtAOld, rightLanePtA, colorRightMarking, 5));
                    }
                    rightLanePtAOld = rightLanePtA;
                }

                // Draw last segment.
                Point3 leftLaneLastPt = ptB + (orthonormalDirection * halfWidth);
                if (brokenLeft && showLeft) {
                    stringstream leftLaneMarkingName;
                    leftLaneMarkingName << "Left lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(leftLaneMarkingName.str()), leftLanePtAOld, leftLaneLastPt, colorLeftMarking, 5));
                }

                Point3 rightLaneLastPt = ptB - (orthonormalDirection * halfWidth);
                if (brokenRight && showRight) {
                    stringstream rightLaneMarkingName;
                    rightLaneMarkingName << "Right lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(rightLaneMarkingName.str()), rightLanePtAOld, rightLaneLastPt, colorRightMarking, 5));
                }
 
            }
        }

        void ScenarioOpenGLSceneTransformation::transform(const Layer &layer, const Road &road, const Lane &lane, const Arc &arc, threeD::TransformGroup &layerTG) {
            // Get to adjacent vertices, determine direction, construct orthogonal direction
            Point3 ptA = arc.getStart();
            ptA.setZ(layer.getHeight());

            Point3 ptB = arc.getEnd();
            ptB.setZ(layer.getHeight());

            stringstream namePtA;
            namePtA << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << arc.getStart().getID();

            stringstream namePtB;
            namePtB << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << arc.getEnd().getID();

            if (m_renderLaneConnectors) {
                layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtA.str()), ptA, Point3(1, 0, 0), 5));
                layerTG.addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtB.str()), ptB, Point3(1, 0, 0), 5));
            }

            // ScUI starts Arc at 6pm but we use 3pm. Thus, we have to correct the Z-rotation.
            double correctRotation = Constants::PI / 2.0;

            // Transposition of the arc.
            Point3 t(0, 0, 0);
            t.setX(ptA.getX() - arc.getRadius() * cos(arc.getRotationZ() - correctRotation));
            t.setY(ptA.getY() - arc.getRadius() * sin(arc.getRotationZ() - correctRotation));      
            t.setZ(layer.getHeight());

            // Calculate skeleton points.
            Point3 colorSkeleton(0.01, 0.01, 0.01);

            double beginInterval = arc.getBeginInterval();
            double endInterval = arc.getEndInterval();
            double stepSize = 5.0 * Constants::PI / 180.0; // 5° to rad.
            uint32_t steps = (unsigned int) round( (endInterval - beginInterval) / stepSize );

            double width = arc.getLaneAttribute().getWidth();

            Point3 leftOld;
            leftOld.setX(t.getX() + (arc.getRadius() - width/2.0) * cos(arc.getRotationZ() - correctRotation));
            leftOld.setY(t.getY() + (arc.getRadius() - width/2.0) * sin(arc.getRotationZ() - correctRotation));      
            leftOld.setZ(layer.getHeight());

            Point3 rightOld;
            rightOld.setX(t.getX() + (arc.getRadius() + width/2.0) * cos(arc.getRotationZ() - correctRotation));
            rightOld.setY(t.getY() + (arc.getRadius() + width/2.0) * sin(arc.getRotationZ() - correctRotation));      
            rightOld.setZ(layer.getHeight());

            Point3 centerOld = ptA;

            bool brokenLeft = true;
            bool brokenRight = true;
            bool showLeft = true;
            bool showRight = true;
            for(uint32_t i = 0; i < steps; i++) {
                // Calculate the skeleton approximation.                
                Point3 center(0, 0, 0);
                center.setX(arc.getRadius() * cos(arc.getRotationZ() - correctRotation + i * stepSize));
                center.setY(arc.getRadius() * sin(arc.getRotationZ() - correctRotation + i * stepSize));

                // Calculate left point.
                Point3 left(0, 0, 0);
                left.setX((arc.getRadius() - width/2.0) * cos(arc.getRotationZ() - correctRotation + i * stepSize));
                left.setY((arc.getRadius() - width/2.0) * sin(arc.getRotationZ() - correctRotation + i * stepSize));

                Point3 right(0, 0, 0);
                right.setX((arc.getRadius() + width/2.0) * cos(arc.getRotationZ() - correctRotation + i * stepSize));
                right.setY((arc.getRadius() + width/2.0) * sin(arc.getRotationZ() - correctRotation + i * stepSize));

                // Transpose the points.
                center += t;
                left += t;
                right += t;
/*
                // Add center point to TG.
                stringstream nameLane;
                nameLane << "Lane: " << layer.getID() << "." << road.getID() << "." << lane.getID() << " (" << i << ")";
                layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(nameLane.str()), centerOld, center, colorSkeleton, 1));
*/
                // Add left point to TG.
                Point3 colorLeftMarking;
                switch (arc.getLaneAttribute().getLeftLaneMarking()) {
                    case LaneAttribute::SOLID_YELLOW:
                    case LaneAttribute::DOUBLE_YELLOW:
                        colorLeftMarking = Point3(1, 1, 0);
                    break;

                    case LaneAttribute::BROKEN_WHITE:
                        if (brokenLeft) {
                            colorLeftMarking = Point3(0, 0, 0);
                        }
                        else {                        
                            colorLeftMarking = Point3(1, 1, 1);
                        }
                        brokenLeft = !brokenLeft;
                    break;

                    case LaneAttribute::SOLID_WHITE:
                        colorLeftMarking = Point3(1, 1, 1);
                    break;

                    case LaneAttribute::UNDEFINED:
                        showLeft = false;
                    break;

                    case LaneAttribute::CROSSWALK:
                        colorLeftMarking = Point3(0, 0, 0);
                    break;
                }

                if (brokenLeft && showLeft) {
                    stringstream leftLaneMarkingName;
                    leftLaneMarkingName << "Left lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID() << " (" << i << ")";
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(leftLaneMarkingName.str()), leftOld, left, colorLeftMarking, 5));
                }

                // Add right point to TG.
                Point3 colorRightMarking;
                switch (arc.getLaneAttribute().getRightLaneMarking()) {
                    case LaneAttribute::SOLID_YELLOW:
                    case LaneAttribute::DOUBLE_YELLOW:
                        colorRightMarking = Point3(1, 1, 0);
                    break;

                    case LaneAttribute::BROKEN_WHITE:
                        if (brokenRight) {
                            colorRightMarking = Point3(0, 0, 0);
                        }
                        else {                        
                            colorRightMarking = Point3(1, 1, 1);
                        }
                        brokenRight = !brokenRight;
                    break;

                    case LaneAttribute::SOLID_WHITE:
                        colorRightMarking = Point3(1, 1, 1);
                    break;

                    case LaneAttribute::UNDEFINED:
                        showRight = false;
                    break;

                    case LaneAttribute::CROSSWALK:
                        colorRightMarking = Point3(0, 0, 0);
                    break;
                }

                if (brokenRight && showRight) {
                    stringstream rightLaneMarkingName;
                    rightLaneMarkingName << "Right lane marking: " << layer.getID() << "." << road.getID() << "." << lane.getID() << " (" << i << ")";
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(rightLaneMarkingName.str()), rightOld, right, colorRightMarking, 5));
                }

                // Keep old points.
                centerOld = center;
                leftOld = left;
                rightOld = right;
            }
        }

        void ScenarioOpenGLSceneTransformation::transform(const data::scenario::LaneModel &laneModel, threeD::TransformGroup &layerTG) {
            const vector<TrafficControl*>& listOfTrafficControls = laneModel.getListOfTrafficControls();
            vector<TrafficControl*>::const_iterator it = listOfTrafficControls.begin();
            while(it != listOfTrafficControls.end()) {
                TrafficControl *tc = (*it++);
                if (tc->getType() == TrafficControl::TRAFFICSIGN) {
                    TrafficSign *ts = (TrafficSign*)tc;
                    if (ts != NULL) {
                        cerr << "Stopline: " << ts->toString() << endl;
                        Shape *s = ts->getShape();
                        if ( (s != NULL) && (s->getType() == Shape::POLYGON) ) {
                            Polygon *p = dynamic_cast<Polygon*>(s);
                            if (p != NULL) {
                                vector<Point3> vertices;

                                // Convert vertices.
                                const vector<Vertex3> &originalVertices = p->getListOfVertices();
                                vector<Vertex3>::const_iterator jt = originalVertices.begin();
                                while (jt != originalVertices.end()) {
                                    Vertex3 v3 = (*jt++);
                                    vertices.push_back(v3);
                                }

                                Point3 color(p->getColor());
                                if (vertices.size() == 2) {
                                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(p->getName()), vertices.at(0), vertices.at(1), color, 5));
                                }
                            }
                        }
                    }
                }
            }
        }

        void ScenarioOpenGLSceneTransformation::transform(const data::scenario::Layer &/*layer*/, const data::scenario::Zone &z, threeD::TransformGroup &layerTG) {
            Point3 color(Point3(1, 1, 1));

            // Convert vertices.
            const vector<IDVertex3> &originalVertices = z.getPerimeter().getListOfIdentifiableVertices();
            vector<IDVertex3>::const_iterator jt = originalVertices.begin();

            Point3 startPoint = (*jt);
            Point3 oldPoint = startPoint;
            Point3 nextPoint = startPoint;

            bool firstCycle = true;
            while (jt != originalVertices.end()) {
                IDVertex3 idv3 = (*jt++);
                nextPoint = idv3;

                if (!firstCycle) {
                    layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(z.getName()), oldPoint, nextPoint, color, 5));
                }
                oldPoint = nextPoint;

                firstCycle = false;
            }
            layerTG.addChild(new threeD::models::Line(threeD::NodeDescriptor(z.getName()), nextPoint, startPoint, color, 5));
        }

        void ScenarioOpenGLSceneTransformation::visit(ScenarioNode &node) {
            try {
                Surroundings &surroundings = dynamic_cast<Surroundings&>(node);
                clog << "Surroundings found." <<  endl;
                transform(surroundings, *m_root);
            } catch (...) {}

            try {
                Layer &layer = dynamic_cast<Layer&>(node);
                clog << "Layer found." <<  endl;
                threeD::TransformGroup *layerTG = new threeD::TransformGroup(threeD::NodeDescriptor("Layer: " + layer.getName()));
                m_root->addChild(layerTG);

                const vector<Road> &listOfRoads = layer.getListOfRoads();
                vector<Road>::const_iterator it = listOfRoads.begin();
                while (it != listOfRoads.end()) {
                    Road road = (*it++);
                    const vector<Lane> &listOfLanes = road.getListOfLanes();
                    vector<Lane>::const_iterator jt = listOfLanes.begin();
                    while (jt != listOfLanes.end()) {
                        Lane lane = (*jt++);
                        LaneModel *laneModel = lane.getLaneModel();
                        if (laneModel != NULL) {
                            // Draw stop lines.
                            transform(*laneModel, *layerTG);

                            // Draw lane models.
                            bool drawn = false;
                            Arc *arc = dynamic_cast<Arc*>(laneModel);
                            if ( !drawn && (arc != NULL) ) {
                                transform(layer, road, lane, *arc, *layerTG);
                                drawn = true;
                            }

                            Clothoid *clothoid = dynamic_cast<Clothoid*>(laneModel);
                            if (clothoid != NULL) {
                                clog << "OpenGL transformation for Clothoid not implemented yet." << endl;
/*
                                Point3 ptA = clothoid->getStart();
                                ptA.setZ(layer.getHeight());

                                Point3 ptB = clothoid->getEnd();
                                ptB.setZ(layer.getHeight());

                                stringstream namePtA;
                                namePtA << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << clothoid->getStart().getID();

                                stringstream namePtB;
                                namePtB << "Waypoint: " << layer.getID() << "." << road.getID() << "." << lane.getID() << "." << clothoid->getStart().getID();

                                layerTG->addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtA.str()), ptA, Point3(1, 0, 0), 5));
                                layerTG->addChild(new threeD::models::Point(threeD::NodeDescriptor(namePtB.str()), ptB, Point3(1, 0, 0), 5));

                                Point3 colorSkeleton(0.2, 0.2, 0.2);
                                stringstream nameLane;
                                nameLane << "Lane: " << layer.getID() << "." << road.getID() << "." << lane.getID();
                                layerTG->addChild(new threeD::models::Line(threeD::NodeDescriptor(nameLane.str()), ptA, ptB, colorSkeleton, 1));
*/
                            }

                            PointModel *pointModel = dynamic_cast<PointModel*>(laneModel);
                            if ( !drawn && (pointModel != NULL) ) {
                                transform(layer, road, lane, *pointModel, *layerTG);
                                drawn = true;
                            }

                            StraightLine *straightLine = dynamic_cast<StraightLine*>(laneModel);
                            if ( !drawn && (straightLine != NULL) ) {
                                transform(layer, road, lane, *straightLine, *layerTG);
                                drawn = true;
                            }
                        }
                    }
                }

                const vector<Zone> &listOfZones = layer.getListOfZones();
                vector<Zone>::const_iterator jt = listOfZones.begin();
                while (jt != listOfZones.end()) {
                    Zone zone = (*jt++);

                    transform(layer, zone, *layerTG);
                }

            } catch (...) {}
        }

    }
} // hesperia::scenario
