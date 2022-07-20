#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <wiringPi.h>
#include <mosquitto.h>

struct mosquitto *mosq;
const char *gpio_relay;
const char *mqtt_host;
const char *mqtt_topic;


void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    printf("ID: %d\n", * (int *) obj);
    if (rc) {
        printf("Error with result code: %d\n", rc);
        exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, mqtt_topic, 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
    digitalWrite(atoi(gpio_relay), LOW);
    delay(500);
    digitalWrite(atoi(gpio_relay), HIGH);
    delay(500);
}

void *handle_relay(void *args) {
    
    gpio_relay = getenv("GPIO_RELAY");
    mqtt_host = getenv("MQTT_HOST");
    mqtt_topic = getenv("MQTT_TOPIC");

    printf("GPIO relay -> %d\n", atoi(gpio_relay));
    printf("mqtt host -> %s\n", mqtt_host);
    printf("mqtt topic -> %s\n", mqtt_topic);

    //wiringPiSetup();
    pinMode(atoi(gpio_relay), OUTPUT);
    
    int rc, id=101;
    mosquitto_lib_init();
    mosq = mosquitto_new("subscribe-lab", true, &id);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    rc = mosquitto_connect(mosq, mqtt_host, 1883, 10);
    if (rc) {
        printf("Could not connect to Broker with return code %d\n", rc);
    }   
    mosquitto_loop_forever(mosq, -1, 1);	
    mosquitto_lib_cleanup();
}

