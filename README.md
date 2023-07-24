# librenightscout
Проект создан для управления шаговым мотором с помощью NodeMCU(esp8266) и сервисом nightscout исходя из показаний датчика Freestyle libre (уровня сахара в крови). 


## Компоненты: 
1) NodeMCU v1 (esp8266)
2) Шаговый мотор 28byj-48 с драйвером uln2003
3) Кнопка
4) Резистор на 10 Ком
5) Телефон с установленным и настроенным приложением Xdrip
6) Виртуальный/физический сервер с Ubuntu 20.04
7) Шнур microusb для прошивки NodeMCU


## Демонстрация:
https://youtu.be/vY7p96062Fs


## Схема подключения:
![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/3cad3e4a-550d-4fe1-87a6-9dffb6b31b0f)



## Настройка Xdrip
1) Настройки-> Источник данных-> LibreAlarm
   ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/354fc14d-5733-4b45-acca-95b9b50875ad)
   
3) Настройки-> Функция сканирования NFC-> Переключить ползунок
   ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/d4082d88-2cf6-4237-84c8-463fa8df23ec)
   
5) Настройки-> Загрузка в облако-> Синхронизация с Nightscout -> Включено
   ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/f1f2d398-fa52-4c3c-bb92-5ec22ea29f13)
   ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/432ff23d-4b9d-4718-86f3-54bbc2215b77)
   
7) Настройки-> Загрузка в облако-> Синхронизация с Nightscout -> Основной URL
   ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/47adb7ac-0f85-4fd3-908b-07f2e8ecf467)
   Вводим URL
   ``` https://<SECRET_KEY>@librenightscout.ru/api/v1/ ```
   
 9) Настройки-> Прогнозирование значений-> Выключить ползунок
    ![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/52fd7fa4-08ec-4e00-9b13-f94579cdb86f)



## Разворачивание nightscout: 
https://nightscout.github.io/vendors/VPS/docker/


## Ссылка на сервер:
https://cp.beget.com/login

## Xdrip
https://github.com/NightscoutFoundation/xDrip
Скачать: https://jamorham.github.io/#xdrip-plus

## Доп материалы
GPIO NodeMCU v1
https://lastminuteengineers.b-cdn.net/wp-content/uploads/iot/ESP8266-Pinout-NodeMCU.png![image](https://github.com/VadimBunegin/librenightscout/assets/61832909/33780780-cd73-432b-bf19-a1f960326419)



