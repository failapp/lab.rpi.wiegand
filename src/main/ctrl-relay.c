#include <stdio.h>
#include <stdlib.h>
#include <mosquitto.h>
#include <wiringPi.h>

const char *gpio_relay;
const char *wait_relay;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    printf("ID: %d\n", * (int *) obj);
    if (rc) {
        printf("Error with result code: %d\n", rc);
        exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, "sync/relay/gpio4", 0);
}


void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    
    printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);

    digitalWrite(atoi(gpio_relay), HIGH);
    delay(atoi(wait_relay));
    digitalWrite(atoi(gpio_relay), LOW);

}


int main() {

    //gpio_relay = getenv("GPIO_RELAY");
    gpio_relay = "4";
    wait_relay = "1000";

    wiringPiSetup();
    pinMode(atoi(gpio_relay), OUTPUT);

    // definir estado de modulo rele ..
    digitalWrite(atoi(gpio_relay), LOW);


    int rc, id=104;
    mosquitto_lib_init();

    struct mosquitto *mosq;

    mosq = mosquitto_new("subscribe-relay-module", true, &id);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);

    rc = mosquitto_connect(mosq, "localhost", 1883, 10);
    if (rc) {
        printf("Could not connect to Broker with return code %d\n", rc);
        return -1; 
    }


    mosquitto_loop_forever(mosq, -1, 1);	
    mosquitto_lib_cleanup();

    return 0;
}