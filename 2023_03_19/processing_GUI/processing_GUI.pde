import processing.serial.*;
int[] indata_a = {0,0,0,0,0,0,0,0};
int[] outdata = {0,0,0,0,0,0,0,0};
int inData=0;
Serial port;  // Create object from Serial class
int val;        // Data received from the serial port
int sum=0;

void setup() 
{
  size(1000,500);
  smooth();
  String portName ;
  println(Serial.list());
  port = new Serial(this,"COM4",115200); 
  port.bufferUntil('\n');
}

  
  
void serialEvent(Serial port)
{
 byte[] in_buff = port.readBytesUntil('\n');  // 프로세싱을 사용하려 경우 TC275의 송신 프로토콜에 마지막에 '\n'을 반드시 추가해서 넣을 것
 int byte_length = 0;
 int[] input_io_data = new int[8];
 int i;
 
 if(in_buff != null)
  {
    byte_length = in_buff.length;
    print("Data length :  ");
    println(byte_length);
    
      for(i=0;i<byte_length;i++)
      {
          print( (in_buff[i]+256)%256);  // byte의 범위가 -127+256이므로
          print("   ");
      }
      
   // println(hex(in_buff[0]));
   // println(in_buff[0]+256);
   // println(in_buff[1]+256);  
     println();
     for(i=0; i<8; i++){
       input_io_data[i]=in_buff[i+2];
     }
       if( ( (in_buff[0]+ 256) == 174) && ( (in_buff[1]+ 256) == 184)  &&  ( (in_buff[2] == 'I') ) )
        {
           println("anaylze");
           println( (in_buff[4]+256)%256);
        // java modbus crc는 복잡하므로 여기서는 생략함
        
           for(i=0;i<byte_length;i++)
           {
              input_io_data[i] = (int)((in_buff[4] >>i) & 0x01);  // input ioport data가 저장 됨
              print(input_io_data[i]);
              print("  ");
           }
              println(); println(); println();      
        }
    }
    
    for(i=0; i<8; i++){
      if(input_io_data[i]==i+1){
        outdata[i]=1;
      }
      else{
        outdata[i]=0;
      }
    }
}

void draw() {
  int i=0;
   background(65);
  fill(0,26,51);
  rect(50,300,900,60);
  fill(255,26,51);
  rect(50,120,900,60);
   
  for( i=0; i<8; i++){
   fill(0);
   ellipse(140+100*i,150,40,40);
   ellipse(160+100*i,150,40,40);
   rect(140+100*i,130,20,40);
   
   if(indata_a[i]==0){
      fill(255);
      ellipse(140+100*i,150,40,40);
    } else{
      fill(255);
      ellipse(160+100*i,150,40,40);
    }
   
   if(indata_a[i]==0){
      if((mouseX-140-100*i)*(mouseX-140-100*i)+(mouseY-150)*(mouseY-150)<=400){
        if(mousePressed){
          indata_a[i]=1;
          delay(150);
        }
      }
    } else{
      if((mouseX-160-100*i)*(mouseX-160-100*i)+(mouseY-150)*(mouseY-150)<=400){
        if(mousePressed){
          indata_a[i]=0;
          delay(150);
        }
      }
    }
    
    
   /*if(indata_a[i]==0){
      outdata[i]=0;
    } else {
      outdata[i]=1;
    }*/
    
    
   if(outdata[i]==0){
        fill(255);
      } else{
        fill(255,0,0);
      }
      ellipse(150+100*i,330,40,40);
    }
    sum=0;
   
    if(indata_a[0]==1){
       sum = sum + 1;
    }
    if(indata_a[1]==1){
       sum = sum + 2;
    }
    if(indata_a[2]==1){
       sum = sum + 4;
    }
    if(indata_a[3]==1){
       sum = sum + 8;
    }
    if(indata_a[4]==1){
       sum = sum + 16;
    }
    if(indata_a[5]==1){
       sum = sum + 32;
    }
    if(indata_a[6]==1){
       sum = sum + 64;
    }
    if(indata_a[7]==1){
       sum = sum + 128;
    }
    
    port.write(sum);

}
