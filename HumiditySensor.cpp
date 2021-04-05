/*
 * HumiditySensor.cpp
 *
 * Read the data from the humidity sensor.
 *
 Copyright (c) 2017-2021 Michael Neuweiler

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

#include "HumiditySensor.h"

HumiditySensor::HumiditySensor() {
	dht = NULL;
}

HumiditySensor::~HumiditySensor() {
	if (dht != NULL) {
		delete dht;
		dht = NULL;
	}
}

void HumiditySensor::initialize() {
	logger.info(F("initializing humidity sensor"));
	if (dht != NULL) {
		delete dht;
	}
	dht = new DHT(configuration.getIO()->humiditySensor, configuration.getIO()->humiditySensorType);
	dht->begin();
}

/**
 * Get relative humidity in %
 */
uint8_t HumiditySensor::getRelativeHumidity() {
	if (dht == NULL || millis() < 10000) {
		return 99;
	}
	return (int) dht->readHumidity();
}

/**
 * Get temperature in 0.1 deg C
 */
int16_t HumiditySensor::getTemperature() {
	if (dht == NULL) {
		return 0;
	}
	return dht->readTemperature() * 10;
}
