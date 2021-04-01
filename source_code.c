#include "address_map_arm.h"

typedef struct _timer
{
int status ;
int control ;
int low_period ;
int high_period ;
int low_counter ;
int high_counter ;
} timer ;
typedef struct _GPIO{
	int data;
	int control;
} GPIO;

typedef struct _ADC{
	int channel0;
	int channel1;
    int channel2;
} ADC;
int pow(int base, int exp){
	if (exp==0){
		return 0;
	}
	else{
		int result=base;
		while(exp!=1){
			result=result*base;
			exp--;
		}
		return result;
	}
}
//fake wind dataset
int c=0;
int fakeData[]={3000,-4000,5000,6000,4000,-5000,-3000,2000,4000,-3000,2000,-3000,2000,3000,-2000,4000,4000,3000,-2000,1000,3000,2000,4000};
int retriveWData(){
    if(c!=sizeof(fakeData)/sizeof(fakeData[0])-1){
        return fakeData[c];
        c++;
    }
    else{
        return fakeData[c];
        c=0;
    }
}


//fake pressure dataset
int fakePData[]={1,15,20,17,20,6,7,3,16,18,19,2,4,9,12,11};
int c1=0;
int retrivePData(){
    if(c1!=sizeof(fakePData)/sizeof(fakePData[0])-1){
        return fakePData[c1];
        c1++;
    }
    else{
        return fakePData[c1];
        c1=0;
    }
}



//define address variables for segment display, I/O peripheral, and timer
volatile timer * t= (timer*) TIMER_BASE;
volatile int * display= (int *) HEX3_HEX0_BASE;
volatile int * LED_ptr       = (int *)LED_BASE; // LED address

volatile GPIO * GPIOPtr= (GPIO *)JP1_BASE;
volatile ADC * ADCPtr= (ADC *)ADC_BASE;

int main(void) {

int lookUpTable[16]={0x3f,0x06,0x5B,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x67,0x77,0x7c,0x39,0x5e,0x79,0x71};
//initialize the counting interval to 0.1 seconds
int interval = 10000000;
//initialization of other predefined starting values
int mSpeed=3;//wind speed
int bSpeed=0;//ballon speed
int mass=50;//mass
int uBound=10;//upper altitude bound
int lBound=-10;//lower altitude bound
int counter,sum,fPressure,fSpeed,m,sPosition=0;
int airPressure,position,altitude=0;


//set up the timer to run continuously
t -> control = 6;

//configure GPIO port to input or output

int output_mask=pow(2,10)-1;//output mask, all bits are set to 0 except from bit 0 to bit 9(inclusive) 
int input_mask=~(pow(2,11));//input mask for wind velocity sensor
int input_mask2=~(pow(2,12));//input mask for pressure sensor
GPIOPtr->control|=output_mask;
GPIOPtr->control&=input_mask;
GPIOPtr->control&=input_mask2;
//interpret and read data from input pin corresponding to the wind speed sensor and pressure sensor

//while (true)
while(1){
/*
if (0.1 s has passed)
{
apply bit mask to retrieve wind speed and direction data from desired input pins and 
store the data into array of dimension of 100
update counter variable
}
*/
if(t->status&1){
    t->status=1;
    counter+=1;
    ADCPtr->channel0=1;
    GPIOPtr->data=ADCPtr->channel0&1045;
    mSpeed=(GPIOPtr->data)>>10;//measure speed
    mSpeed=retriveWData();
    ADCPtr->channel1=1;
    GPIOPtr->data=ADCPtr->channel1&1045;
    airPressure=(GPIOPtr->data)>>11;//measure pressure
    airPressure=0;
    sum+=mSpeed;
}
/*
if (the previous statement has been executed 100 times)
{
calculate the average value of the 10 int values and stored them into an array
use basic arithmetic to convert calculated average wind speed into actual speed of the 
balloon using mass and ratio
find out relative position with the equation (di-(v*Δt)), where di is the balloon’s initial 
position and v is the wind velocity
check the sign of the relative position
*/
if(counter==10){
    counter=0;
    mSpeed=sum/10;
    bSpeed=mSpeed/mass*0.1;
    altitude=airPressure*10;
    position=position+sPosition+mSpeed;
    sum=0;
    if(position<0){
        *display=lookUpTable[position/100]+(lookUpTable[position/10]<<8)+(lookUpTable[position%10]<<16)+lookUpTable[10]<<24;
    }
    else{
        *display=lookUpTable[position/100]+(lookUpTable[position/10]<<8)+(lookUpTable[position%10]<<16)+lookUpTable[11]<<24;
    }

}

/*
if (the sign does not equal to wind direction and altitude is less than 5)
{
Write to output port connecting to the actuator so that it drives the balloon
one level up
update altitude variable
}
*/
mSpeed=retriveWData();
if((mSpeed<0&&position<0)||(mSpeed>0&&position>0)){//if ballond is in undesired layer
    if((altitude!=uBound)&&(m==0)){
        GPIOPtr->data=1;//write to output port connecting to actuator
        //reading from sensor
        fPressure+=1;//pressure is expected to rise 
        altitude=fPressure*10;
        //updated wind velocity
        ADCPtr->channel0=1;
        GPIOPtr->data=ADCPtr->channel0&1045;
        mSpeed=(GPIOPtr->data)>>10;//measure speed
        mSpeed=retriveWData();
        m=1;
    }
    if(altitude!=lBound&&m==1){
        GPIOPtr->data=0;//write to output port connecting to actuator
        //reading from sensor
        fPressure-=1;//pressure is expected to rise 
        altitude=fPressure*10;
        //updated wind velocity
        ADCPtr->channel0=1;
        GPIOPtr->data=ADCPtr->channel0&1045;
        mSpeed=(GPIOPtr->data)>>10;//measure speed
        mSpeed=retriveWData();
        m=0;
    }
}

/*
else if (the sign does not equal to wind direction and altitude equals 5)
{
Write to output port connecting to the actuator so that it drives the balloon
one level down
update altitude variable
}
}
*/

}
}