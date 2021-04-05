/*
 * ThermalZone.cpp
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

#include "ThermalZone.h"

uint8_t ThermalZone::zoneCounter = 0;

ThermalZone::ThermalZone()
{
	id = zoneCounter++;

    pid = NULL;
    actualTemperature = -999;
    targetTemperature = 0;
    plateTemperature = 0;
    plateMaxTemperatureProgram = 0;
    plateTargetTemperature = 0;
    temperatureHigh = false;

    status.hiveTarget = 0;
    for (int i = 0; i < CFG_MAX_NUMBER_PLATES; i++) {
        status.hiveActual[i] = 0;
        status.plateActual[i] = 0;
        status.plateTarget[i] = 0;
        status.platePower[i] = 0;
        status.plateFanSpeed[i] = 0;
    }
}

ThermalZone::~ThermalZone()
{
    if (pid) {
        delete pid;
        pid = NULL;
    }
}

void ThermalZone::handleEvent(Event event, ...)
{
    switch (event) {
    case PROCESS:
        process();
        break;
    case PROGRAM_START:
    case PROGRAM_UPDATE:
        va_list args;
        va_start(args, event);
        programChange(va_arg(args, Program));
        va_end(args);
        break;
    case PROGRAM_STOP:
    case TEMPERATURE_ALERT:
        break;
    case PROGRAM_PAUSE:
        break;
    case PROGRAM_RESUME:
        break;
    }
}

void ThermalZone::initialize()
{
    initPid();
    eventHandler.subscribe(this);
}

void ThermalZone::process()
{
    actualTemperature = retrieveTemperature();
    status.hiveActual[id] = actualTemperature;

	int16_t plateTemp = calculatePlateTargetTemperature();
	for (SimpleList<Plate>::iterator itr = plates.begin(); itr != plates.end(); ++itr) {
		itr->setTargetTemperature(plateTemp);
	}

	if (actualTemperature > configuration.getParams()->hiveMaxTemp && !temperatureHigh) {
    	eventHandler.publish(TEMPERATURE_HIGH); // pause all heaters, activate fresh air vent
    	temperatureHigh = true;
    }
    if (actualTemperature > configuration.getParams()->hiveOverTemp) {
    	eventHandler.publish(TEMPERATURE_ALERT); // abort program?, alert !!
    }
    if (actualTemperature < configuration.getParams()->hiveMaxTemp && temperatureHigh) {
       	eventHandler.publish(TEMPERATURE_NORMAL);
       	temperatureHigh = false;
    }
}

void ThermalZone::programChange(const Program &program)
{
    Logger::info(F("Updating thermal zone with new program settings"));

    // adjust the PID which defines the target temperature of the plates based on the hive temp
    pid->SetOutputLimits((program.preHeat ? program.temperaturePreHeat : program.temperatureHive), program.temperaturePlate);
    pid->SetTunings(program.hiveKp, program.hiveKi, program.hiveKd);
    plateTargetTemperature = program.temperaturePlate;
    plateMaxTemperatureProgram = program.temperaturePlate;

    targetTemperature = (program.preHeat ? program.temperaturePreHeat : program.temperatureHive);
    status.hiveTarget = targetTemperature;
}


/**
 * Initialize the PID to define the plateTemperature based on the targetTemperature and the
 * actual temperature of the zone.
 */
void ThermalZone::initPid()
{
    pid = new PID(&actualTemperature, &plateTemperature, &targetTemperature, 0, 0, 0, DIRECT);
    pid->SetOutputLimits(0, configuration.getParams()->plateOverTemp);
    pid->SetSampleTime(CFG_LOOP_DELAY);
    pid->SetMode(AUTOMATIC);
}

/**
 * Retrieve temperature data from all assigned sensors and return highest value (in 0.1 deg C)
 */
int16_t ThermalZone::retrieveTemperature()
{
    int16_t max = -999;

    for (SimpleList<TemperatureSensor>::iterator sensor = temperatureSensors.begin(); sensor != temperatureSensors.end(); ++sensor) {
        sensor->retrieveData();
        if (sensor->getId() < CFG_MAX_NUMBER_PLATES)
            status.hiveActual[sensor->getId()] = sensor->getTemperatureCelsius();
        max = max(max, sensor->getTemperatureCelsius());
    }
    return max;
}

/**
 * Calculate the desired plate temperature based on the hive temperature and the current state. (in 0.1 deg C)
 */
int16_t ThermalZone::calculatePlateTargetTemperature()
{
    if (actualTemperature == -999) {
        return 0;
    }

    pid->Compute();

    // don't set directly as plateTemperature tends to jump. Dampen with 0.1 deg per second
    if (plateTemperature > plateTargetTemperature)
        plateTargetTemperature++;
    else if (plateTemperature < plateTargetTemperature)
        plateTargetTemperature--;

    plateTargetTemperature = constrain(plateTargetTemperature, 0, plateMaxTemperatureProgram);
    status.plateTarget[id] = plateTargetTemperature;

    return plateTargetTemperature;
}

void ThermalZone::addSensor(TemperatureSensor sensor)
{
	temperatureSensors.push_back(sensor);
}

void ThermalZone::addPlate(Plate plate)
{
	plates.push_back(plate);
}