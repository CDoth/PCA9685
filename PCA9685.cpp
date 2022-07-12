#include "PCA9685.h"
#include <cmath>
#include <stdio.h>


uint8_t getPreScaleValue(int freq) {
    return (std::round(OCSFREQ / (PCA_RESOLUTION * freq)) - 1);
}
float ms2hz(int ms) {
    return 1000.0f / float(ms);
}

/*
PCA9685Controller::PCA9685Controller() {
    clearAll();
}
PCA9685Controller::~PCA9685Controller() {
    clearAll();
}
int PCA9685Controller::open() {
    handle = DI2C::openI2CDevice("/dev/i2c-0", PCA9685_DEFAULT_ADDRESS);
    return handle;
}
int PCA9685Controller::open(const char *deviceName, DI2C::DI2CRegister address) {
    handle = DI2C::openI2CDevice(deviceName, address);
    return handle;
}
int16_t PCA9685Controller::signalTactWidth() const {
    return std::abs(PWMOFF_tacts -PWMON_tacts);
}
int PCA9685Controller::signalWidth() const {
    return signalWidth_ms;
}
int PCA9685Controller::mainWidth() const {
    return mainWidth_ms;
}
int PCA9685Controller::initLed(int __ledIndex) {
    if(ledIndex < 0 || ledIndex > 15) {
        return -1;
    }
    ledIndex = __ledIndex;

    LEDON_L = ledIndex + 6 + 0;
    LEDON_H = ledIndex + 6 + 1;
    LEDOFF_L = ledIndex + 6 + 2;
    LEDOFF_H = ledIndex + 6 + 3;

    return 0;
}

int PCA9685Controller::preparePCA() {
    return writeRegister(MODE1, 0);

}
int PCA9685Controller::setMainFreq(float frequencyInHertzs) {

    if(frequencyInHertzs <= 0) {
        return -1;
    }
    mainFreq_hz = frequencyInHertzs;
    mainWidth_ms = 1000 / mainFreq_hz;

    auto value = getPreScaleValue(frequencyInHertzs);

    printf("PCA9685Controller::setMainFreq: frequencyInHertzs: [%f] value: [%d]\n",
           frequencyInHertzs, value);

    return writeRegister(PRE_SCALE, value);
}
int PCA9685Controller::setMainFreqByTotalWidth(float widthInMilliseconds) {
    if(widthInMilliseconds <= 0) {
        return -1;
    }

    printf("PCA9685Controller::setMainFreqByTotalWidth: ms: [%f]\n",
           widthInMilliseconds);

    return setMainFreq(ms2hz(widthInMilliseconds));
}
int PCA9685Controller::setPWMStartByTact(int tact) {
    if(tact < 0) {
        return -1;
    }
    if(tact > 4095) {
        tact = tact % 4095;
    }
    updateSignalWidth();
    return setLedOn(tact);
}
int PCA9685Controller::setPWMStartByPart(float part) {
    if(part < 0.0f) {
        return -1;
    }
    return setPWMStartByTact(PCA_RESOLUTION * part);
}
int PCA9685Controller::setPWMStartByMilliseconds(float ms) {
    return setPWMStartByTact(PCA_RESOLUTION * (ms/mainWidth_ms));
}
int PCA9685Controller::setPWMWidthByTacts(int tacts) {
    tacts += PWMON_tacts;
    if(tacts > PCA_RESOLUTION) {
        tacts = PCA_RESOLUTION % tacts;
    }
    signalWidth_ms = mainWidth_ms * (tacts / PCA_RESOLUTION);
    printf("setPWMWidthByTacts: [%d]\n", tacts);
    return setLedOff(tacts);
}
int PCA9685Controller::setPWMWidthByPartOfMain(int partInPercent) {
    if(partInPercent < 0) partInPercent = 0;
    if(partInPercent > 100) partInPercent = 100;
    int16_t width = 4095.0f * float(partInPercent) / 100.0f;
    return setPWMWidthByTacts(width);
}
int PCA9685Controller::setPWMWidthByMilliseconds(float ms) {

    printf("setPWMWidthByMilliseconds: ms: [%f] width: [%f]\n", ms, mainWidth_ms);

    if(ms < 0 || ms > mainWidth_ms) {
        return -1;
    }
    return setPWMWidthByTacts(float(PCA_RESOLUTION) * (ms / mainWidth_ms));
}
int PCA9685Controller::increasePWMWidth(int tacts) {
    tacts += PWMOFF_tacts;
    if(tacts > PCA_RESOLUTION) {
        tacts = PCA_RESOLUTION % tacts;
    }
    return setLedOff(tacts);
}
int PCA9685Controller::decreasePWMWidth(int tacts) {
    tacts = PWMOFF_tacts - tacts;
    if(tacts < PWMON_tacts) {
        tacts = PWMON_tacts;
    }
    return setLedOff(tacts);
}
int PCA9685Controller::increasePWMWidthByMilliseconds(int ms) {
    auto w = signalWidth_ms + ms;
    if(w < 0 || w > mainWidth_ms) {
        return -1;
    }
    return setPWMWidthByMilliseconds(w);
}
int PCA9685Controller::decreasePWMWidthByMilliseconds(int ms) {
    auto w = signalWidth_ms - ms;
    if(w < 0 || w > mainWidth_ms) {
        return -1;
    }
    return setPWMWidthByMilliseconds(w);
}
int PCA9685Controller::pwmOff() {
    return setPWMWidthByTacts(0);
}

void PCA9685Controller::clearAll() {
    mainFreq_hz = 0.0f;
    mainWidth_ms = 0.0f;
    ledIndex = 0;
    PWMOFF_tacts = 0;
    PWMON_tacts = 0;
    LEDON_L = 0;
    LEDON_H = 0;
    LEDOFF_L = 0;
    LEDOFF_H = 0;
    handle = 0;
}
void PCA9685Controller::updateSignalWidth() {
    signalWidth_ms = mainWidth_ms * (signalTactWidth() / PCA_RESOLUTION);
}
int PCA9685Controller::setLedOn(int16_t value) {
    PCA9685_DATA data;
    data.value = value;
    return setLedOn(data);
}
int PCA9685Controller::setLedOn(PCA9685_DATA data) {
    if( writeRegister(LEDON_L, data.low) < 0 ) {
        return -1;
    }
    if( writeRegister(LEDON_H, data.high) < 0 ) {
        return -1;
    }
    return 0;
}
int PCA9685Controller::setLedOff(int16_t value) {
    PCA9685_DATA data;
    data.value = value;
    return setLedOff(data);
}
int PCA9685Controller::setLedOff(PCA9685_DATA data) {
    if( writeRegister(LEDOFF_L, data.low) < 0 ) {
        return -1;
    }
    if( writeRegister(LEDOFF_H, data.high) < 0 ) {
        return -1;
    }

    printf("OFF L: [%d] OFF H: [%d] h: [%d] d: [%d]\n", LEDOFF_L, LEDOFF_H, handle, (int)data.value);
    return 0;
}
int PCA9685Controller::writeRegister(register_t registerAddress, uint8_t data) {

    printf("writeRegister: handle: [%d] addr: [%d] data: [%d]\n", handle, registerAddress, (int)data);
    DI2C::DI2CWrite(handle, registerAddress, data);
    return 0;
}
int PCA9685Controller::readRegister(register_t registerAddress, uint8_t &data) {

    DI2C::DI2CRead(handle, registerAddress, data);
    return 0;
}
*/


