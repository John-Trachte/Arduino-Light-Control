void setup()
{
	pinMode(3, OUTPUT); // green led
	pinMode(5, OUTPUT); // yellow 
	pinMode(6, OUTPUT); // blue 
	pinMode(9, OUTPUT); // red 
}

void loop()
{
    float greenScale = 1, yellowScale = 1, blueScale = 1, redScale = 1;

    analogWrite(3, greenScale * 255);
    analogWrite(5, yellowScale * 255);
    analogWrite(6, blueScale * 255);
    analogWrite(9, redScale * 255);
}