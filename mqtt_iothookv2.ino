/*Bu uygulamamızda da geçen uygulamada olduğu gibi mqtt publisherından gelen sayı verisini
 * dakikaya bağlı olarak ledi yakacak şekilde programlıyoruz. Geçen uygulamadan farkı ise
 * delay yerine millis(); fonksiyonunu kullanıyor olmamız. 
 * mfbynl
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//Wifi ve mqtt server bilgilerini giriyoruz
const char* ssid = "Mese";
const char* password =  "MeseBilisim1303**";
const char* mqttServer = "iothook.com";
const int mqttPort = 1883;
const char* mqttUser = "publicmqttbroker";
const char* mqttPassword = "publicmqttbroker";


WiFiClient espClient;
PubSubClient client(espClient);


unsigned long önceki_zaman;
unsigned long simdiki_zaman;
int leddurum = LOW;



void setup() {
  pinMode(LED_BUILTIN, OUTPUT);


  Serial.begin(115200);

  /* WiFi BAĞLANTISI */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");


  /* MQTT BAĞLANTISI */
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
  //Test mesajı
  client.publish("nodemcu", "test");
  client.subscribe("nodemcu");

}



void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.println((char)payload[i]);
  }
  // Bu komut ile gelen veriyi daha okunabilir ve kullanılabilir hale getiriyoruz
  payload[length] = '\0';

  char * charPointer = (char *)payload;
  double f = 0;
  int in = 0;
  String s = "";
  if (isDigit(charPointer[0])) {
    f = atof(charPointer);
    in = atoi(charPointer);
    Serial.printf("Float = %0.1f, int = %d\n", f, in);
  } 
  /*Bir dakikayı gelen veri ile bölüyoruz, çıkan değerin yarısında ledimiz yanıp yarısında
   *sönüyor...
  */
  float hesap = (60 / f);
  float delay_hesap = (hesap / 2);
  Serial.println();
  Serial.print("Dakikada ");
  Serial.print(in);
  Serial.println(" kere tur atacak");
  Serial.println();
  Serial.print(delay_hesap);
  Serial.println(" saniye yanıp sönecek");
  Serial.println();

  digitalWrite(LED_BUILTIN, HIGH);

/*Bu kod bloğunda for döngüsü içine aldığımız millis(); fonksiyonuna şimdiki_zaman değerini
 * atıyoruz. Hesaplanan bekleme değerini her geçtiğinde ledimiz yanıp sönüyor ve bunu delay
 * olmadan gerçekleştiriyor...
 */
  for (int i = 0; i < in;) {
    
      simdiki_zaman = millis();
      if (simdiki_zaman - önceki_zaman > delay_hesap * 1000) {
        leddurum = !leddurum;
        digitalWrite(LED_BUILTIN, leddurum);
        Serial.print(i + 1);
        Serial.println(". tur bitti");
        Serial.println();
        önceki_zaman = simdiki_zaman;
        i++;
      }
    }

  digitalWrite(LED_BUILTIN, HIGH);
  Serial.println();
  Serial.println("-----------------------");

  //Bitti mesajını göndermek için tekrardan bağlanıyoruz...
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");


  /* MQTT BAĞLANTISI */
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP8266Client", mqttUser, mqttPassword )) {

      Serial.println("connected");

    } else {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);

    }
  }
  
  //Bitti mesajı
  client.publish("nodemcu", "bitti");
  client.subscribe("nodemcu");
  Serial.println("Döngü Bitti!");
}

void loop() {
  client.loop();
}
