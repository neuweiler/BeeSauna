/*
 * SerialConsole.h
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

#ifndef SERIALCONSOLE_H_
#define SERIALCONSOLE_H_

#include <Arduino.h>
#include "Logger.h"
#include "Device.h"
#include "ProgramHandler.h"

class SerialConsole: Device
{
public:
    SerialConsole();
    void process();
    void printMenu();

private:
    char cmdBuffer[CFG_SERIAL_BUFFER_SIZE + 1];
    int ptrBuffer;

    bool handleShortCmd();
    bool handleCmd();
    bool handleCmdSystem(String &command, int32_t value);
    bool handleCmdParams(String &command, int32_t value);
    bool handleCmdSensor(String &command, char *cmdBuffer);
    bool handleCmdIO(String &command, int32_t value);
    bool handleCmdProgram(String &command, int32_t value);
    uint8_t getIndex(String command);
    void printMenuParams();
    void printMenuSensors();
    void printMenuIO();
    void printMenuProgram();
};

#endif /* SERIALCONSOLE_H_ */
