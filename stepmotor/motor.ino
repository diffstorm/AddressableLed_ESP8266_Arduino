#define PINR 4 // DIR
#define PING 5 // STEP
#define PINB 12

#define DIR_LEFT 0
#define DIR_RIGHT 1

unsigned int StepsToGo = 0;
unsigned int Direction = DIR_LEFT;
unsigned int Frequency = 5;

void SetupBoard()
{
  pinMode(PINR, OUTPUT);
  digitalWrite(PINR,0);

  pinMode(PING, OUTPUT);
  digitalWrite(PING,0);

  pinMode(PINB, OUTPUT);
  digitalWrite(PINB,0);
}

void SetFrequency(int f)
{
  Frequency = f;
  SetupFPS();
}

void SetDirection(int d)
{
  Direction = d;
  digitalWrite(PINR,d);
}

void SetSteps(int s)
{
  StepsToGo = s;
}

void HandleStepMotor()
{
  static bool pin = false;
  
  if(StepsToGo > 0)
  {
    if(pin)
    {
      StepsToGo--;
    }
    digitalWrite(PING,(pin==true)?1:0); // mosfet invert
    pin = !pin;
  }
}
