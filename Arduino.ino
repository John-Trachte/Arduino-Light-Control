void setup()
{
  Serial.begin(9600);
  
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, OUTPUT);
}

int greenScale = 1, yellowScale = 1, blueScale = 1, redScale = 1;


void loop()
{
    greenScale = Serial.read();
    yellowScale = Serial.read();
  	blueScale = Serial.read();
    redScale = Serial.read();

    analogWrite(3, (greenScale * 255) / 1000);
    analogWrite(5, (yellowScale * 255) / 1000);
    analogWrite(6, (blueScale * 255) / 1000);
    analogWrite(9, (redScale * 255)  / 1000);
}