 /*
 *  \brief     Управление 16-ю реле через UART
 *
 *  \author    Андрей Алексеев <andrey@thinking-house.ru>
 *
 *  \copyright Лаборатория Умного Дома (ИП Алексеев А.П. ИНН 780725728171), 2020. Все права защищены.
 */

//! Подключение заголовочных файлов

//! Макроопределения

// Состояние вывода равное Включено
#define ON LOW
// Состояние вывода равное Выключено
#define OFF HIGH
// Скорость порта UART
#define UART_BAND 115200
// Символ конца строки
#define END_SYMBOL '\r'

//! Локальные типы данных

//! Глобальные переменные
const int relayCount = 16;
const int relayPin[]={2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3};
bool relayStatus[]={false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
String relayNames[] ={"CH1", "CH2", "CH3", "CH4", "CH5", "CH6", "CH7", "CH8", "CH9", "CH10", "CH11", "CH12", "CH13", "CH14", "CH15", "CH16"};
unsigned long relayTimestamp[] ={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// Задержка по умолчанию 1 сек
int delayS = 1;

// Служебные переменные
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete

//! Глобальные функции

/**
*        
* \brief Первоначальная настройка. Первая запускаемая функция после сброса, установка параметров
*/
void setup()
{
    Serial.begin(UART_BAND);
  
    for(int i=0; i < relayCount; i++)
    {
        pinMode(relayPin[i], OUTPUT); // set relay pins as output  
        digitalWrite(relayPin[i], OFF);// initial relay status to be OFF
    }
    inputString.reserve(200);
    delay(delayS * 1000 + 1000);
    Serial.println("Code Started");
}

/**
*        
* \brief Основное тело программы. Выполняется циклически
*/  
void loop()
{
    if (stringComplete) 
    {
        if (inputString.startsWith("DELAY="))
        {
            inputString.replace("DELAY=", "");
            delayS = inputString.toInt();
            Serial.print("Set Delay = ");Serial.println(delayS);
        }
        else
        {
            int number = inputString.toInt();
            if (number > 0 && number <= 16)
            {
                 relayTimestamp[number - 1] = millis();
            }
            else
            {
                Serial.print("ERROR. Relay ");Serial.print(number);Serial.println(" not found");
            }
        }
        inputString = "";
        stringComplete = false;
    }

    for(int i = 0; i < relayCount; i++)
    {
        if ((millis() - relayTimestamp[i] < delayS * 1000) && !relayStatus[i])
        {
            digitalWrite(relayPin[i], LOW); // Turn ON relay
            Serial.print(relayNames[i]);
            Serial.println(" ON");
            relayStatus[i] = true;
        }
        else if ((millis() - relayTimestamp[i] > delayS * 1000) && relayStatus[i])
        {
            digitalWrite(relayPin[i], HIGH); // Turn ON relay
            Serial.print(relayNames[i]);
            Serial.println(" OFF");
            relayStatus[i] = false;
        }
        else
        {
            
        }
    }
}

/**
*        
* \brief Обработка команд из UART Работает по прерыванию
*/
void serialEvent() 
{
    while (Serial.available()) 
    {
        char inChar = (char)Serial.read();
        inputString += inChar;
  
        if (inChar == END_SYMBOL) 
        {
            stringComplete = true;
        }
    }
}
