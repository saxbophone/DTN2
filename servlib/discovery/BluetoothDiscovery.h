/*
 *    Copyright 2006 Baylor University
 * 
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 * 
 *        http://www.apache.org/licenses/LICENSE-2.0
 * 
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#ifndef _BT_DISCOVERY_H_
#define _BT_DISCOVERY_H_

#include <config.h>

#ifdef OASYS_BLUETOOTH_ENABLED
#include <bluetooth/bluetooth.h>

#include <oasys/thread/Thread.h>
#include <oasys/thread/Notifier.h>

#include "Discovery.h"

namespace dtn {

/**
 * Neighbor discovery main thread for Bluetooth CL
 */
class BluetoothDiscovery : public Discovery,
                           public oasys::Thread
{
public:
    void shutdown();

    virtual ~BluetoothDiscovery() { shutdown(); }
protected:
    friend class Discovery;

    BluetoothDiscovery(const std::string& name);
    bool configure(int argc, const char* argv[]);
    void run();

    volatile bool shutdown_;
    bdaddr_t local_addr_;
    oasys::Notifier notifier_;
};

} // dtn

#endif // OASYS_BLUETOOTH_ENABLED
#endif // _BT_DISCOVERY_H_