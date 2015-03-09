/**
 * chasecar - Render video from chasing the ego car
 * Copyright (C) 2012 - 2015 Christian Berger
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

#ifndef CHASECAR_H_
#define CHASECAR_H_

#include <vector>

#include <opencv/cv.h>

#include "core/SharedPointer.h"
#include "core/wrapper/Image.h"
#include "core/base/FIFOQueue.h"

#include "core/base/ConferenceClientModule.h"
#include "hesperia/data/environment/EgoState.h"
#include "hesperia/data/environment/Obstacle.h"
#include "hesperia/io/camera/ImageGrabber.h"

#include "OpenGLGrabber.h"

namespace chasecar {

    using namespace std;

    /**
     * This class is the camera server providing new camera images from the chase car perspective.
     */
    class ChaseCar : public core::base::ConferenceClientModule {
        private:
            /**
             * "Forbidden" copy constructor. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the copy constructor.
             *
             * @param obj Reference to an object of this class.
             */
            ChaseCar(const ChaseCar &/*obj*/);

            /**
             * "Forbidden" assignment operator. Goal: The compiler should warn
             * already at compile time for unwanted bugs caused by any misuse
             * of the assignment operator.
             *
             * @param obj Reference to an object of this class.
             * @return Reference to this instance.
             */
            ChaseCar& operator=(const ChaseCar &/*obj*/);

        public:
            /**
             * Constructor.
             *
             * @param argc Number of command line arguments.
             * @param argv Command line arguments.
             */
            ChaseCar(const int32_t &argc, char **argv);

            virtual ~ChaseCar();

            virtual void setUp();

            virtual void tearDown();

            core::base::ModuleState::MODULE_EXITCODE body();

            /**
             * This method returns the singleton instance.
             *
             * @return The singleton pointer.
             */
            static ChaseCar& getInstance();

            /**
             * This method is called by OpenGL to draw the scene.
             */
            void display();

            /**
             * This method is called by OpenGL for resizing
             * the window.
             *
             * @param w new width.
             * @param h new height.
             */
            void resize(int32_t w, int32_t h);

            /**
             * This method processes any key event.
             *
             * @param key The pressed key.
             * @param x
             * @param y
             */
            void processKey(unsigned char key, int32_t x, int32_t y);

            /**
             * This method processes any mouse motion event.
             *
             * @param x Mouse's X position.
             * @param y Mouse's Y position.
             */
            void processMouseMotion(int32_t x, int32_t y);

            /**
             * This method processes any mouse event.
             *
             * @param button The pressed mouse button.
             * @param state GLUT_DOWN or GLUT_UP.
             * @param x Mouse's X position.
             * @param y Mouse's Y position.
             */
            void processMouseEvent(int32_t button, int32_t state, int32_t x, int32_t y);

        private:
            static ChaseCar* m_singleton;
            hesperia::data::environment::EgoState m_egoState;

            core::base::FIFOQueue m_FIFO_Obstacles;

            OpenGLGrabber *m_grabber;
            core::SharedPointer<core::wrapper::Image> m_image;

            float m_translationX;
            float m_translationY;
            float m_scale;
            float m_phi;
            float m_theta;
            float m_height;

            int32_t m_mouseX;
            int32_t m_mouseY;
            int32_t m_mouseButton;

            /**
             * This method initializes the GLUT subsystem.
             */
            void initGlut();

            /**
             * This method initializes the GL subsystem.
             */
            void initGL();

            /**
             * This method draws the three dimensional scene.
             */
            void drawScene();
    };

} // chasecar

#endif /*CHASECAR_H_*/
