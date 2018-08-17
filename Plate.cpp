/*
 * Plate.cpp
 *
 * Class which controls a single heating palte which consists of a heating element,
 * temperature sensor (to measure the plate's heat) and a fan to distribute the heat.
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

#include "Plate.h"

uint8_t Plate::activeHeaters = 0;

Plate::Plate() :
        Device()
{
    currentTemperature = 0.0;
    targetTemperature = 0.0;
    power = 0.0;
    maxPower = 0;
    index = 0;
    pid = NULL;
    sensorHeater = NULL;
    heater = NULL;
    fan = NULL;
}

void Plate::initialize()
{
    Device::initialize();

    maxPower = Configuration::getParams()->maxHeaterPower;

    pid = new PID(&currentTemperature, &power, &targetTemperature, 0, 0, 0, DIRECT);
    pid->SetOutputLimits(0, maxPower);
    pid->SetSampleTime(CFG_LOOP_DELAY);
    pid->SetMode(AUTOMATIC);
}

void Plate::initialize(uint8_t index)
{
    Logger::debug(F("initializing plate %d"), index + 1);

    initialize();
    this->index = index;
    sensorHeater = new TemperatureSensor(index, true);
    heater = new Heater(index);
    fan = new Fan(Configuration::getIO()->fan[index]);
    fan->setSpeed(Configuration::getParams()->minFanSpeed);
}

Plate::~Plate()
{
}

/**
 * Set the maximum temperature of the plate (in 0.1 deg C)
 */
void Plate::setTargetTemperature(int16_t temperature)
{
    targetTemperature = temperature;
}

int16_t Plate::getTargetTemperature()
{
    return targetTemperature;
}

/**
 * Set the maximum temperature of the plate (0-255)
 */
void Plate::setMaximumPower(uint8_t power)
{
    this->maxPower = constrain(power, (double ) 0, Configuration::getParams()->maxHeaterPower);
    pid->SetOutputLimits(0, this->maxPower);
}

uint8_t Plate::getMaximumPower()
{
    return maxPower;
}

/**
 * Set the plate's fan speed (0-255)
 */
void Plate::setFanSpeed(uint8_t speed)
{
    fan->setSpeed(speed);
    Status::getInstance()->fanSpeedPlate[index] = speed;
}

/**
 * Set the tuning parameters of the PID controller for the plate temperature
 */
void Plate::setPIDTuning(double kp, double ki, double kd)
{
    pid->SetTunings(kp, ki, kd);
}

/**
 * Get the temperature of the plate in 0.1 deg C
 */
int16_t Plate::getTemperature()
{
    return sensorHeater->getTemperatureCelsius();
}

/**
 * Get the applied power of the heater (0-255)
 */
uint8_t Plate::getPower()
{
    return heater->getPower();
}

/**
 * Get the current fan speed.
 */
uint8_t Plate::getFanSpeed()
{
    return fan->getSpeed();
}

/**
 * Get the plate's index.
 */
uint8_t Plate::getIndex()
{
    return index;
}

uint8_t Plate::calculateHeaterPower()
{
    bool wasOn = (power > Configuration::getParams()->maxHeaterPower / 2);

    pid->Compute(); // updates power
    if (currentTemperature > Configuration::getParams()->plateOverTemp) {
        Logger::error(F("ALERT !!! Plate %d is over-heating !!!"), index + 1);
        Status::getInstance()->setSystemState(Status::overtemp);
        Status::getInstance()->errorCode = Status::overtempPlate;
        power = 0;
    }

    if (Configuration::getParams()->usePWM) {
        return constrain(power, (double )0, maxPower);
    } else {
        if ((power > Configuration::getParams()->maxHeaterPower / 2) && (activeHeaters < Configuration::getParams()->maxConcurrentHeaters)) {
            activeHeaters++;
            return 255;
        } else {
            if (wasOn && activeHeaters > 0) {
                activeHeaters--;
            }
            return 0;
        }
    }
}

/**
 * Method called for each plate in the main loop.
 * It re-calculates the power and fan speed according to current
 * settings and sensor data.
 */
void Plate::process()
{
    Device::process();
    sensorHeater->retrieveData();
    currentTemperature = sensorHeater->getTemperatureCelsius();
    Status::getInstance()->temperaturePlate[index] = currentTemperature;

    uint8_t power = calculateHeaterPower();
    Status::getInstance()->powerPlate[index] = power;
    heater->setPower(power);

}
