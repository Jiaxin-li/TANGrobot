/**
 * OpenDaVINCI - Portable middleware for distributed components.
 * Copyright (C) 2014 - 2015 Christian Berger
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "core/data/Container.h"
#include "context/base/RecordingContainer.h"

namespace context {
    namespace base {

        using namespace std;
        using namespace core::data;
        using namespace tools::recorder;

		RecordingContainer::RecordingContainer(const float &freq, const string &urlFileName, const uint32_t &memorySegmentSize, const uint32_t &numberOfSegments) :
			m_freq(freq),
            m_urlFileName(urlFileName),
            m_memorySegmentSize(memorySegmentSize),
            m_numberOfSegments(numberOfSegments),
            m_recorder(NULL) {
        }

		RecordingContainer::~RecordingContainer() {
            OPENDAVINCI_CORE_DELETE_POINTER(m_recorder);
        }

		void RecordingContainer::setup() {
			if (m_urlFileName != "") {
                // We can use the sychronous recorder as we are running in a deterministic simulation anyways.
                const bool THREADING = false;
                m_recorder = new Recorder(m_urlFileName, m_memorySegmentSize, m_numberOfSegments, THREADING);
			}
		}

        void RecordingContainer::tearDown() {}

		void RecordingContainer::report(const core::wrapper::Time &/*t*/) {
			if (m_recorder != NULL) {
	            const uint32_t SIZE = getFIFO().getSize();
	            for (uint32_t i = 0; i < SIZE; i++) {
	                Container c = getFIFO().leave();
	                cerr << "(RecordingContainer) Storing '" << c.toString() << "'." << endl;
                    m_recorder->store(c);
	            }
			}
			getFIFO().clear();
		}

		float RecordingContainer::getFrequency() const {
			return m_freq;
		}

    }
} // context::base
