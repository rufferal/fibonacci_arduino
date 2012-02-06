//If you change these, change the bitmasks in NOR function
const byte pinA = 7;
const byte pinB = 6;
const byte inPin = A0;

unsigned long ADD(unsigned long a, unsigned long b) {
  unsigned long total = 0, s;
  byte abit, bbit, bitcount = 0, cin = 0; // cin = carry bit
  
  // In an ALU, this loop is parallelized with rows of full adders built from transistors
  while (OR(OR(a,b),cin)) { // ( a || b || cin
  
    // Can only calculate one bit at a time
    abit = a & 1; 
    bbit = b & 1;
    
    // Calculate the sum and carry bit
    // This represents a full adder
    s = XOR(cin,XOR(abit,bbit));
    cin = OR(AND(cin,XOR(abit,bbit)),AND(abit,bbit));
    
    // Place the new bit in the right place on the dword
    total |= s << bitcount++;
    
    // Move on to the next pair of bits
    a >>= 1;
    b >>= 1;
    
  }
  return total;
}

// These all decompose to a series of NOR calls -->
byte NAND(byte a, byte b) {
  return NOT(AND(a,b));
}

byte NOT(byte a) {
  return NOR(a,a);
}

byte OR(byte a, byte b) {
  return NOT(NOR(a,b));
}

byte XOR(byte a, byte b) {
  return NOR(AND(a,b),NOR(a,b));
}

byte XNOR(byte a, byte b) {
  return OR(AND(a,b),NOR(a,b));
}

byte AND(byte a, byte b) {
  return NOR(NOT(a),NOT(b));
} // <--

byte NOR(byte a, byte b) {
  const byte pin7 = 128, pin6 = 64; // (highest bits)
  
  // Using PORTD instead of digitalWrite cuts execution time by 60%
  // This implementation requires minor modification for some chips
  
  //digitalWrite(pinA,a);
  //digitalWrite(pinB,b);
  
  if (a) {
    PORTD |= pin7;
  } else {
    PORTD &= ~pin7;
  }
  
  if (b) {
    PORTD |= pin6;
  } else {
    PORTD &= ~pin6;
  }
  
  return digitalRead(inPin);
}

void setup() {
  pinMode(pinA, OUTPUT);
  pinMode(pinB, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  unsigned long a, b;
  byte term;
  
  // Wait for signal from serial
  while (!Serial.available());
  // Clear the serial buffer
  for (byte i = Serial.available(); i-- > 0; Serial.read());
  
  // First two terms are 0 and 1
  a = 0;
  b = 1;
  term = 3;
  
  Serial.println("THE FIBONACCI SERIES\n1:\t0\n2:\t1");
  
  // 47 is the limit for signed, 48 for unsigned
  while (term <= 48) {
    
      int time = micros();
      unsigned long result = ADD(a,b);
      time = micros() - time;
      
      Serial.print(term++);
      Serial.print(":\t");
      Serial.print(result);
      Serial.print(" in ");
      Serial.print(time);
      Serial.println(" microseconds.");   
      
      a = b;
      b = result;
      
  }

}
