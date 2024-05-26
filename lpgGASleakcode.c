#include <HTTPClient.h>
#include <ESP32Servo.h>

const char* ssid = "stgts";
const char* password = "12345678";
const char* PUSHBULLET_TOKEN = "o.BaVicEUlz90IvbVjaPTfnXbW4hdoSnYC";

const int MQ5_PIN = 34; // Analog pin connected to MQ-5 sensor
const float GAS_THRESHOLD = 1500.0; // Gas concentration threshold

const int SERVO_PIN = 12; // Pin connected to servo signal wire
Servo myServo; // Create a servo object

void setup() {
  Serial.begin(115200);
  delay(4000);
  
  Serial.println("Connecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Attach servo to the servo pin
  myServo.attach(SERVO_PIN);
  myServo.write(0);
  delay(1000); // Delay to allow servo to reach position
}

void loop() {
  int gasValue = analogRead(MQ5_PIN);
  Serial.print("Gas Value: ");
  Serial.println(gasValue);

  if (gasValue > GAS_THRESHOLD) {
    // Rotate servo by 90 degrees
    myServo.write(90);
    delay(1000); // Delay to allow servo to reach position
    sendPushNotification("Gas Leak Alert", "Gas leak detected! Take immediate action.");
    

    delay(60000); // Send notification every 1 minute
  }

  delay(1000); // Adjust delay as needed
}

void sendPushNotification(const char* title, const char* body) {
  HTTPClient http;
  http.begin("https://api.pushbullet.com/v2/pushes");
  http.addHeader("Access-Token", PUSHBULLET_TOKEN);
  http.addHeader("Content-Type", "application/json");

  // Construct JSON payload
  String jsonPayload = "{\"type\":\"note\",\"title\":\"" + String(title) + "\",\"body\":\"" + String(body) + "\"}";

  // Send POST request
  int httpResponseCode = http.POST(jsonPayload);

  if (httpResponseCode == 200) {
    Serial.println("Notification sent successfully.");
  } else {
    Serial.print("Failed to send notification. Status code: ");
    Serial.println(httpResponseCode);
    Serial.println(http.getString());
  }

  http.end();
}