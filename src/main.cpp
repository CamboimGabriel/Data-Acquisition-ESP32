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

//solo 1,2,3,4
DHT solo1(33, DHT22);
DHT solo2(32, DHT22);
DHT solo3(26, DHT22);
DHT solo4(27, DHT22);

//ambiente
DHT ambiente(25, DHT11);

OneWire oneWire(18);

DallasTemperature sensors(&oneWire);

WiFiClientSecure client;

String textFix = "GET /forms/d/e/1FAIpQLSeMRePPcHbF6prkBmC6bcII-fQSmF6d_Z5HYZuBBG6CjxmJZA/formResponse?ifq&entry.1839580957=";
 
float temperaturaENTRADA;
float temperaturaSAIDA; 
float temperaturaSOLO1;
float temperaturaSOLO2;
float temperaturaSOLO3;
float temperaturaSOLO4;
float temperaturaAMBIENTE;
float vazao;

int contaPulso; //Variável para a quantidade de pulsos

void connectWiFi()
{
    Serial.println("Connecting");

    //Troque pelo nome e senha da sua rede WiFi
    WiFi.begin("Pex_Movel2.4", "pexgeral@2020");
    
    //Espera enquanto não estiver conectado
    while(WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }

    Serial.println();
    Serial.print("Connected to ");
    Serial.println(WiFi.SSID());
    
}

void wifiConnectionTask(void* param)
{
    while(true)
    {
        //Se a WiFi não está conectada
        if(WiFi.status() != WL_CONNECTED)
        {
            //Manda conectar
            connectWiFi();
        }

        //Delay de 100 ticks
        vTaskDelay(100);
    }
}

void vazaoTask(void* param)
{
    while(true)
    {
        contaPulso = 0;//Zera a variável
        sei(); //Habilita interrupção
        vTaskDelay (1000); //Aguarda 1 segundo
        cli(); //Desabilita interrupção
        vazao = contaPulso / 5.5;
        printf("Vazão: %f L/min\n", vazao);
    }
}

void incpulso ()  {
 contaPulso++; //Incrementa a variável de pulsos
}

void googleConnectionTask(void* param)
{
    while(true){

    if (client.connect("docs.google.com", 443) == 1)//Tenta se conectar ao servidor do Google docs na porta 443 (HTTPS)
    {

        String toSend = textFix;//Atribuimos a String auxiliar na nova String que sera enviada

        //temperatura de entrada
        toSend += temperaturaENTRADA;

        //temperatura de saída
        toSend += "&entry.998674289=";
        toSend += temperaturaSAIDA;

        //vazao
        toSend += "&entry.1925389049=";
        toSend += vazao;

        //temperatura do solo 1
        toSend += "&entry.863873917=";
        toSend += temperaturaSOLO1;

        //temperatura do solo 2
        toSend += "&entry.1518420806=";
        toSend += temperaturaSOLO2;

        //temperatura do solo 3
        toSend += "&entry.317280867=";
        toSend += temperaturaSOLO3;

        //temperatura do solo 4
        toSend += "&entry.941940502=";
        toSend += temperaturaSOLO4;

        //temperatura ambiente
        toSend += "&entry.75671070=";
        toSend += temperaturaAMBIENTE;
        
        toSend += "&submit=Submit HTTP/1.1";//Completamos o metodo GET para nosso formulario.
        client.println(toSend);//Enviamos o GET ao servidor-
        client.println("Host: docs.google.com");//-
        client.println();//-
        client.stop();//Encerramos a conexao com o servidor
        Serial.println("Dados enviados.");//Mostra no monitor que foi enviado
        
    }
    else
    {
        Serial.println("Erro ao se conectar");//Se nao for possivel conectar no servidor, ira avisar no monitor.
    }

    vTaskDelay(10000);

    }
}

void setup()
{
    Serial.begin(115200);

    solo1.begin();
    solo2.begin();
    solo3.begin();
    solo4.begin();
    ambiente.begin();
    sensors.begin();

    connectWiFi();

    pinMode(2, INPUT);
    attachInterrupt(0, incpulso, RISING);

    //Cria uma nova tarefa no core 0
    xTaskCreatePinnedToCore(
        wifiConnectionTask,     //Função que será executada
        "wifiConnectionTask",   //Nome da tarefa
        10000,                  //Tamanho da memória disponível (em WORDs)
        NULL,                   //Não vamos passar nenhum parametro
        2,                      //prioridade
        NULL,                   //Não precisamos de referência para a tarefa
        0);                     //Número do core
    
  xTaskCreatePinnedToCore(
        googleConnectionTask,
        "googleConnectionTask", 
        10000,
        NULL,
        2,
        &google,
        1);
   
   xTaskCreatePinnedToCore(
        vazaoTask,
        "vazaoTask", 
        10000,
        NULL,
        2,
        NULL,
        1);
}

void loop()
{
    vTaskSuspend(google);

    sensors.requestTemperatures();

    temperaturaENTRADA = sensors.getTempCByIndex(0);
    temperaturaSAIDA = sensors.getTempCByIndex(1);

    temperaturaSOLO1 = solo1.readTemperature();
    temperaturaSOLO2 = solo2.readTemperature();
    temperaturaSOLO3 = solo3.readTemperature();
    temperaturaSOLO4 = solo4.readTemperature();
    temperaturaAMBIENTE = ambiente.readTemperature();

    vTaskResume(google);
}

