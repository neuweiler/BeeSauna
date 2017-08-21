/*
 * Status.cpp
 *
 * Holds the state machine's current status and transition logic as well as
 * some global status values.
 *
 Copyright (c) 2017 Michael Neuweiler

 Permission is hereby granted, free of charge, to any person obtaining
 a copy of this software and associated documentation files (the
 "Software"), to deal in the Software without restriction, including
 without limitation the rights to use, copy, modify, merge, publish,
 distribute, sublicense, and/or sell copies of the Software, and to
 permit persons to whom the Software is furnished to do so, subject to
 the following conditions:

 The above copyright notice and this permission notice shall be included
 in all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include "Status.h"

Status status;

/*
 * Constructor
 */
Status::Status()
{
    systemState = init;
}

/*
 * Retrieve the current system state.
 */
Status::SystemState Status::getSystemState()
{
    return systemState;
}

/*
 * Set a new system state. The new system state is validated if the
 * transition is allowed from the old state. If an invalid transition is
 * attempted, the new state will be 'error'.
 * The old and new state are broadcast to all devices.
 */
Status::SystemState Status::setSystemState(SystemState newSystemState)
{
    if (systemState == newSystemState) {
        return systemState;
    }

    if (newSystemState == error) {
        systemState = error;
    } else {
        switch (systemState) {
        case init:
            if (newSystemState == ready) {
                systemState = newSystemState;
            }
            break;
        case ready:
            if (newSystemState == preHeat || newSystemState == running) {
                systemState = newSystemState;
            }
            break;
        case preHeat:
            if (newSystemState == running) {
                systemState = newSystemState;
            }
            break;
        case running:
            if (newSystemState == ready || newSystemState == shutdown) {
                systemState = newSystemState;
            }
            break;
        }
    }
    if (systemState == newSystemState) {
        Logger::info("switching to system state '%s'", systemStateToStr(systemState).c_str());
    } else {
        Logger::error("switching from system state '%s' to '%s' is not allowed", systemStateToStr(systemState).c_str(), systemStateToStr(newSystemState).c_str());
        systemState = error;
    }

    return systemState;
}

/*
 * Convert the current state into a string.
 */
String Status::systemStateToStr(SystemState state)
{
    switch (state) {
    case init:
        return "initializing";
    case ready:
        return "ready";
    case preHeat:
        return "pre-heating";
    case running:
        return "running";
    case error:
        return "error";
    case shutdown:
        return "shut-down";
    }
    Logger::error("the system state is invalid, contact your support!");
    return "invalid";
}
