/*
 * TemperatureSensor.h
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

#ifndef TEMPERATURESENSOR_H_
#define TEMPERATURESENSOR_H_

#include <Arduino.h>
#include <OneWire.h>
#include "Configuration.h"
#include "Logger.h"

class TemperatureSensor
{
public:
    enum DeviceType
    {
        UNKNOWN,
        DS18S20,
        DS18B20,
        DS1822
    };

    TemperatureSensor();
    TemperatureSensor(uint8_t index, bool plate);
    static void prepareData();
    static void resetSearch();
    static SensorAddress search();
    DeviceType getType();
    String getTypeStr();
    SensorAddress getAddress();
    void setAddress(SensorAddress sensorAddress);
    void setResolution(byte resolution);
    void retrieveData();
    int16_t getTemperatureCelsius();
    int16_t getTemperatureFahrenheit();
protected:

private:
    uint8_t index;
    SensorAddress address;
    DeviceType type;
    int16_t temperature; // integer representation of temperature
    static OneWire *ds;
};

#endif /* TEMPERATURESENSOR_H_ */
