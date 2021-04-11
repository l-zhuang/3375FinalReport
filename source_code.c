#include "address_map_arm.h"

typedef struct _jtag_uart
{
    int data ;
    int control ;
} jtag_uart ;

//define timer register address
typedef struct _timer
{
    int status ;
    int control ;
    int low_period ;
    int high_period ;
    int low_counter ;
    int high_counter ;
} timer ;

//define GPIO address 
typedef struct _GPIO{
	int data;
	int control;
} GPIO;

//Pre-defined wind speed dataset
int fakeData[]={3,4,5,6,1};

//define address variables for Led, timer, segment display, GPIO port, switch and ADC
volatile timer * t= (timer*) TIMER_BASE;
volatile int * display= (int *) HEX3_HEX0_BASE;
volatile int * LED_ptr       = (int *)LED_BASE; // LED address
volatile GPIO * GPIOPtr= (GPIO *)JP1_BASE;
volatile int * switchPtr= (int *)SW_BASE;
volatile jtag_uart * const uart =( jtag_uart *) JTAG_UART_BASE ;

int main(void) {
    //7-segment display lookup table array
    int lookUpTable[16]={0x3f,0x06,0x5B,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x67,0x77,0x7c,0x39,0x5e,0x79,0x71};
    int launch=0;

    //initialize the counting interval to 0.1 seconds
    int interval = 100000000;
    t->low_period=interval;
    t->high_period=interval>>16;

    //initialization of predefined starting values
    int sPosition=0;
    int wSpeed=0;
    int altitude=3;
    int bitmask=1<<(altitude-1);

    //other intializations
    int sum,counter,position=0;
    int m=0;

    //read wind direction from corresponding switch
    bitmask=1<<(altitude-1);
    int dir=(*(switchPtr)&bitmask)>>(altitude-1);

    //wind speed is positive((to the right) if switch is off
    if(dir!=1){
        wSpeed=fakeData[altitude-1];
    }
    else{//negative
        wSpeed=(-1)*fakeData[altitude-1];
    }


int bSpeed=0;//ballon speed
int mass=5;//mass

//set up the timer to run continuously
t -> control = 6;

// buffer for reading data from JTAG UART
int uart_buf ;

//while (true)
while(1){

if(t->status&1){

    t->status=1;
    if(launch==0){
        uart_buf = uart ->data ;
        while (( uart_buf & 0x8000 ) &&(( uart_buf & 0xFF ) != 0x6c ))
        {
            uart -> data = ( uart_buf & 0xFF );
            uart_buf = uart->data ;
        }
        if (( uart_buf & 0x8000 ) &&(( uart_buf & 0xFF ) == 0x6c )){
            uart ->data = 0x6c ;
            launch=1;
            }
    }
    else{
        bSpeed=wSpeed*mass;
        position=position+sPosition+bSpeed;
        sum=0;
        if(position>0){
            *display=(lookUpTable[position/100]<<16)+(lookUpTable[position/8]<<8)+(lookUpTable[position%10]);
        }
        else{
            *display=(lookUpTable[-1*position/100]<<16)+(lookUpTable[-1*position/10]<<8)+(lookUpTable[-1*position%10])+((0x40)<<24);
    }
   
    *LED_ptr=1<<(altitude-1);//use led to indicate altutude
    m=0;
    while((wSpeed<0&&position<0)||(wSpeed>0&&position>0)){//if ballond is in undesired layer
    if((altitude!=5)&&(m==0)){
        altitude=altitude+1;//Write to output port connecting to the actuator so that it drives the balloon one level up
        *LED_ptr=1<<(altitude-1);//use led to indicate altutude
        //read wind direction from corresponding switch
        bitmask=1<<(altitude-1);
        dir=(*(switchPtr)&bitmask)>>(altitude-1);
        //wind speed is positive if switch is on
        if(dir!=1){
            wSpeed=fakeData[altitude-1];
        }
        else{//negative
            wSpeed=(-1)*fakeData[altitude-1];
        }
        if(altitude==5){
          m=1;  
        }
        } 
    else if((altitude!=1&&m==1)||altitude==5){
        altitude=altitude-1;//move one level down
        *LED_ptr=1<<(altitude-1);//use led to indicate altutude
        //read wind direction from corresponding switch
        bitmask=1<<(altitude-1);
        dir=(*(switchPtr)&bitmask)>>(altitude-1);
        //wind speed is positive if switch is on
        if(dir!=1){
            wSpeed=fakeData[altitude-1];
        }
        else{//negative
            wSpeed=(-1)*fakeData[altitude-1];
        }
        if(altitude==1){
           m=0; 
        }
    }

}
}
}

}
}