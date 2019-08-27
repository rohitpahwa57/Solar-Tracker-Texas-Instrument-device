#include "driverlib.h"
#include "device.h"
#include "math.h"
//MOTOR MOVE PINS BELOW
# define LOOP_COUNT 10
# define PIN1A 0
# define PIN1B 1
# define PIN2A 2
# define PIN2B 3
# define PIN3A 4
# define PIN3B 5
# define PIN4A 6
# define PIN4B 7
//ALL CLOCK VARIABLE BELOW
#define CLOK 64
#define RST 66
#define DATA 63
#define button 67
#define DEG_TO_RAD 0.01745329
#define PI 3.141592654
#define TWOPI 6.28318531

int Clockdata[64], Seco,MIN,HOUR ,DAY,MONTH ,YEAR;
float ye,da ,mo ,ho ,mi, sec;

//BOTH ENCODER VARIABLE
int encpina = 58;
int encpinb = 59;
int encpos = 0;
int encpinaLast = 0;
int n = 0;
int encpinaDO = 60;
int encpinbDO = 61;
int encposDO = 0;
int encpinaLastDO = 0;
int nDO = 0;

// SUN VARIABLE BELOW
float Azimu, Elevat;
int azih, elevh;
//CONVERTING DEGREE TO POS VARIABLE
float abu=0, babu=0;
//MOTOR MOVE VARIABLE BELOW
int azi,zen;
int Buttonstate=0;
void readbutton();
void settime();
void Whatsthetime();
void getangle();
long int JulianDate(int YEAR, int MONTH, int DAY);
void reset();
void movemotor();
void readposition();
void movemotorDO();
void readpositionDO();
void angletopos();

