#include <WiFi.h>
#include <WiFiUdp.h> //Socket UDP
#include <NTPClient.h>
#include <DHT.h>
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <HTTPClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

TaskHandle_t google;

#define user "VIVOFIBRA-C048"
#define password "722345C048"

DHT solo11(32, DHT22);
DHT solo12(33, DHT22);
DHT solo13(25, DHT22);
DHT solo14(26, DHT22);

//4, 5, 22, 23
//34 35 n pega ADC

DHT solo21(27, DHT22); 
DHT solo22(14, DHT22); 
DHT solo23(12, DHT22);
DHT solo24(13, DHT22); 

DHT solo31(5, DHT22); 
DHT solo32(4, DHT22);
DHT solo33(22, DHT22);
DHT solo34(23, DHT22);

//ambiente
DHT ambiente(19, DHT11);

//Sensores de temp a prova d'agua
OneWire oneWire(18);
DallasTemperature sensors(&oneWire);

WiFiClientSecure client;

String textFix = "GET /forms/d/e/1FAIpQLSeMRePPcHbF6prkBmC6bcII-fQSmF6d_Z5HYZuBBG6CjxmJZA/formResponse?ifq&entry.1839580957=";

//sensores de temperatura
float temperaturaENTRADA = 0;
float temperaturaSAIDA = 0;
float temperaturaSOLO11 = 0;
float temperaturaSOLO12 = 0;
float temperaturaSOLO13 = 0;
float temperaturaSOLO14 = 0;
float temperaturaSOLO21 = 0;
float temperaturaSOLO22 = 0;
float temperaturaSOLO23 = 0;
float temperaturaSOLO24 = 0;
float temperaturaSOLO31 = 0;
float temperaturaSOLO32 = 0;
float temperaturaSOLO33 = 0;
float temperaturaSOLO34 = 0;
float temperaturaAMBIENTE = 0;

//Sensor de vazão
const int PINO_SENSOR = 21;
unsigned long contador = 0;
const float FATOR_CALIBRACAO = 4.5;
float fluxo = 0;
unsigned long tempo_antes = 0;

void contador_pulso()
{
    contador++;
}

void connectWiFi()
{
    Serial.println("Connecting");

    WiFi.begin(user,password);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
}

void wifiConnectionTask(void *param)
{
    while (true)
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            connectWiFi();
        }
        vTaskDelay(100);
    }
}

