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
const char *wait_relay;
const char *relay_status;
const char *mqtt_id;


void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    printf("ID: %d\n", * (int *) obj);
    if (rc) {
        printf("Error with result code: %d\n", rc);
        exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, mqtt_topic, 0);
}


void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    
    printf("new message with topic -> %s: %s\n", msg->topic, (char *) msg->payload);
    
    if (atoi(relay_status) == 1) {
        digitalWrite(atoi(gpio_relay), HIGH);
        delay(atoi(wait_relay));
        digitalWrite(atoi(gpio_relay), LOW);
    } else {
        digitalWrite(atoi(gpio_relay), LOW);
        delay(atoi(wait_relay));
        digitalWrite(atoi(gpio_relay), HIGH);
    }

}


void *handle_relay(void *args) {
    
    gpio_relay = getenv("GPIO_RELAY");
    mqtt_host = getenv("MQTT_HOST");
    mqtt_topic = getenv("MQTT_TOPIC_RELAY");
    wait_relay = getenv("WAIT_RELAY");
    relay_status = getenv("RELAY_STATUS");
    mqtt_id = getenv("MQTT_ID");
    
    printf("reader: GPIO relay module -> %d\n", atoi(gpio_relay));
    printf("reader: status relay module -> %d\n", atoi(relay_status));
    printf("reader: mqtt host -> %s\n", mqtt_host);
    printf("reader: mqtt topic relay -> %s\n", mqtt_topic);

    //wiringPiSetup();
    pinMode(atoi(gpio_relay), OUTPUT);

    // definir estado de modulo rele ..
    if (atoi(relay_status) == 1) {
        digitalWrite(atoi(gpio_relay), LOW);
    } else {
        digitalWrite(atoi(gpio_relay), HIGH);
    }
    

    //int rc, id=101;
    int rc, id=atoi(mqtt_id);
    char subscriber_name[20] = "subscriber-";
    strcat(subscriber_name, mqtt_id);

    mosquitto_lib_init();
    //mosq = mosquitto_new("subscribe-lab", true, &id);
    mosq = mosquitto_new(subscriber_name, true, &id);

    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    rc = mosquitto_connect(mosq, mqtt_host, 1883, 10);
    if (rc) {
        printf("Could not connect to Broker with return code %d\n", rc);
    }   
    mosquitto_loop_forever(mosq, -1, 1);	
    mosquitto_lib_cleanup();

}