void main(void)
{
    Device_init();
    Device_initGPIO();
    GPIO_setPadConfig(PIN1A, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN1A, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN1B, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN1B, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN2A, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN2A, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN2B, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN2B, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN3A, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN3A, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN3B, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN3B, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN4A, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN4A, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(PIN4B, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(PIN4B, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(CLOK, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(CLOK, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(RST, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(RST, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(DATA, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(DATA, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(button, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(button, GPIO_DIR_MODE_IN);

    Interrupt_initModule();
    Interrupt_initVectorTable();
    EINT;
    ERTM;
    for(;;)
    {
    readbutton();
    Whatsthetime();
    settime();
    if(Buttonstate==1)
    {
        getangle();
    }
    else
    {
        reset();
    }
    angletopos();
    movemotor();
    movemotorDO();
    }

}
void readbutton()
{   int state, prevstate=0;
    state =GPIO_readPin(button);
    if (prevstate != state)
    {
        Buttonstate= Buttonstate+1;

        DEVICE_DELAY_US(1000000);
    }
    prevstate = state;
    if (Buttonstate > 3)
    {
        Buttonstate=0;
    }
}
void Whatsthetime()
{
    int i;
    GPIO_setDirectionMode(DATA, GPIO_DIR_MODE_OUT);
    GPIO_writePin(RST, 1);
    DEVICE_DELAY_US(4);
    for(i=0;i<=7;i++)
    {
        if(i==0||i==1||i==2||i==3||i==4||i==5)
        {
            GPIO_writePin(DATA, 1);
            GPIO_writePin(CLOK, 1);
            DEVICE_DELAY_US(1);
            GPIO_writePin(CLOK, 0);
            DEVICE_DELAY_US(1);
        }
        else if(i == 6)
        {
        GPIO_writePin(DATA, 0);
        GPIO_writePin(CLOK, 1);
        DEVICE_DELAY_US(1);
        GPIO_writePin(CLOK, 0);
        DEVICE_DELAY_US(1);
        }
        else
        {
        GPIO_writePin(DATA, 1);
        GPIO_writePin(CLOK, 1);
        DEVICE_DELAY_US(1);
        GPIO_writePin(DATA, 0);
        GPIO_setDirectionMode(DATA, GPIO_DIR_MODE_IN);
        }
    }
    for(i=1;i<=64;i++)
    {
        GPIO_writePin(CLOK, 1);
        DEVICE_DELAY_US(1);
        GPIO_writePin(CLOK, 0);
        DEVICE_DELAY_US(1);
        Clockdata[i]=GPIO_readPin(DATA);
    }
    GPIO_writePin(RST, 0);
}
void settime()
{
    int i,base,num,decimal_val,rem;
    sec=0;
    mi=0;
    ho=0;
    da=0;
    mo=0;
    ye=0;
    num = 0;
    for(i=4;i<=64;i+=4)
    {
        num = (Clockdata[i]*1000)+(Clockdata[i-1]*100)+(Clockdata[i-2]*10)+(Clockdata[i-3]*1);
        base =1;
        decimal_val=0;
        while(num>0)
        {
            rem = num % 10;
            decimal_val=decimal_val+rem*base;
            num = num/10;
            base=base*2;
        }
       num=0;
       if(i==4||i==8)
       {
       sec=(sec/10)+decimal_val;
       }
       else if(i==12||i==16)
       {
       mi=(mi/10)+decimal_val;
       }
       else if(i==20||i==24)
       {
       ho=(ho/10)+decimal_val;
       }
       else if(i==28||i==32)
       {
       da=(da/10)+decimal_val;
       }
       else if(i==36||i==40)
       {
       mo=(mo/10)+decimal_val;
       }
       else if(i==52||i==56)
       {
       ye=(ye/10)+decimal_val;
       }
       else{}
     }
     mo= mo+0.01;
     Seco= sec*10;
     MIN= mi*10;
     HOUR= ho*10;
     DAY= da*10;
     MONTH= mo*10;
     YEAR=ye*10;
}
void getangle()
{
   // int HOUR,MIN=0,Seco=0,MONTH=4,DAY=1,YEAR;
    float Lon=-1.61*DEG_TO_RAD, Lat=54.98*DEG_TO_RAD;
    float T,JD_frac,L0,M,e,C,L_true,f,GrHrAngle,Obl,RA,Decl,HrAngle,elev,azimuth,R,jk;
    long int JD_whole,JDx;

        HOUR = HOUR ;
        JD_whole=JulianDate(YEAR,MONTH,DAY);
        JD_frac=(HOUR+MIN/60.+Seco/3600.)/24.-.5;
        T=JD_whole-2451545; T=(T+JD_frac)/36525.;
        L0=DEG_TO_RAD*fmod(280.46645+36000.76983*T,360);
        M=DEG_TO_RAD*fmod(357.5291+35999.0503*T,360);
        e=0.016708617-0.000042037*T;
        C=DEG_TO_RAD*((1.9146-0.004847*T)*sin(M)+(0.019993-0.000101*T)*sin(2*M)+0.00029*sin(3*M));
        f=M+C;
        Obl=DEG_TO_RAD*(23+26/60.+21.448/3600.-46.815/3600*T);
        JDx=JD_whole-2451545;
        GrHrAngle=280.46061837+(360*JDx)%360+.98564736629*JDx+360.98564736629*JD_frac;
        GrHrAngle=fmod(GrHrAngle,360.);
        L_true=fmod(C+L0,TWOPI);
        R=1.000001018*(1-e*e)/(1+e*cos(f));
        RA=atan2(sin(L_true)*cos(Obl),cos(L_true));
        Decl=asin(sin(Obl)*sin(L_true));
        HrAngle=DEG_TO_RAD*GrHrAngle+Lon-RA;
        elev=asin(sin(Lat)*sin(Decl)+cos(Lat)*(cos(Decl)*cos(HrAngle)));
        azimuth=PI+atan2(sin(HrAngle),cos(HrAngle)*sin(Lat)-tan(Decl)*cos(Lat));
        // Azimuth measured eastward from north.
        jk=R;
        R=jk;
    Azimu = azimuth/DEG_TO_RAD;
    Elevat = elev/DEG_TO_RAD;

    azih= Azimu;
    elevh= Elevat;

}

long int JulianDate(int year, int month, int day)
  {
  long JD_whole;
  int A,B;
  if (month<=2)
    {
      year--; month+=12;
    }
  A=year/100; B=2-A+A/4;
  JD_whole=(long)(365.25*(year+4716))+(int)(30.6001*(month+1))+day+B-1524;
  return JD_whole;
}
void reset()
{
azih =180;
elevh=0;
}

void angletopos()
{
    float h=2.77;
    if (azih<=270 && azih>=90 && elevh<=90 && elevh>=0)
    {
    abu = azih-180;
    azi= abu*h;
    babu= elevh;
    zen=babu*h;
    }
}
void movemotor()
{
    for(;;)
    {
        readposition();
        if(encpos<zen-1)
        {           readposition();
                    readposition();
                    readposition();
                    GPIO_writePin(PIN1B, 1);
                    GPIO_writePin(PIN2A, 1);
                    readposition();
                    readposition();
                    readposition();
                    readposition();
                    DEVICE_DELAY_US(250);
                    readposition();
                    readposition();
                    readposition();
                    readposition();
                    GPIO_writePin(PIN1B, 0);
                    GPIO_writePin(PIN2A, 0);
                    readposition();
                    DEVICE_DELAY_US(750);
                    readposition();
        }

        else if(encpos>zen+1)
        {
            readposition();
            readposition();
            readposition();
            GPIO_writePin(PIN1A, 1);
            GPIO_writePin(PIN2B, 1);
            readposition();
            readposition();
            readposition();
            readposition();
            DEVICE_DELAY_US(250);
            readposition();
            readposition();
            readposition();
            readposition();
            GPIO_writePin(PIN1A, 0);
            GPIO_writePin(PIN2B, 0);
            DEVICE_DELAY_US(750);
            readposition();
        }
        else
        {
            break;
         }

    }
}
void readposition()
{

    n = GPIO_readPin(encpina);
    if ((encpinaLast == 0) && (n == 1))
        {
      if (GPIO_readPin(encpinb) == 0)
          {
              encpos--;
          }
        else
            {
              encpos++;
             }
        }
        encpinaLast = n;
}
void movemotorDO()
{
    for(;;)
    {
        readpositionDO();
        if(encposDO<azi-5)
        {           readpositionDO();
                    readpositionDO();
                    readpositionDO();
                    GPIO_writePin(PIN3B, 1);
                    GPIO_writePin(PIN4A, 1);
                    readpositionDO();
                    readpositionDO();
                    readpositionDO();
                    readpositionDO();
                    DEVICE_DELAY_US(300);
                    readpositionDO();
                    readpositionDO();
                    readpositionDO();
                    readpositionDO();
                    GPIO_writePin(PIN3B, 0);
                    GPIO_writePin(PIN4A, 0);
                    readpositionDO();
                    DEVICE_DELAY_US(700);
                    readpositionDO();
        }

        else if(encposDO>azi+5)
        {   readpositionDO();
            readpositionDO();
            readpositionDO();
            GPIO_writePin(PIN3A, 1);
            GPIO_writePin(PIN4B, 1);
            readpositionDO();
            readpositionDO();
            readpositionDO();
            readpositionDO();
            DEVICE_DELAY_US(300);
            readpositionDO();
            readpositionDO();
            readpositionDO();
            readpositionDO();
            GPIO_writePin(PIN3A, 0);
            GPIO_writePin(PIN4B, 0);
            DEVICE_DELAY_US(700);
            readpositionDO();
        }
        else
        {
            break;
         }

    }
}
void readpositionDO()
{

    nDO = GPIO_readPin(encpinaDO);
    if ((encpinaLastDO == 0) && (nDO == 1))
        {
      if (GPIO_readPin(encpinbDO) == 0)
          {
              encposDO++;
          }
        else
            {
              encposDO--;
             }
        }
        encpinaLastDO = nDO;
}
