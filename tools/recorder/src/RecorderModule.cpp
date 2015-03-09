/**
 * recorder - Tool for recording data
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

#include <iostream>

#include "core/macros.h"
#include "core/base/KeyValueDataStore.h"
#include "core/base/Thread.h"
#include "core/data/Container.h"
#include "core/data/recorder/RecorderCommand.h"

#include "tools/recorder/Recorder.h"
#include "RecorderModule.h"

namespace recorder {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::io;
    using namespace tools::recorder;

    RecorderModule::RecorderModule(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "Recorder") {}

    RecorderModule::~RecorderModule() {}

    void RecorderModule::setUp() {}

    void RecorderModule::tearDown() {}

    void RecorderModule::wait() {
        AbstractModule::wait();
    }

    ModuleState::MODULE_EXITCODE RecorderModule::body() {
        // Check if the recorder is remotely controlled.
        bool remoteControl = (getKeyValueConfiguration().getValue<bool>("recorder.remoteControl") != 0);

        // URL for storing containers.
        const string recorderOutputURL = getKeyValueConfiguration().getValue<string>("recorder.output");
        // Size of memory segments.
        const uint32_t MEMORY_SEGMENT_SIZE = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.memorySegmentSize");
        // Number of memory segments.
        const uint32_t NUMBER_OF_SEGMENTS = getKeyValueConfiguration().getValue<uint32_t>("global.buffer.numberOfMemorySegments");
        // Run recorder in asynchronous mode to allow real-time recording in background.
        const bool THREADING = true;

        // Actual "recording" interface.
        Recorder r(recorderOutputURL, MEMORY_SEGMENT_SIZE, NUMBER_OF_SEGMENTS, THREADING);

        // Connect recorder's FIFOQueue to record all containers except for shared images/shared data.
        addDataStoreFor(r.getFIFO());

        // Connect recorder's data store that can handle shared data.
        addDataStoreFor(Container::SHARED_DATA, r.getDataStoreForSharedData());
        addDataStoreFor(Container::SHARED_IMAGE, r.getDataStoreForSharedData());

        // Get key/value-datastore for controlling the recorder.
        KeyValueDataStore &kvds = getKeyValueDataStore();

        // If remote control is disabled, simply start recording immediately.
        bool recording = (!remoteControl);
        while (getModuleState() == ModuleState::RUNNING) {
            // Recording queued entries.
            if (recording) {
                if (!r.getFIFO().isEmpty()) {
                    r.recordQueueEntries();
                }
                else {
                    Thread::usleep(500);
                }
            }

            // Check for remote control.
            if (remoteControl) {
                Container container = kvds.get(Container::RECORDER_COMMAND);
                if (container.getDataType() == Container::RECORDER_COMMAND) {
                    core::data::recorder::RecorderCommand rc;
                    rc = container.getData<core::data::recorder::RecorderCommand>();

                    recording = (rc.getCommand() == core::data::recorder::RecorderCommand::RECORD);
                }

                // Discard existing entries.
                if (!recording) {
                    r.getFIFO().clear();
                }
            }
        }

        return ModuleState::OKAY;
    }

} // recorder
