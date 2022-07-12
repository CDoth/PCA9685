# PCA9685
PCA9685 is PWM controller with I2C interface. This project provide to manage PCA9685 from computer with I2C (in my case OrangePi with GPIO).

## Start 
Before using PCA9685 call this:
```
    int PCA9685Master::open(const char *deviceName, DI2C::DI2CRegister address);
    int PCA9685Master::preparePCA();
```
Each iteration of PCA9685 contain 4096 tacts. Each pulse describes by its start point and its width. Start point is a tact (one of 4096)
when pulse become logic one. Time of each iteration (each 4096 tacts) is tunable. You have to set main frequency of each 4096 tacts:
```
    int setMainFreq(float frequencyInHertzs); // 
    int setMainFreqByTotalWidth(int widthInMilliseconds);
```
If you call `setMainFreqByTotalWidth(20)` this mean PCA9685 need 20 ms to run 4096 tacts (from 0 to 4095).

To set start point use:
```
    int PCA9685Master::setPWMStartByTact(int tact, int ledIndex = -1); // set one of 4096 tact to start
    int PCA9685Master::setPWMStartByPart(float part, int ledIndex = -1); // set percent of 4096 tacts, this tact will be start tact
    int PCA9685Master::setPWMStartByMilliseconds(float ms, int ledIndex = -1); // set ms where "ms >=0 && ms < main_frequency"
```
To set pulse width use:
```
    int setPWMWidthByTacts(int tacts, int ledIndex);
    int setPWMWidthByTacts(int tacts);
    
    int setPWMWidthByPartOfMain(int partInPercent, int ledIndex);
    int setPWMWidthByPartOfMain(int partInPercent);

    int setPWMWidthByMilliseconds(float ms, int ledIndex);
    int setPWMWidthByMilliseconds(float ms);
```

My description of PCA9685 on russian:
```
1. Управление PCA9685 для генерации ШИМ сигнала.
Регистр PRE_SCALE (254, FE) хранит 8 битное значение главной частоты контроллера. 
Записывая значение в данный регистр можно урпавлять частотой ШИМ сигнала.
Главный период равен 4096 тактам, реальная длина этого периода задается с помощью 
регистра PRE_SCALE. 
Формула для рассчета значения, которое нужно записать в PRE_SCALE что бы получить частоту X:

PRE_SCALE = round( ocs / (4096 * f) ) - 1 = X
Где osc частота осцилятора, по дефолту - 25МГц (в формуле 25000000).
f - требуемая частота, например 50 герц (20 миллисекунд на главный период).
Частота в 50 герц означает, что 4096 тактов будут проходить за 20 миллисекунд.
Тогда, что бы задать это значение - используем формулу:
round( 25000000 / (4096 * 50) ) - 1 = 122
Значение 122 нужно записать в PRE_SCALE что бы задать длину главного периода 20мс (50 герц).

Тогда, основываясь на том факте, что 4096 тактов это известное реальное время - можно подсчитать скважность
целевого ШИМ сигнала.
Если необходимая ширина ШИМ сигнала 2мс, тогда: 2мс это 10% от 20мс. А 10% от 4096 это 409-410. 
Схема управления шириной сигнала такая:
В регистры с префиксом LEDX_ON (где X это индекс выхода) пишем значение от 0 до 4095, означающее время
начала логической еденицы. Допустим это 0, то есть целевой испульс находится (начинается) в самом начале главного
периода (начинается на 0 такте из 4095 доступных).
И в регистры с префиксом LEDX_OFF пишем значение от 0 до 4095, означающее время
конца логической еденицы для каждого главного периода. 
Таким образом, если мы хотим получить ширину импульса 2мс при частоте 50 герц (20мс ширина главного периода)
мы задаем:
PRE_SCALE = 122
LEDX_ON = 0
LEDX_OFF = 410
Можно сдвинуть целевой импульс относительно начала главного периода. Таким образом, при настройках:
PRE_SCALE = 122
LEDX_ON = 100
LEDX_OFF = 510
(сдвинули импульс на 100 тактов "вправо")
Ширина ШИМ сигнала все также будет 2мс, частота все также будет 50 гц, но начинатся целевой импульс будет
с задержкой относительно начала главного периода (на 100 такте из 4095)

2. Оссобенности конкретных ESC и мотора.
Все значения тактов указаны с учетом PRE_SCALE == 122 (20мс, 50герц)
ESC начинет калиброваться лишь в диапозоне 150-270 таков. Имеется ввиду ширина целевого испульса, равная
n тактам, где n лежит в промежутке от 150 до 270 тактов.
Ширина испульса ниже или выше этого диапозона приводит к тому, что ESC игнорирует данный сигнал
при подаче питания и не калибруется (и соответсвенно не подает управляющее питание на мотор).
Не зависимо от того, какой шириной сигнала был откалиброван данный ESC - минимальное вращение мотора
начинается лишь с сигнала шириной минимум 273 такта.
При ширине примерно в 320 тактов мотор развивет довольно устрашающие, судя по звуку проппеллера, обороты 
и судя по измерениям выдает примерно 400-500 граммов тяги с пропеллером 10 дюймов.

3. Итог
Само по себе управление вращение мотором следует из управления шириной ШИМ сигнала. 
А значение ширины ШИМ сигнала, программно, в простейшем случае, задается лишь изменением регистра
LEDX_OFF (помещая в него значения от 0 до 4095). Значение помещается в два байта, из которых лишь
12 бит являются рабочими.

Простой способ передачи таких значений:

typedef union {
    int16_t value;
    struct {
        uint8_t low;
        uint8_t high;
    };
}PCA9685_DATA;

Значение ширины сигнала пишем в value, а записываем в регистры из low и high:
	PCA9685_DATA data;
	data.value = 410;
     	writeRegister(LED0_OFF_L, data.low);
        writeRegister(LED0_OFF_H, data.high);
Задали ширину ШИМ сигнала 410 тактов.
```
