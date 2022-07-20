#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <mosquitto.h>
//#include <wiringPi.h>

struct mosquitto *mosq;
int gpio_relay=-1;

void on_connect(struct mosquitto *mosq, void *obj, int rc) {
    printf("ID: %d\n", * (int *) obj);
    if (rc) {
        printf("Error with result code: %d\n", rc);
        exit(-1);
    }
    mosquitto_subscribe(mosq, NULL, "test/t1", 0);
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    printf("New message with topic %s: %s\n", msg->topic, (char *) msg->payload);
    // digitalWrite(gpio_relay, LOW);
    // delay(500);
    // digitalWrite(gpio_relay, HIGH);
    // delay(500);
}



void *handle_relay(void *args) {

    /*
    const char *pin_relay = getenv("GPIO_RELAY");
    if (pin_relay!=NULL) gpio_relay = atoi(pin_relay);
    printf("GPIO relay -> %d\n", gpio_relay);
    wiringPiSetup();
    pinMode(gpio_relay, OUTPUT);
    */
   
    int rc, id=101;

    mosquitto_lib_init();
    mosq = mosquitto_new("subscribe-lab", true, &id);
    mosquitto_connect_callback_set(mosq, on_connect);
    mosquitto_message_callback_set(mosq, on_message);
    rc = mosquitto_connect(mosq, "localhost", 1883, 10);
    if (rc) {
        printf("Could not connect to Broker with return code %d\n", rc);
    }
    
    mosquitto_loop_forever(mosq, -1, 1);	
    mosquitto_lib_cleanup();
}


void *hello(void *args){

    /*
    const char *pin_data0 = getenv("PIN_DATA0_IN");
    const char *pin_data1 = getenv("PIN_DATA1_IN");

    int pin0=-1;
    int pin1=-1;

    //printf("PIN_DATA_IN :%s\n",( pin_data0!=NULL ) ? pin_data0 : "getenv returned NULL");
    //printf("PIN_DATA_OUT :%s\n",( pin_data1!=NULL ) ? pin_data1 : "getenv returned NULL");

    if (pin_data0!=NULL) pin0 = atoi(pin_data0);
    if (pin_data1!=NULL) pin1 = atoi(pin_data1);

    printf("pin0 -> %d\n", pin0);
    printf("pin1 -> %d\n", pin1);
    */

    char *str="hello xD";
    for (int i=0; i< strlen(str); i++) {
        fflush(stdout);
        printf("%c", str[i]);
        sleep(1);
    }
    printf("\n");
}



int main(int argc, char const *argv[] ) {

    int rc, id=12;
    pthread_t greeting;
    pthread_t relay;
    //pthread_t reader_in;
    //pthread_t reader_out;

    pthread_create(&greeting, NULL, hello, NULL);
    pthread_create(&relay, NULL, handle_relay, NULL);  

    pthread_join(greeting, NULL);
    pthread_join(relay, NULL);


    return 0;
}
