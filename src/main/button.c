#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <mosquitto.h>


//int state = 0;         // used to track if LED is on or off.
//int toggleCount = 0;   // used to count number of falling edges received.

const char *gpio_relay;
const char *gpio_button;
const char *wait_relay;
const char *relay_status;

void toggle()
{
    
    if (atoi(relay_status) == 1) {
        digitalWrite(atoi(gpio_relay), HIGH);
        delay(atoi(wait_relay));
        digitalWrite(atoi(gpio_relay), LOW);
    } else {
        digitalWrite(atoi(gpio_relay), LOW);
        delay(atoi(wait_relay));
        digitalWrite(atoi(gpio_relay), HIGH);    
    }
    

    /*
    if(state==1) {
        digitalWrite(LedPin,LOW); // Set -ve lead of LED to ground.
        state = 0;
        printf("%d: LED ON\n", toggleCount++);
    } else {
        digitalWrite(LedPin, HIGH); // Set -ve lead of LED to +3.3V.
        state = 1;
        printf("%d: LED OFF\n", toggleCount++);
    }
    */
}


void *handle_button (void *args) {

    gpio_button = getenv("GPIO_BUTTON");
    gpio_relay = getenv("GPIO_RELAY");
    wait_relay = getenv("WAIT_RELAY");
    relay_status = getenv("RELAY_STATUS");

    printf("GPIO button -> %d\n", atoi(gpio_button));
    printf("status relay module -> %d\n", atoi(relay_status));

    /*
    if (wiringPiSetup() == -1) {
        printf("setup wiringPi failed !\n");
    }
    */
    //pinMode(LedPin, OUTPUT); // Set LED Pin mode as output..

    // definir pin de modulo rele ..
    pinMode(atoi(gpio_relay), OUTPUT);

    // definir estado de modulo rele ..
    if (atoi(relay_status) == 1) {
        digitalWrite(atoi(gpio_relay), LOW);
    }else {
        digitalWrite(atoi(gpio_relay), HIGH);
    }
    

    // Try registering interrupt handler.
    if (wiringPiISR(atoi(gpio_button), INT_EDGE_FALLING, &toggle) != 0) {
        printf("Registering ISR failed\n");
    }

    // wait for ever so the program doesn't terminate.
    while(1) {
        // Yield the thread so CPU doesn't run 100%
        delay(5000);
    }

}