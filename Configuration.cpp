/*
 * Configuration.cpp
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

#include "Configuration.h"


Configuration::Configuration()
{
}

Configuration::~Configuration()
{
}

/**
 * Return the instance of the singleton
 */
Configuration *Configuration::getInstance()
{
    static Configuration instance;
    return &instance;
}

/**
 * Load the configuration from EEPROM and verify the CRC values
 * as well as the existance of the ApiSauna token.
 *
 * If the token is not found, it is assumed that the configuration was never saved on this board and the config is re-set and saved.
 * If a CRC check fails, a error is printed to the log and HID and false is returned - causing the controller to go into error state.
 */
bool Configuration::load()
{
    Logger::info(F("loading configuration from EEPROM"));
    EEPROM.get(CONFIG_ADDRESS_IO, *getIO());
    EEPROM.get(CONFIG_ADDRESS_PARAMS, *getParams());
    EEPROM.get(CONFIG_ADDRESS_SENSOR, *getSensor());

    if (getParams()->token != CFG_EEPROM_CONFIG_TOKEN) {
        Logger::warn(F("no ApiSauna token found in EEPROM --> resetting configuration"));
        reset();
        save();
    }

    if (getParams()->crc != crc((uint8_t *)getParams() + 4, sizeof(ConfigurationParams) - 4)) {
        Logger::error(F("invalid crc detected in parameter configuration"));
        return false;
    }
    if (getIO()->crc != crc((uint8_t *)getIO() + 4, sizeof(ConfigurationIO) - 4)) {
        Logger::error(F("invalid crc detected in I/O configuration"));
        return false;
    }
    if (getSensor()->crc != crc((uint8_t *)getSensor() + 4, sizeof(ConfigurationSensor) - 4)) {
        Logger::error(F("invalid crc detected in sensor configuration"));
        return false;
    }

    if (getParams()->numberOfPlates > CFG_MAX_NUMBER_PLATES) {
        getParams()->numberOfPlates = CFG_MAX_NUMBER_PLATES;
    }

    return true;
}

/**
 * Calculate and set the CRC values of all configurations.
 */
void Configuration::updateCrc()
{
    getParams()->crc = crc((uint8_t*) (getParams()) + 4, sizeof(ConfigurationParams) - 4);
    getIO()->crc = crc((uint8_t*) (getIO()) + 4, sizeof(ConfigurationIO) - 4);
    getSensor()->crc = crc((uint8_t*) (getSensor()) + 4, sizeof(ConfigurationSensor) - 4);
}

/**
 * Re-calc the CRC values and store all configuration blocks to EEPROM.
 */
void Configuration::save()
{
    Logger::info(F("saving configuration to EEPROM"));

    updateCrc();
    EEPROM.put(CONFIG_ADDRESS_IO, *getIO());
    EEPROM.put(CONFIG_ADDRESS_PARAMS, *getParams());
    EEPROM.put(CONFIG_ADDRESS_SENSOR, *getSensor());

    Logger::info(F("done"));
}

/**
 * Reset the configuration to default values and update the CRCs.
 */
void Configuration::reset()
{
    Logger::info(F("resetting configuration to default values"));

    ConfigurationIO *configIO = getIO();
    ConfigurationParams *configParams = getParams();
    ConfigurationSensor *configSensor = getSensor();

    configParams->token = CFG_EEPROM_CONFIG_TOKEN;
    configParams->version = 1;

    for (int i = 0; i < CFG_MAX_NUMBER_PLATES; i++) {
        configIO->heater[i] = 0;
        configIO->fan[i] = 0;
        configSensor->addressPlate[i].value = 0;
        configSensor->addressHive[i].value = 0;
    }

    configIO->heartbeat = 13; //13 is L, 73 is TX, 72 is RX
    configIO->temperatureSensor = 4;
    configIO->humiditySensor = 9;
    configIO->humiditySensorType = 22;
    configIO->vaporizer = 5;
    configIO->humidifierFan = 6;
    configIO->heaterRelay = A0;
    configIO->heater[0] = 11;
    configIO->heater[1] = 12;
    configIO->heater[2] = 2;
    configIO->heater[3] = 3;
    configIO->fan[0] = 7;
    configIO->fan[1] = 8;
    configIO->fan[2] = 44;
    configIO->fan[3] = 45;
    configIO->buttonLeft = 11;
    configIO->buttonRight = 12;
    configIO->buttonUp = 2;
    configIO->buttonDown = 3;
    configIO->buttonSelect = 13;
    configIO->beeper = 10;
    configIO->lcdRs = 22;
    configIO->lcdEnable = 23;
    configIO->lcdD0 = 24;
    configIO->lcdD1 = 25;
    configIO->lcdD2 = 26;
    configIO->lcdD3 = 27;

    configParams->numberOfPlates = 4;
    configParams->maxHeaterPower = 170;
    configParams->minFanSpeed = 10;
    configParams->hiveOverTemp = 460;
    configParams->hiveOverTempRecover = 350;
    configParams->plateOverTemp = 850;
    configParams->usePWM = 0;
    configParams->maxConcurrentHeaters = 2;

//TODO remove this, it's only temporary to ease testing
    configSensor->addressPlate[0].value = 0x3d0516a4f187ff28;
    configSensor->addressPlate[1].value = 0x9a0516a50124ff28;
    configSensor->addressPlate[2].value = 0xe10316a5188cff28;
    configSensor->addressPlate[3].value = 0xed0316a48290ff28;
    configSensor->addressHive[0].value = 0xc00416a4cf26ff28;
    configSensor->addressHive[1].value = 0x1f0115a86b01ff28;
    configSensor->addressHive[2].value = 0x4d0416a4d157ff28;
    configSensor->addressHive[3].value = 0xf70315a86f2fff28;

    updateCrc();
}

/*
 * Returns the singleton instance of the I/O configuration.
 */
ConfigurationIO *Configuration::getIO()
{
    static ConfigurationIO configIO;
    return &configIO;
}

/*
 * Returns the singleton instance of the parameter configuration.
 */
ConfigurationParams *Configuration::getParams()
{
    static ConfigurationParams configParams;
    return &configParams;
}

/*
 * Returns the singleton instance of the sensor configuration.
 */
ConfigurationSensor *Configuration::getSensor()
{
    static ConfigurationSensor configSensor;
    return &configSensor;
}

/**
 * Calculate the CRC of a memory area defined by a location and its size.
 */
uint32_t Configuration::crc(uint8_t *location, uint32_t size)
{
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (uint8_t *pos = location; pos < location + size  ; ++pos) {
    crc = crc_table[(crc ^ *pos) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (*pos >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
  }
  return crc;
}