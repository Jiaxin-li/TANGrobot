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

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "core/macros.h"
#include "core/data/Constants.h"
#include "core/data/environment/Point3.h"

#include "hesperia/decorator/models/OBJXArchive.h"
#include "hesperia/decorator/models/Triangle.h"
#include "hesperia/decorator/models/TriangleSet.h"

#include "hesperia/threeD/TextureManager.h"

namespace hesperia {
    namespace decorator {
        namespace models {

            using namespace core;
            using namespace core::data::environment;

            OBJXArchive::OBJXArchive() :
                    m_mapOfImages(),
                    m_mapOfMaterials(),
                    m_objFile(),
                    m_mtlFile() {}

            OBJXArchive::~OBJXArchive() {
                map<string, wrapper::Image*, wrapper::StringComparator>::iterator it = m_mapOfImages.begin();

                while (it != m_mapOfImages.end()) {
                    clog << "Deleting " << it->first << "...";
                    OPENDAVINCI_CORE_DELETE_POINTER(it->second);
                    clog << "done." << endl;
                    it++;
                }
            }

            void OBJXArchive::setContentsOfObjFile(const string &objContents) {
                m_objFile.str(objContents);
            }

            void OBJXArchive::setContentsOfMtlFile(const string &mtlContents) {
                m_mtlFile.str(mtlContents);
            }

            const stringstream& OBJXArchive::getContentsOfObjFile() const {
                return m_objFile;
            }

            const stringstream& OBJXArchive::getContentsOfMtlFile() const {
                return m_mtlFile;
            }

            void OBJXArchive::addImage(const string &name, wrapper::Image *image) {
                if ( (name.length() > 0) && (image != NULL) && (image->getWidth() > 0) && (image->getHeight() > 0) ) {
                    wrapper::Image *existingEntry = m_mapOfImages[name];

                    if (existingEntry == NULL) {
                        clog << "Adding " << name << ", W: " << image->getWidth() << ", H: " << image->getHeight() << endl;
                        m_mapOfImages[name] = image;
                    }
                }
            }

            vector<string> OBJXArchive::getListOfImages() const {
                vector<string> listOfImages;

                map<string, core::wrapper::Image*, core::wrapper::StringComparator>::const_iterator it = m_mapOfImages.begin();
                while (it != m_mapOfImages.end()) {
                    listOfImages.push_back(it->first);
                    it++;
                }

                return listOfImages;
            }

            void OBJXArchive::createMapOfMaterials() {
                m_mapOfMaterials.clear();

                if (m_mtlFile.str().length() > 0) {
                    m_mtlFile.seekg(ios::beg);
                    string line = "";
                    Material m;
                    while (getline(m_mtlFile, line)) {
                        if ( (line.find("newmtl ") != string::npos) && (line.length() > 8) ) {
                            // Store old material description.
                            if (m.getName() != "Undefined") {
                                m_mapOfMaterials[m.getName()] = m;
                            }

                            // Start next material description.
                            m = Material(line.substr(7));
                        }

                        if ( (line.find("Ns ") != string::npos) && (line.length() > 4) ) {
                            stringstream shininess(line.substr(3));
                            double s;
                            shininess >> s;
                            m.setShininess(s);
                        }

                        if ( (line.find("Ka ") != string::npos) && (line.length() > 4) ) {
                            stringstream ambient(line.substr(3));
                            double x;
                            double y;
                            double z;
                            ambient >> x;
                            ambient >> y;
                            ambient >> z;
                            m.setAmbient(Point3(x, y, z));
                        }

                        if ( (line.find("Kd ") != string::npos) && (line.length() > 4) ) {
                            stringstream diffuse(line.substr(3));
                            double x;
                            double y;
                            double z;
                            diffuse >> x;
                            diffuse >> y;
                            diffuse >> z;
                            m.setDiffuse(Point3(x, y, z));
                        }

                        if ( (line.find("Ks ") != string::npos) && (line.length() > 4) ) {
                            stringstream specular(line.substr(3));
                            double x;
                            double y;
                            double z;
                            specular >> x;
                            specular >> y;
                            specular >> z;
                            m.setSpecular(Point3(x, y, z));
                        }

                        if ( (line.find("map_Kd ") != string::npos) && (line.length() > 8) ) {
                            m.setTextureName(line.substr(7));
                            m.setImage(m_mapOfImages[m.getTextureName()]);
                        }
                    }
                }
            }

