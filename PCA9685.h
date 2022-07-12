#include <DI2C.h>

#define PCA9685_DEFAULT_ADDRESS 0x40


#define LED0_ON_L 0x06
#define LED0_ON_H 0x07
#define LED0_OFF_L 0x08
#define LED0_OFF_H 0x09

#define MODE1 0x00
#define MODE2 0x01
#define PRE_SCALE 0xFE

#define ALL_LED_ON_L 0xFA
#define ALL_LED_ON_H 0xFB
#define ALL_LED_OFF_L 0xFC
#define ALL_LED_OFF_H 0xFD
#define OCSFREQ (25000000)
#include <stdint.h>

#define PCA_RESOLUTION (4096)


/*
 How to start:

 //Create object:
 PCA9685Controller c;

 //Open I2C interface of PCA9685:
 c.open(); //or c.open("devName", devAddress);

 // Init output PCA9685 port (0 here)
 c.initLed(0); // 0 mean first port

 // Set pulse start:
 c.setPWMStartByTact(0);

 // Set main freq:
 c.setMainFreqByTotalWidth(20);

 // Call preparePCA:
 c.preparePCA();

 // Set pulse width
 c.setPWMWidthByMilliseconds(0.0);
*/
typedef int register_t;

typedef union {
    int16_t value;
    struct {
        uint8_t low;
        uint8_t high;
    };
}PCA9685_DATA;

uint8_t getPreScaleValue(int freq);
float ms2hz(int ms);

/*
class PCA9685Controller {
public:
    PCA9685Controller();
    ~PCA9685Controller();

    int open();
    int open(const char *deviceName, DI2C::DI2CRegister address);


    int frequency() const {return mainFreq_hz;}
    int16_t signalTactWidth() const;
    ///
    /// \brief signalWidth Signal width in milliseconds
    /// \return
    ///
    int signalWidth() const;
    int16_t mainTactWidth() const {return PCA_RESOLUTION;}
    ///
    /// \brief mainWidth Width of each 4096 tacts in milliseconds
    /// \return
    ///
    int mainWidth() const;
    ///
    /// \brief led 0 - 15
    /// \return
    ///
    int led() const {return ledIndex;}
    //------------------------------------------------
    int initLed(int ledIndex);
    int preparePCA();
    //------------------------------------------------
    ///
    /// \brief setMainFreq set frequency of each 4096 tacts by frequencyInHertzs
    ///
    int setMainFreq(float frequencyInHertzs);
    ///
    /// \brief setMainFreqByTotalWidthset frequency of each 4096 tacts to ( 1000 / widthInMilliseconds )
    ///
    int setMainFreqByTotalWidth(float widthInMilliseconds);
    //------------------------------------------------
    ///
    /// PWM start is tact when main signal turn on. This is shift of signal.

    ///
    /// \brief setPWMStartByTacts Set signal shift to 'tact'. Vale can be more than 4096, shift will be set to (tact % 4096)
    ///
    int setPWMStartByTact(int tact);
    ///
    /// \brief setPWMStartByPart Set signal shift to (part * PCA_RESOLUTION)
    ///
    int setPWMStartByPart(float part);
    ///
    /// \brief setPWMStartByMilliseconds Set signal shift equal 'ms'
    ///
    int setPWMStartByMilliseconds(float ms);
    //------------------------------------------------
    ///
    /// \brief setPWMWidthByTacts
    /// \param tacts
    /// \return
    ///
    int setPWMWidthByTacts(int tacts);
    ///
    /// \brief setPWMWidthByPartOfMain
    /// \param partInPercent
    /// \return
    ///
    int setPWMWidthByPartOfMain(int partInPercent);
    ///
    /// \brief setPWMWidthByMilliseconds
    /// \param ms
    /// \return
    ///
    int setPWMWidthByMilliseconds(float ms);
    //------------------------------------------------
    int increasePWMWidth(int tacts = 1);
    int decreasePWMWidth(int tacts = 1);
    int increasePWMWidthByMilliseconds(int ms);
    int decreasePWMWidthByMilliseconds(int ms);

    int pwmOff();
//    int pwmOn();
    //------------------------------------------------

private:
    void clearAll();
    void updateSignalWidth();
    int setLedOn(int16_t value);
    int setLedOn(PCA9685_DATA data);
    int setLedOff(int16_t value);
    int setLedOff(PCA9685_DATA data);
    int writeRegister(register_t registerAddress, uint8_t data);
    int readRegister(register_t registerAddress, uint8_t &data);
private:
    ///
    /// \brief mainFreq
    /// freq of each 4096 tacts in hz
    ///
    float mainFreq_hz;
    ///
    /// \brief mainLenght
    /// Time lenght in milliseconds of each 4096 tacts
    float mainWidth_ms;

    ///
    /// \brief signalWidth
    /// Width in milliseconds of main signal
    float signalWidth_ms;


    ///
    /// \brief ledIndex
    /// 0 - 15
    int ledIndex;
    ///
    /// \brief PWMOFF
    /// Store index of tact (0-4095) when signal turn on
    int16_t PWMOFF_tacts;
    ///
    /// \brief PWMON
    /// Store index of tact (0-4095) when signal torn off
    int16_t PWMON_tacts;

    ///
    /// \brief LEDON_L LEDON_H LEDOFF_L LEDOFF_H
    /// Main width control
    register_t LEDON_L;
    register_t LEDON_H;
    register_t LEDOFF_L;
    register_t LEDOFF_H;


    DI2C::DI2CHandle handle;
};
*/

