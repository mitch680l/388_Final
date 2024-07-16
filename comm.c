#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "eecs388_lib.h"

void auto_brake(int devid)
{
    // Task-1: 
    // Your code here (Use Lab 02 - Lab 04 for reference)
    // Use the directions given in the project document
    uint16_t dist = 0;
    uint16_t Dist_h = 0;
    uint16_t Dist_l = 0;



        if ('Y' == ser_read(devid) && 'Y' == ser_read(devid)) {
            //printf("Reading Lidar\n");
            Dist_l = ser_read(devid);
            Dist_h = ser_read(devid);

            dist = (Dist_h << 8) | Dist_l;
        
    

        // Setting braking conditions
            //printf ("Lidar Value: %u\n", dist);
            if(dist < 60){ // less than 60cm, flash red LED
                gpio_write(GREEN_LED, OFF);
    
                gpio_write(RED_LED, ON);
                delay(100);
                gpio_write(RED_LED, OFF);
                delay(100);
                //gpio_write(RED_LED, ON);
                //delay(100);
                //gpio_write(RED_LED, OFF);

            }

            else if(dist >= 60 && dist <= 100){ // between 60 and 100 turn on red LED
                gpio_write(GREEN_LED, OFF);
                gpio_write(RED_LED, ON);
                }

            else if(dist > 100 && dist <= 200){ // between 100 cm and 200cm, turn on YELLOW LED (red + green)
                gpio_write(GREEN_LED, ON);
                gpio_write(RED_LED, ON);
                }

            else if(dist > 200){ // greated than 200cm distance
                gpio_write(RED_LED, OFF);
                gpio_write(GREEN_LED, ON);
                }
        }
        }




int read_from_pi(int devid)
{
    // Task-2: 
    // You code goes here (Use Lab 09 for reference)
    // After performing Task-2 at dnn.py code, modify this part to read angle values from Raspberry Pi.
    
    
    
    char str[4];
    int read;
        if(ser_isready(devid)) {
            ser_readline(1,4, str);
            //printf("String Angle =%s", &str_ptr);
            sscanf(str,"%d",&read);
            return read;
            //ser_printline(1,str_ptr);
        }
    return 0;
}
    

#define SERVO_PULSE_MAX 2400    /* 2400 us */
#define SERVO_PULSE_MIN 544     /* 544 us */
#define SERVO_PERIOD    20000   /* 20000 us (20ms) */
void steering(int gpio, int pos)
{
    
    int range = SERVO_PULSE_MAX - SERVO_PULSE_MIN;

    int delay = ((pos / 180) * range) + SERVO_PULSE_MIN;
    //printf("\nSteering at Delay: %d\n",delay);
    gpio_write(gpio,ON);
    delay_usec(delay);
    gpio_write(gpio, OFF);
    delay_usec(SERVO_PERIOD - delay);
    
    
}


int main()
{
    // initialize UART channels
    ser_setup(0); // uart0
    ser_setup(1); // uart1
    int pi_to_hifive = 1; //The connection with Pi uses uart 1
    int lidar_to_hifive = 0; //the lidar uses uart 0
    
    printf("\nUsing UART %d for Pi -> HiFive", pi_to_hifive);
    printf("\nUsing UART %d for Lidar -> HiFive", lidar_to_hifive);
    
    //Initializing PINs
    gpio_mode(PIN_19, OUTPUT);
    gpio_mode(RED_LED, OUTPUT);
    gpio_mode(BLUE_LED, OUTPUT);
    gpio_mode(GREEN_LED, OUTPUT);

    printf("Setup completed.\n");
    printf("Begin the main loop.\n");

    while (1) {
        auto_brake(lidar_to_hifive); // measuring distance using lidar and braking
        int angle = read_from_pi(pi_to_hifive); //getting turn direction from pi
        if (angle > 180) {
             angle = 0;
            
        }
        
        if (angle != 0) {
           printf("\nangle=%d", angle);
        }
        
        int gpio = PIN_19; 
        for (int i = 0; i < 2; i++){
            // Here, we set the angle to 180 if the prediction from the DNN is a positive angle
            // and 0 if the prediction is a negative angle.
            // This is so that it is easier to see the movement of the servo.
            // You are welcome to pass the angle values directly to the steering function.
            // If the servo function is written correctly, it should still work,
            // only the movements of the servo will be more subtle
            
            steering(gpio,angle);
            /*
            if(angle>0){
                steering(gpio, 180);
            }
            else {
                steering(gpio,0);
            }
            */
            // Uncomment the line below to see the actual angles on the servo.
            // Remember to comment out the if-else statement above!
            // steering(gpio, angle);
        }

    }
    return 0;
}