            const vector<TriangleSet> OBJXArchive::getListOfTriangleSets() {
                vector<TriangleSet> listOfTriangleSets;

                uint32_t triangleCounter = 0;

                // Read materials.
                createMapOfMaterials();

                if (m_objFile.str().length() > 0) {
                    // Parse all available vertices.
                    vector<Point3> listOfVertices;
                    vector<Point3> listOfNormals;
                    vector<Point3> listOfTextureCoordinates;

                    m_objFile.seekg(ios::beg);
                    string line = "";
                    while (getline(m_objFile, line)) {
                        // Vertices.
                        if (line.find("v ") != string::npos) {
                            stringstream xyz(line.substr(2));
                            double x;
                            double y;
                            double z;
                            xyz >> x;
                            xyz >> y;
                            xyz >> z;
                            listOfVertices.push_back(Point3(x, y, z));
                        }

                        // Normals.
                        if (line.find("vn") != string::npos) {
                            stringstream xyz(line.substr(3));
                            double x;
                            double y;
                            double z;
                            xyz >> x;
                            xyz >> y;
                            xyz >> z;
                            listOfNormals.push_back(Point3(x, y, z));
                        }

                        // TextureCoordinates.
                        if (line.find("vt") != string::npos) {
                            stringstream xy(line.substr(3));
                            double x;
                            double y;
                            xy >> x;
                            xy >> y;
                            listOfTextureCoordinates.push_back(Point3(x, y, 0));
                        }
                    }

                    // Rewind file to parse faces.
                    m_objFile.clear();
                    m_objFile.seekg(ios::beg);
                    line = "";

                    // Add root triangle set if no groups are defined.
                    TriangleSet triangleSet;
                    while (getline(m_objFile, line)) {
                        if (line.find("g ") != string::npos) {
                            // Add new group.
                            listOfTriangleSets.push_back(triangleSet);

                            triangleSet = TriangleSet();
                        }

                        if ( (line.find("usemtl ") != string::npos) && (line.length() > 8) ) {
                            Material m = m_mapOfMaterials[line.substr(7)];
                            triangleSet.setMaterial(m);
                        }

                        if (line.find("f ") != string::npos) {
                            vector<Point3> vertices;
                            vector<Point3> textureCoordinates;
                            vector<Point3> normals;
                            uint32_t numberOfVertices = 0;
                            stringstream unknownFormat(line.substr(2));
                            if (line.find("//") != string::npos) {
                                // Format is v//n.
                                stringstream vnStream(line.substr(2));
                                while (vnStream.good() && (numberOfVertices < 3)) {
                                    string triangle;
                                    vnStream >> triangle;

                                    replace(triangle.begin(), triangle.end(), '/', ' ');
                                    stringstream triangleStream(triangle);
                                    numberOfVertices++;

                                    uint32_t v;
                                    uint32_t n;

                                    triangleStream >> v;
                                    triangleStream >> n;

                                    if ( (v > 0) && (n > 0) ) {
                                        if ( (v - 1) < listOfVertices.size() ) {
                                            vertices.push_back(listOfVertices[(v-1)]);
                                        }
                                        if ( (n - 1) < listOfNormals.size() ) {
                                            normals.push_back(listOfNormals[(n-1)]);
                                        }
                                    }
                                }

                                // Add the result.
                                if (vertices.size() == 3) {
                                    Triangle t;
                                    t.setVertices(vertices[0], vertices[1], vertices[2]);
                                    if (!normals.empty()) {
                                        t.setNormal(normals[0]);
                                    }
                                    triangleSet.addTriangle(t);
                                    triangleCounter++;
                                }
                            }
                            else if (line.find("/") == string::npos) {
                                // Format is v.
                                stringstream vStream(line.substr(2));
                                while (vStream.good() && (numberOfVertices < 3)) {
                                    uint32_t v;
                                    vStream >> v;
                                    numberOfVertices++;

                                    if (v > 0) {
                                        if ( (v - 1) < listOfVertices.size() ) {
                                            vertices.push_back(listOfVertices[(v-1)]);
                                        }
                                    }
                                }

                                // Add the result.
                                if (vertices.size() == 3) {
                                    Triangle t;
                                    t.setVertices(vertices[0], vertices[1], vertices[2]);
                                    triangleSet.addTriangle(t);
                                    triangleCounter++;
                                }
                            }
                            else {
                                // Format could be v/t/n or v/t.
                                string s;
                                unknownFormat >> s;

                                uint32_t numberOfSlashes = count(s.begin(), s.end(), '/');
                                if (numberOfSlashes == 2) {
                                    // Format is v/t/n.
                                    stringstream vtnStream(line.substr(2));
                                    while (vtnStream.good() && (numberOfVertices < 3)) {
                                        string triangle;
                                        vtnStream >> triangle;

                                        replace(triangle.begin(), triangle.end(), '/', ' ');
                                        stringstream triangleStream(triangle);
                                        numberOfVertices++;

                                        uint32_t v;
                                        uint32_t t;
                                        uint32_t n;

                                        triangleStream >> v;
                                        triangleStream >> t;
                                        triangleStream >> n;

                                        if ( (v > 0) && (t > 0) && (n > 0) ) {
                                            if ( (v - 1) < listOfVertices.size() ) {
                                                vertices.push_back(listOfVertices[(v-1)]);
                                            }
                                            if ( (t - 1) < listOfTextureCoordinates.size() ) {
                                                textureCoordinates.push_back(listOfTextureCoordinates[(t-1)]);
                                            }
                                            if ( (n - 1) < listOfNormals.size() ) {
                                                normals.push_back(listOfNormals[(n-1)]);
                                            }
                                        }
                                    }

                                    // Add the result.
                                    if (vertices.size() == 3) {
                                        Triangle t;
                                        t.setVertices(vertices[0], vertices[1], vertices[2]);
                                        if (!normals.empty()) {
                                            t.setNormal(normals[0]);
                                        }
                                        if (textureCoordinates.size() == 3) {
                                            t.setTextureCoordinates(textureCoordinates[0], textureCoordinates[1], textureCoordinates[2]);
                                        }
                                        triangleSet.addTriangle(t);
                                        triangleCounter++;
                                    }
                                }
                                else if (numberOfSlashes == 1) {
                                    // Format is v/t.
                                    stringstream vtStream(line.substr(2));
                                    while (vtStream.good() && (numberOfVertices < 3)) {
                                        string triangle;
                                        vtStream >> triangle;

                                        replace(triangle.begin(), triangle.end(), '/', ' ');
                                        stringstream triangleStream(triangle);
                                        numberOfVertices++;

                                        uint32_t v;
                                        uint32_t t;

                                        triangleStream >> v;
                                        triangleStream >> t;

                                        if ( (v > 0) && (t > 0) ) {
                                            if ( (v - 1) < listOfVertices.size() ) {
                                                vertices.push_back(listOfVertices[(v-1)]);
                                            }
                                            if ( (t - 1) < listOfTextureCoordinates.size() ) {
                                                textureCoordinates.push_back(listOfTextureCoordinates[(t-1)]);
                                            }
                                        }
                                    }

                                    // Add the result.
                                    if (vertices.size() == 3) {
                                        Triangle t;
                                        t.setVertices(vertices[0], vertices[1], vertices[2]);
                                        if (textureCoordinates.size() == 3) {
                                            t.setTextureCoordinates(textureCoordinates[0], textureCoordinates[1], textureCoordinates[2]);
                                        }
                                        triangleSet.addTriangle(t);
                                        triangleCounter++;
                                    }
                                }
                                else {
                                    clog << "Unknown format." << endl;
                                }
                            }
                        }
                    }
                }

                clog << "Model contains " << triangleCounter << " triangles." << endl;

                return listOfTriangleSets;
            }

        }
    }
} // hesperia::decorator::models
