/**
 * lanedetector - Sample application for detecting lane markings.
 * Copyright (C) 2012 - 2015 Christian Berger
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either ver_centr_startion 2
 * of the License, or (at your option) any later ver_centr_startion.
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

#include <iostream>
#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "core/macros.h"
#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/image/SharedImage.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"

#include "tools/player/Player.h"

#include "GeneratedHeaders_Data.h"

#include "LaneDetector.h"

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::image;
    using namespace tools::player;
    using namespace cv;

    LaneDetector::LaneDetector(const int32_t &argc, char **argv) : ConferenceClientModule(argc, argv, "lanedetector"),
        m_hasAttachedToSharedImageMemory(false),
        m_sharedImageMemory(),
        m_image(NULL),
        m_debug(false) {}

    LaneDetector::~LaneDetector() {}

    void LaneDetector::setUp() {
	    // This method will be call automatically _before_ running body().
	    if (m_debug) {
		    // Create an OpenCV-window.
		    cvNamedWindow("WindowShowImage", CV_WINDOW_AUTOSIZE);
		    cvMoveWindow("WindowShowImage", 300, 100);
	    }
    }

    void LaneDetector::tearDown() {
	    // This method will be call automatically _after_ return from body().
	    if (m_image != NULL) {
		    cvReleaseImage(&m_image);
	    }

	    if (m_debug) {
		    cvDestroyWindow("WindowShowImage");
	    }
    }

    bool LaneDetector::readSharedImage(Container &c) {
	    bool retVal = false;

	    if (c.getDataType() == Container::SHARED_IMAGE) {
		    SharedImage si = c.getData<SharedImage> ();

		    // Check if we have already attached to the shared memory.
		    if (!m_hasAttachedToSharedImageMemory) {
			    m_sharedImageMemory
					    = core::wrapper::SharedMemoryFactory::attachToSharedMemory(
							    si.getName());
		    }

		    // Check if we could successfully attach to the shared memory.
		    if (m_sharedImageMemory->isValid()) {
			    // Lock the memory region to gain exclusive access. REMEMBER!!! DO NOT FAIL WITHIN lock() / unlock(), otherwise, the image producing process would fail.
			    m_sharedImageMemory->lock();
			    {
				    const uint32_t numberOfChannels = 3;
				    // For example, simply show the image.
				    if (m_image == NULL) {
					    m_image = cvCreateImage(cvSize(si.getWidth(), si.getHeight()), IPL_DEPTH_8U, numberOfChannels);
				    }

				    // Copying the image data is very expensive...
				    if (m_image != NULL) {
					    memcpy(m_image->imageData,
							   m_sharedImageMemory->getSharedMemory(),
							   si.getWidth() * si.getHeight() * numberOfChannels);
				    }
			    }

			    // Release the memory region so that the image produce (i.e. the camera for example) can provide the next raw image data.
			    m_sharedImageMemory->unlock();

			    // Mirror the image.
			    cvFlip(m_image, 0, -1);

			    retVal = true;
		    }
	    }
	    return retVal;
    }

    // You should start your work in this method.
    void LaneDetector::processImage() {
        // Example: Show the image.
    	
    	//draw a line       
    	        int width = m_image -> width;
    	        int height = m_image -> height;
    	        int step  = m_image ->widthStep;
    	        unsigned char* image = (unsigned char*)m_image->imageData;
    	        int sample_near = 60; // define near vision
    	        int sample_far = 240; // define further vision
    	        int desired_right_near =243; //220
    	        int desired_right_far =75; 
    	        double k = 0.2; //portion control 
    	        int max_left = -24;
    	        int max_right = 24;// max turning steering 
    	        //cout << " width:"<< width<<endl;
    	        //cout << " height:"<< height<<endl;
    	        CvScalar red = CV_RGB(250,0,0);
    	        CvScalar green = CV_RGB(0,250,0);
    	        int thickness = 1;
    	        int connectivity = 8;
    	        bool use_near = 1;// deceide which vision to use;
    	        
    	        CvPoint ver_centr_start = cvPoint(width/2,height);
    	        CvPoint ver_centr_end = cvPoint(width/2,0);
    	        CvPoint near_sample_start = cvPoint(width/2,height- sample_near);
    	        CvPoint near_sample_end ;
    	        CvPoint far_sample_start = cvPoint(width/2,height- sample_far);
    	        CvPoint far_sample_end ;
    	       
    	        
       

        //TODO: Start here.
        
        int right_near = 0;
        int right_far = 0;
        // 1. Do something with the image m_image here, for example: find lane marking features, optimize quality, ...
        // find right distance 
        //  I (x, y) ~ ((unsigned char*) (img-> imageData + img-> widthStep * y)) [x]        3 chanel     
        
        while((image + step * (height- sample_near+2)) [(width/2+right_near)*3]==0 && right_near < width/2 ){ right_near ++;} 
        while((image + step * (height- sample_far+2)) [(width/2+right_far)*3]==0 && right_far < width/2 ) {right_far++; }
       
        if (right_near >= width/2){ // near vision lost
        	use_near = 0;  	   
        }
        else {
        	use_near = 1; 
        }
       cout << "right_near:"<< right_near<<endl;
       cout << "right_far:"<< right_far<<endl;
       near_sample_end = cvPoint(width/2 + right_near ,height- sample_near);
       far_sample_end = cvPoint(width/2 + right_far ,height- sample_far);
       
       if (m_debug) {
                  if (m_image != NULL) {
                	  cvLine(m_image,ver_centr_start,ver_centr_end,red,thickness,connectivity);
                	  cvLine(m_image,near_sample_start,near_sample_end,green,thickness,connectivity);
                	  cvLine(m_image,far_sample_start,far_sample_end,green,thickness,connectivity);
                  	
                      cvShowImage("WindowShowImage", m_image);
                      cvWaitKey(10);
                  }
              }
       
        // 2. Calculate desired steering commands from your image features to be processed by driver.
       double difference;
       if (use_near){
    	   difference = (  right_near-desired_right_near) *k ; //right_near- desired_right_near   	   
       } 
       else{
    	   difference = (  right_far-desired_right_far) *k ; //right_near- desired_right_near  	   
       }
       
       if (difference < max_left) difference = max_left;
       else if ( difference > max_right) difference = max_right;
        
        
        
        // Here, you see an example of how to send the data structure SteeringData to the ContainerConference. This data structure will be received by all running components. In our example, it will be processed by Driver. To change this data structure, have a look at Data.odvd in the root folder of this source.
        SteeringData sd;
        sd.setExampleData(difference);

        // Create container for finally sending the data.
        Container c(Container::USER_DATA_1, sd);
        // Send container.
        getConference().send(c);
    }

    // This method will do the main data processing job.
    // Therefore, it tries to open the real camera first. If that fails, the virtual camera images from camgen are used.
    ModuleState::MODULE_EXITCODE LaneDetector::body() {
	    // Get configuration data.
	    KeyValueConfiguration kv = getKeyValueConfiguration();
	    m_debug = kv.getValue<int32_t> ("lanedetector.debug") == 1;

        Player *player = NULL;
/*
        // Lane-detector can also directly read the data from file. This might be interesting to inspect the algorithm step-wisely.
        core::io::URL url("file://recorder.rec");

        // Size of the memory buffer.
        const uint32_t MEMORY_SEGMENT_SIZE = kv.getValue<uint32_t>("global.buffer.memorySegmentSize");

        // Number of memory segments.
        const uint32_t NUMBER_OF_SEGMENTS = kv.getValue<uint32_t>("global.buffer.numberOfMemorySegments");

        // If AUTO_REWIND is true, the file will be played endlessly.
        const bool AUTO_REWIND = true;

        player = new Player(url, AUTO_REWIND, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS);
*/

        
	    while (getModuleState() == ModuleState::RUNNING) {
		    bool has_next_frame = false;

		    // Use the shared memory image.
            Container c;
            if (player != NULL) {
		        // Read the next container from file.
                c = player->getNextContainerToBeSent();
            }
            else {
		        // Get the most recent available container for a SHARED_IMAGE.
		        c = getKeyValueDataStore().get(Container::SHARED_IMAGE);
            }                

		    if (c.getDataType() == Container::SHARED_IMAGE) {
			    // Example for processing the received container.
			    has_next_frame = readSharedImage(c);
		    }

		    // Process the read image.
		    if (true == has_next_frame) {
			    processImage();
		    }
	    }

        OPENDAVINCI_CORE_DELETE_POINTER(player);

	    return ModuleState::OKAY;
    }

} // msv

