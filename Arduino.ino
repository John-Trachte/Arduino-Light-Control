void setup()
{
  Serial.begin(9600);
  
  pinMode(3, OUTPUT); // green,   top left
  pinMode(5, OUTPUT); // yellow,  top right
  pinMode(6, OUTPUT); // blue,    bottom left
  pinMode(9, OUTPUT); // red,     bottom right
}

char color;

void loop()
{

}

void serialEvent(){
  color = Serial.read();
  switch(color) {
    case 'G':
      analogWrite(3, 255);
      analogWrite(5, 0);
      analogWrite(6, 0);
      analogWrite(9, 0);
    break;

    case 'B':
      analogWrite(3, 0);
      analogWrite(5, 0);
      analogWrite(6, 255);
      analogWrite(9, 0);
    break;

    case 'Y':
      analogWrite(3, 0);
      analogWrite(5, 255);
      analogWrite(6, 0);
      analogWrite(9, 0);
    break;

    case 'R':
      analogWrite(3, 0);
      analogWrite(5, 0);
      analogWrite(6, 0);
      analogWrite(9, 255);
    break;    
  }
}