//========================================================================= LED
PCA9685Led::PCA9685Led() {
    clearAll();
}
int PCA9685Led::signalTactRange() const {
    return std::abs(PWMOFF_tacts - PWMON_tacts);
}
int PCA9685Led::setPWMWidthByTacts(int tacts) {

    tacts += PWMON_tacts;
    if(tacts > PCA_RESOLUTION) {
        tacts = PCA_RESOLUTION % tacts;
    }
    signalWidth_ms = master->mainWidth_ms * (tacts / PCA_RESOLUTION);

//    printf("PCA9685Led::setPWMWidthByTacts: [%d] ON: [%d] OFF: [%d] \n", tacts,
//           PWMON_tacts, PWMOFF_tacts);

    return setLedOff(tacts);
}
void PCA9685Led::init(PCA9685Master *m, int index) {

    master = m;
    LEDON_L = (index * 4) + 6 + 0;
    LEDON_H = (index * 4) + 6 + 1;
    LEDOFF_L = (index * 4) + 6 + 2;
    LEDOFF_H = (index * 4) + 6 + 3;

//    printf("PCA9685Led::init: index: [%d] ONL: [%d] ONH: [%d] "
//           "OFFL: [%d] OFFH: [%d]\n", index, LEDON_L, LEDON_H,
//           LEDOFF_L, LEDOFF_H);

}
//------------
void PCA9685Led::clearAll() {

    signalWidth_ms = 0.0f;
    PWMOFF_tacts = 0;
    PWMON_tacts = 0;
    LEDON_L = 0;
    LEDON_H = 0;
    LEDOFF_L = 0;
    LEDOFF_H = 0;
    master = nullptr;
}
void PCA9685Led::updateSignalWidth() {
    signalWidth_ms = master->mainWidth_ms * (signalTactRange() / PCA_RESOLUTION);
}
int PCA9685Led::setLedOn(int16_t value) {
    PCA9685_DATA data;
    data.value = value;
    return setLedOn(data);
}
int PCA9685Led::setLedOn(PCA9685_DATA data) {
    PWMON_tacts = data.value;
    if( writeRegister(LEDON_L, data.low) < 0 ) {
        return -1;
    }
    if( writeRegister(LEDON_H, data.high) < 0 ) {
        return -1;
    }

//    printf("PCA9685Led::setLedOn: value: [%d]\n", data.value);
    return 0;
}
int PCA9685Led::setLedOff(int16_t value) {
    PCA9685_DATA data;
    data.value = value;
    return setLedOff(data);
}
int PCA9685Led::setLedOff(PCA9685_DATA data) {
    PWMOFF_tacts = data.value;
    if( writeRegister(LEDOFF_L, data.low) < 0 ) {
        return -1;
    }
    if( writeRegister(LEDOFF_H, data.high) < 0 ) {
        return -1;
    }
    printf("PCA9685Led::setLedOff: value: [%d] REGL: [%d] REGH: [%d]\n",
           data.value, LEDOFF_L, LEDOFF_H);
    return 0;
}
int PCA9685Led::writeRegister(register_t registerAddress, uint8_t data) {
    DI2C::DI2CWrite(master->handle, registerAddress, data);
    return 0;
}
int PCA9685Led::readRegister(register_t registerAddress, uint8_t &data) {
    DI2C::DI2CRead(master->handle, registerAddress, data);
    return 0;
}
//========================================================================= MASTER
PCA9685Master::PCA9685Master() {
    clearAll();
    printf("PCA9685Master::create: [%p]\n", this);
    for(int i=0;i!=16;++i) {
        LEDS[i].init(this, i);
    }
}
int PCA9685Master::open() {
    handle = DI2C::openI2CDevice("/dev/i2c-0", PCA9685_DEFAULT_ADDRESS);

    printf("PCA9685Master::open: [%d]\n", handle);
    writeRegister(MODE1, 0);
    return handle;
}
int PCA9685Master::open(const char *deviceName, DI2C::DI2CRegister address) {
    handle = DI2C::openI2CDevice(deviceName, address);
    writeRegister(MODE1, 0);
    return handle;
}
//------------------------------------------------
int PCA9685Master::setPWMStartByTact(int tact, int ledIndex) {
    if(tact < 0) {
        return -1;
    }
    if(tact > 4095) {
        tact = tact % 4095;
    }
    if(ledIndex < 0) {
        for(int i=0;i!=16;++i) {

//            printf("PCA9685Master::setPWMStartByTact: [%d] \n", i);
            LEDS[i].setLedOn(tact);
            LEDS[i].updateSignalWidth();
        }
    } else {
        LEDS[ledIndex].setLedOn(tact);
        LEDS[ledIndex].updateSignalWidth();
    }

    return 0;
}
int PCA9685Master::setPWMStartByPart(float part, int ledIndex) {
    if(part < 0.0f) {
        return -1;
    }
    return setPWMStartByTact(PCA_RESOLUTION * part, ledIndex);
}
int PCA9685Master::setPWMStartByMilliseconds(float ms, int ledIndex) {
    return setPWMStartByTact(PCA_RESOLUTION * (ms/mainWidth_ms), ledIndex);
}
//------------------------------------------------
int PCA9685Master::setPWMWidthByTacts(int tacts, int ledIndex) {
    if(ledIndex < 0 || ledIndex > 15) {
        return -1;
    }
    printf("PCA9685Master::setPWMWidthByTacts: tacts: [%d] index: [%d]\n",
           tacts, ledIndex);

    return LEDS[ledIndex].setPWMWidthByTacts(tacts);
}
int PCA9685Master::setPWMWidthByTacts(int tacts) {

    for(int i=0;i!=16;++i) {
        LEDS[i].setPWMWidthByTacts(tacts);
    }

    return 0;
}
int PCA9685Master::setPWMWidthByPartOfMain(int partInPercent, int ledIndex) {

    if(ledIndex < 0 || ledIndex > 15) {
        return -1;
    }
    if(partInPercent < 0) partInPercent = 0;
    if(partInPercent > 100) partInPercent = 100;
    int16_t width = 4095.0f * float(partInPercent) / 100.0f;
    return setPWMWidthByTacts(width, ledIndex);
}
int PCA9685Master::setPWMWidthByPartOfMain(int partInPercent) {

    if(partInPercent < 0) partInPercent = 0;
    if(partInPercent > 100) partInPercent = 100;
    int16_t width = 4095.0f * float(partInPercent) / 100.0f;
    for(int i=0;i!=16;++i) {
        LEDS[i].setPWMWidthByTacts(width);
    }

    return 0;
}
int PCA9685Master::setPWMWidthByMilliseconds(float ms, int ledIndex) {


    if(ms < 0 || ms > mainWidth_ms) {
        return -1;
    }
    if(ledIndex < 0 || ledIndex > 15) {
        return -1;
    }
    return setPWMWidthByTacts(float(PCA_RESOLUTION) * (ms / mainWidth_ms), ledIndex);

}
int PCA9685Master::setPWMWidthByMilliseconds(float ms) {

    int16_t width = float(PCA_RESOLUTION) * (ms / mainWidth_ms);
    for(int i=0;i!=16;++i) {
        LEDS[i].setPWMWidthByTacts(width);
    }
}
//------------------------------------------------
int PCA9685Master::setMainFreq(float frequencyInHertzs) {
    if(frequencyInHertzs <= 0) {
        return -1;
    }
    mainFreq_hz = frequencyInHertzs;
    mainWidth_ms = 1000 / mainFreq_hz;

    auto value = getPreScaleValue(frequencyInHertzs);

    printf("PCA9685Master::setMainFreq: hz: [%f] value: [%d]\n",
           frequencyInHertzs, value);

    return writeRegister(PRE_SCALE, value);
}
int PCA9685Master::setMainFreqByTotalWidth(int widthInMilliseconds) {

    if(widthInMilliseconds <= 0) {
        return -1;
    }

    float hz = ms2hz(widthInMilliseconds);
    printf("PCA9685Master::setMainFreqByTotalWidth: ms: [%d] hz: [%f]\n",
           widthInMilliseconds, hz);
    return setMainFreq(hz);
}
//-------------------------------------------------------
int PCA9685Master::writeRegister(register_t registerAddress, uint8_t data) {
    DI2C::DI2CWrite(handle, registerAddress, data);
    return 0;
}
int PCA9685Master::readRegister(register_t registerAddress, uint8_t &data) {

    DI2C::DI2CRead(handle, registerAddress, data);
    return 0;
}
void PCA9685Master::clearAll() {

    mainFreq_hz = 0.0f;
    mainWidth_ms = 0.0f;
    handle = 0;
}
//------------------------------------------------
//=========================================================================


