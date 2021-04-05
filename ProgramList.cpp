/*
 * ProgramList.cpp
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

#include "ProgramList.h"

ProgramList programList;

ProgramList::ProgramList()
{
	initPrograms();
}

ProgramList::~ProgramList()
{
}

/**
 * Initialize all programs
 */
void ProgramList::initPrograms()
{
    Logger::info(F("loading program data"));

    Program programVarroaSummer;
    programVarroaSummer.running = false;
    programVarroaSummer.preHeat = false;
    programVarroaSummer.name = F("Varroa Killer");
    programVarroaSummer.temperaturePreHeat = 400; // 40 deg C
    programVarroaSummer.fanSpeedPreHeat = 250;
    programVarroaSummer.durationPreHeat = 60; // 60min
    programVarroaSummer.temperatureHive = 410; // 41 deg C
    programVarroaSummer.hiveKp = 4.0;
    programVarroaSummer.hiveKi = 0.2;
    programVarroaSummer.hiveKd = 7.0;
    programVarroaSummer.temperaturePlate = 800; // 70 deg C
    programVarroaSummer.plateKp = 1.0;
    programVarroaSummer.plateKi = 0.10;
    programVarroaSummer.plateKd = 7.0;
    programVarroaSummer.fanSpeed = 200; // minimum is 10
    programVarroaSummer.humidityMinimum = 30;
    programVarroaSummer.humidityMaximum = 35;
    programVarroaSummer.fanSpeedHumidifier = 100;
    programVarroaSummer.duration = 210; // 3.5 hours
    programs.push_back(programVarroaSummer);

    Program programVarroaWinter;
    programVarroaWinter.name = F("Winter Treat");
    programVarroaWinter.running = false;
    programVarroaWinter.preHeat = false;
    programVarroaWinter.temperaturePreHeat = 400; // 40 deg C
    programVarroaWinter.fanSpeedPreHeat = 255;
    programVarroaWinter.durationPreHeat = 60; // 60min
    programVarroaWinter.temperatureHive = 420; // 42.0 deg C
    programVarroaWinter.hiveKp = 8.0;
    programVarroaWinter.hiveKi = 0.2;
    programVarroaWinter.hiveKd = 5.0;
    programVarroaWinter.temperaturePlate = 850; // 75 deg C
    programVarroaWinter.plateKp = 4.0;
    programVarroaWinter.plateKi = 0.09;
    programVarroaWinter.plateKd = 50.0;
    programVarroaWinter.fanSpeed = 255; // minimum is 10
    programVarroaWinter.humidityMinimum = 30;
    programVarroaWinter.humidityMaximum = 35;
    programVarroaWinter.fanSpeedHumidifier = 100;
    programVarroaWinter.duration = 180; // 3 hours
    programs.push_back(programVarroaWinter);

    Program programCleaning;
    programCleaning.running = false;
    programCleaning.preHeat = false;
    programCleaning.name = F("Cleaning");
    programCleaning.temperaturePreHeat = 380; // 38 deg C
    programCleaning.fanSpeedPreHeat = 10;
    programCleaning.durationPreHeat = 0;
    programCleaning.temperatureHive = 425; // 42.5 deg C
    programCleaning.hiveKp = 8.0;
    programCleaning.hiveKi = 0.2;
    programCleaning.hiveKd = 5.0;
    programCleaning.temperaturePlate = 600; // 60 deg C
    programCleaning.plateKp = 4.0;
    programCleaning.plateKi = 0.09;
    programCleaning.plateKd = 50.0;
    programCleaning.fanSpeed = 10; // minimum is 10
    programCleaning.humidityMinimum = 1;
    programCleaning.humidityMaximum = 2;
    programCleaning.fanSpeedHumidifier = 0; // only works from 230 to 255
    programCleaning.duration = 15; // 15min
    programs.push_back(programCleaning);

    Program programMeltHoney;
    programMeltHoney.running = false;
    programMeltHoney.preHeat = false;
    programMeltHoney.name, F("Melt Honey");
    programMeltHoney.temperaturePreHeat = 300;
    programMeltHoney.fanSpeedPreHeat = 10;
    programMeltHoney.durationPreHeat = 0;
    programMeltHoney.temperatureHive = 300;
    programMeltHoney.hiveKp = 8.0;
    programMeltHoney.hiveKi = 0.2;
    programMeltHoney.hiveKd = 5.0;
    programMeltHoney.temperaturePlate = 500;
    programMeltHoney.plateKp = 4.0;
    programMeltHoney.plateKi = 0.09;
    programMeltHoney.plateKd = 50.0;
    programMeltHoney.fanSpeed = 10;
    programMeltHoney.humidityMinimum = 1;
    programMeltHoney.humidityMaximum = 2;
    programMeltHoney.fanSpeedHumidifier = 0; // only works from 230 to 255
    programMeltHoney.duration = 720; // 12 hours
    programs.push_back(programMeltHoney);
}

/**
 * Returns the list of all defined programs
 */
SimpleList<Program>* ProgramList::getPrograms()
{
    return &programs;
}
