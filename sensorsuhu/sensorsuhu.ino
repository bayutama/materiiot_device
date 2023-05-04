#include <WiFi.h> // library WiFi 
#include <Wire.h> // Library Wire
#include "DHT.h" // Llibrary Sensor DHT 
#include <PubSubClient.h> // Library MQTT

#define DHTPIN 23 // konfigurasi pin data DHT di pin D23
#define DHTTYPE DHT22 // Tipe DHT 22
#define MSG_BUFFER_SIZE  (150) //Kapasitas Pesan yang dikirim 150 Bytes
char msg[MSG_BUFFER_SIZE] = "{\"Temp\":\"%.2f\",\"Hum\":\"%.2f\"}"; // Format Pesan yang dikirim 
char pesan[150];

const char* ssid     = "fikrinizar"; // Konfigurasi SSID WiFi yang digunakan 
const char* password = "satekambing48"; // Konfigurasi Password WIF yang digunakan 

const char* mqtt_server = "mqtt.telkomiot.id"; // Konfigurasi Server MQTT Broker
const char* topik = "v2.0/pubs/APP633557d0f1ec817960/DEV6336580760ee643679"; // KOnfigurasi Topik MQTT 
String AccessKey = "18388621febe647b"; // Username MQTT
String AccessToken = "18388621fecebeaa"; // Password MQTT 
String clientId = "Dev1"; // Client ID MQTT 

DHT dht(DHTPIN, DHTTYPE); // Inisiasi Snesor DHT 
WiFiClient espClient; // Inisiasi WiFi 
PubSubClient client(espClient); // Inisiasi MQTT

void setup()
{
  Serial.print("Connecting to "); // Mengkoneksikan Device ke WiFi
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  client.setServer(mqtt_server, 1883); // Mengkoneksikan Device ke broker MQTT 
  Serial.println("");
  dht.begin();
}

void loop()
{
  float h = dht.readHumidity(); // membaca Nilai kelembapan
  float t = dht.readTemperature(); // membaca nilai suhu
  if (isnan(h) || isnan(t))// Jika sensor tidak terbaca, maka  
  {
    return; // Ulangi pembacaan sensor 
  }
  if (!client.connected()) { // jika mqtt tidak terkoneksi 
    reconnect(); // jalankan fungsi recconect 
  }
  client.loop();
  sprintf (pesan, msg, t, h); // packaging pesan yang akan dikirim 
  Serial.print("Publish message: "); // menampilkan pesan pada serial monitor 
  Serial.println(pesan);
  client.publish(topik, pesan);// mengirimkan pesan ke broker 
  delay(1000);
}

void reconnect() { // fungsi recconect mqtt
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect(clientId.c_str(), AccessKey.c_str(), AccessToken.c_str())) {
      Serial.println("connected");
      client.publish(topik, "hello world");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