class PCA9685Master;
class PCA9685Led {
public:
    friend class PCA9685Master;
    PCA9685Led();

    int signalTactRange() const;
private:
    int setPWMWidthByTacts(int tacts);
    void init(PCA9685Master *m, int index);

private:
    void clearAll();
    void updateSignalWidth();
    int setLedOn(int16_t value);
    int setLedOn(PCA9685_DATA data);
    int setLedOff(int16_t value);
    int setLedOff(PCA9685_DATA data);
    int writeRegister(register_t registerAddress, uint8_t data);
    int readRegister(register_t registerAddress, uint8_t &data);
private:
    ///
    /// \brief signalWidth
    /// Width in milliseconds of main signal
    float signalWidth_ms;
    ///
    /// \brief PWMOFF
    /// Store index of tact (0-4095) when signal turn on
    int16_t PWMOFF_tacts;
    ///
    /// \brief PWMON
    /// Store index of tact (0-4095) when signal torn off
    int16_t PWMON_tacts;
    ///
    /// \brief LEDON_L LEDON_H LEDOFF_L LEDOFF_H
    /// Main width control
    register_t LEDON_L;
    register_t LEDON_H;
    register_t LEDOFF_L;
    register_t LEDOFF_H;
private:
    PCA9685Master *master;

};
class PCA9685Master {
public:
    friend class PCA9685Led;
    PCA9685Master();
    int open();
    int open(const char *deviceName, DI2C::DI2CRegister address);
    int preparePCA();
    //------------------------------------------------
    ///
    /// PWM start is tact when main signal turn on. This is shift of signal.

    ///
    /// \brief setPWMStartByTacts Set signal shift to 'tact'.
    ///  Value can be more than 4096, shift will be set to (tact % 4096)
    ///
    int setPWMStartByTact(int tact, int ledIndex = -1);
    ///
    /// \brief setPWMStartByPart Set signal shift to (part * PCA_RESOLUTION)
    ///
    int setPWMStartByPart(float part, int ledIndex = -1);
    ///
    /// \brief setPWMStartByMilliseconds Set signal shift equal 'ms'
    ///
    int setPWMStartByMilliseconds(float ms, int ledIndex = -1);
    //------------------------------------------------
    ///
    /// \brief setPWMWidthByTacts
    /// \param tacts
    /// \return
    ///
    int setPWMWidthByTacts(int tacts, int ledIndex);
    int setPWMWidthByTacts(int tacts);
    ///
    /// \brief setPWMWidthByPartOfMain
    /// \param partInPercent
    /// \return
    ///
    int setPWMWidthByPartOfMain(int partInPercent, int ledIndex);
    int setPWMWidthByPartOfMain(int partInPercent);
    ///
    /// \brief setPWMWidthByMilliseconds
    /// \param ms
    /// \return
    ///
    int setPWMWidthByMilliseconds(float ms, int ledIndex);
    int setPWMWidthByMilliseconds(float ms);
    //------------------------------------------------
    ///
    /// \brief setMainFreq set frequency of each 4096 tacts by frequencyInHertzs
    ///
    int setMainFreq(float frequencyInHertzs);
    ///
    /// \brief setMainFreqByTotalWidthset frequency of each 4096 tacts to ( 1000 / widthInMilliseconds )
    ///
    int setMainFreqByTotalWidth(int widthInMilliseconds);
private:
    int writeRegister(register_t registerAddress, uint8_t data);
    int readRegister(register_t registerAddress, uint8_t &data);
private:
    void clearAll();
private:
    ///
    /// \brief mainFreq
    /// freq of each 4096 tacts in hz
    ///
    float mainFreq_hz;
    ///
    /// \brief mainLenght
    /// Time lenght in milliseconds of each 4096 tacts
    float mainWidth_ms;

    DI2C::DI2CHandle handle;

    PCA9685Led LEDS[16];
};