void googleConnectionTask(void *param)
{
    while (true)
    {
        Serial.print("Temperatura de Entrada: ");
        Serial.println(temperaturaENTRADA);

        Serial.print("Temperatura de Saída: ");
        Serial.println(temperaturaSAIDA);

        Serial.print("Temperatura do Solo 1-1: ");
        Serial.println(temperaturaSOLO11);
        Serial.print("Temperatura do Solo 1-2: ");
        Serial.println(temperaturaSOLO12);
        Serial.print("Temperatura do Solo 1-3: ");
        Serial.println(temperaturaSOLO13);
        Serial.print("Temperatura do Solo 1-4: ");
        Serial.println(temperaturaSOLO14);

        Serial.print("Temperatura do Solo 2-1: ");
        Serial.println(temperaturaSOLO21);
        Serial.print("Temperatura do Solo 2-2: ");
        Serial.println(temperaturaSOLO22);
        Serial.print("Temperatura do Solo 2-3: ");
        Serial.println(temperaturaSOLO23);
        Serial.print("Temperatura do Solo 2-4: ");
        Serial.println(temperaturaSOLO24);

        Serial.print("Temperatura do Solo 3-1: ");
        Serial.println(temperaturaSOLO31);
        Serial.print("Temperatura do Solo 3-2: ");
        Serial.println(temperaturaSOLO32);
        Serial.print("Temperatura do Solo 3-3: ");
        Serial.println(temperaturaSOLO33);
        Serial.print("Temperatura do Solo 3-4: ");
        Serial.println(temperaturaSOLO34);

        Serial.print("Temperatura do Ambiente: ");
        Serial.println(temperaturaAMBIENTE);

        Serial.print("Fluxo de água (L/min): ");
        Serial.println(fluxo);

        Serial.println("-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-");

        if (client.connect("docs.google.com", 443) == 1)
        {

            String toSend = textFix;

            toSend += temperaturaENTRADA;

            toSend += "&entry.998674289=";
            toSend += temperaturaSAIDA;

            toSend += "&entry.1925389049=";
            toSend += fluxo;

            toSend += "&entry.863873917=";
            toSend += temperaturaSOLO11;

            toSend += "&entry.1518420806=";
            toSend += temperaturaSOLO12;

            toSend += "&entry.317280867=";
            toSend += temperaturaSOLO13;

            toSend += "&entry.941940502=";
            toSend += temperaturaSOLO14;

            toSend += "&entry.177193093=";
            toSend += temperaturaSOLO21;

            toSend += "&entry.394934538=";
            toSend += temperaturaSOLO22;

            toSend += "&entry.1773083222=";
            toSend += temperaturaSOLO23;

            toSend += "&entry.1711785333=";
            toSend += temperaturaSOLO24;

            toSend += "&entry.1119390422=";
            toSend += temperaturaSOLO31;

            toSend += "&entry.525185917=";
            toSend += temperaturaSOLO32;

            toSend += "&entry.1983348490=";
            toSend += temperaturaSOLO33;

            toSend += "&entry.1238373872=";
            toSend += temperaturaSOLO34;

            toSend += "&entry.75671070=";
            toSend += temperaturaAMBIENTE;

            toSend += "&submit=Submit HTTP/1.1";     //Completamos o metodo GET para nosso formulario.
            client.println(toSend);                  //Enviamos o GET ao servidor-
            client.println("Host: docs.google.com"); //-
            client.println();                        //-
            client.stop();                           //Encerramos a conexao com o servidor
            Serial.println("Dados enviados.");       //Mostra no monitor que foi enviado
        }
        else
        {
            Serial.println("Erro ao se conectar"); //Se nao for possivel conectar no servidor, ira avisar no monitor.
        }

        vTaskDelay(10000);
    }
}

void setup()
{
    Serial.begin(115200);

    solo11.begin();
    solo12.begin();
    solo13.begin();
    solo14.begin();

    solo21.begin();
    solo22.begin();
    solo23.begin();
    solo24.begin();

    solo31.begin();
    solo32.begin();
    solo33.begin();
    solo34.begin();

    //sensor de agua
    pinMode(PINO_SENSOR, INPUT_PULLUP);

    ambiente.begin();
    sensors.begin();

    connectWiFi();

    xTaskCreatePinnedToCore(
        wifiConnectionTask, 
        "wifiConnectionTask", 
        10000,             
        NULL,             
        2,                  
        NULL,                
        0);        

    xTaskCreatePinnedToCore(
        googleConnectionTask,
        "googleConnectionTask",
        10000,
        NULL,
        2,
        &google,
        1);
}

void loop()
{
    vTaskSuspend(google);

    if ((millis() - tempo_antes) > 1000)
    {
        detachInterrupt(PINO_SENSOR);

        fluxo = ((1000.0 / (millis() - tempo_antes)) * contador) / FATOR_CALIBRACAO;

        contador = 0;

        tempo_antes = millis();

        attachInterrupt(PINO_SENSOR, contador_pulso, FALLING);
    }

    sensors.requestTemperatures();

    temperaturaENTRADA = sensors.getTempCByIndex(0);
    temperaturaSAIDA = sensors.getTempCByIndex(1);

    temperaturaSOLO11 = solo11.readTemperature();
    temperaturaSOLO12 = solo12.readTemperature();
    temperaturaSOLO13 = solo13.readTemperature();
    temperaturaSOLO14 = solo14.readTemperature();

    temperaturaSOLO21 = solo21.readTemperature();
    temperaturaSOLO22 = solo22.readTemperature();
    temperaturaSOLO23 = solo23.readTemperature();
    temperaturaSOLO24 = solo24.readTemperature();

    temperaturaSOLO31 = solo31.readTemperature();
    temperaturaSOLO32 = solo32.readTemperature();
    temperaturaSOLO33 = solo33.readTemperature();
    temperaturaSOLO34 = solo34.readTemperature();

    temperaturaAMBIENTE = ambiente.readTemperature();

    vTaskResume(google);
}
