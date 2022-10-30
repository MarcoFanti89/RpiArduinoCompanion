/*
 * @Author github.com/marcofanti89
 * 
 * Check every 30 secs that there is a TCP server listening on port 22 (SSH service)
 * If the connection is unsuccessful for too long, reboot the Rpi by unplugging and replugging it
 */

#include <SPI.h>
#include <Ethernet.h>

byte arduinoMac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA8, 0x3D };

//this will only be used in case dhcp configuration fails
byte arduinoIp[] = {192,168,1,253};
byte dnsIp[] = {192,168,1,1};
byte gatewayIp[] = {192,168,1,1};
byte subnetMask[] = {255,255,255,0};

boolean usingDhcp;

//ip address of the raspberry pi
byte rpiIp[] = {192,168,1,2};

//30 seconds
#define INTERVAL 30000
#define RPI_MAX_FAILURES_BEFORE_RETRY 6

// milliseconds to keep raspberry off
#define RPI_POWEROFF_MS 10000

//SPI pin to set to control sd card
#define SD_SELECT 4
//pin to control the relay
#define RELAY_ONE 7

int rpi_failure_count;

EthernetClient client;

// ---------------------------------------------
// -------------- SETUP ------------------------
// ---------------------------------------------

void setup()
{
  Serial.begin(9600);
  Serial.println("Arduino starting!");
  
  pinMode (SD_SELECT, OUTPUT);
  digitalWrite (SD_SELECT, HIGH); //turn SD card off so does not interfere
  
  pinMode (RELAY_ONE, INPUT); //disconnect from relay control = normally closed circuit
  
  // start Ethernet
  if(Ethernet.begin(arduinoMac) == 0){
    
    usingDhcp = false;
    
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(100); // do nothing, no point running without Ethernet hardware
      }
    }
    Ethernet.begin(arduinoMac, arduinoIp, dnsIp, gatewayIp, subnetMask);
    
  } else {
    usingDhcp = true;
  }
  
  rpi_failure_count = 0;
}

// ---------------------------------------------
// -------------- MAIN LOOP --------------------
// ---------------------------------------------

void loop()
{
  Serial.print("Connecting to RPi SSH Server... ");
  testConnectivity(rpiIp, 22, &rpi_failure_count);
  
  if(rpi_failure_count >= RPI_MAX_FAILURES_BEFORE_RETRY){
    resetPin(RELAY_ONE, RPI_POWEROFF_MS);
    rpi_failure_count = 0;
  }
  
  delay(INTERVAL);
  
  renewDhcp();
}

// ---------------------------------------------
// -------------- Functions---------------------
// ---------------------------------------------

void testConnectivity(byte address[], int port, int* failure_count){
  int connectResult = client.connect(address, port);
  client.stop();
  
  if(connectResult){
    Serial.println("SUCCESS!! ");
    *failure_count = 0;
  
  } else {
    (*failure_count)++;
    Serial.println("Fail ");
    Serial.print("Current failure count: ");
    Serial.println(*failure_count);
  }
}

void renewDhcp(){
  if(!usingDhcp){
    return;
  }

  int dhcpRenewResult = Ethernet.maintain();
}

void resetPin(int pinNumber, int wait_time){

  Serial.print("Open switch on pin ");
  Serial.print(pinNumber);
  Serial.print("... ");
  
  pinMode (pinNumber, OUTPUT);
  digitalWrite (pinNumber, LOW); //open switch
  Serial.println("DONE\n");

  delay(wait_time);
  
  Serial.print("Closing switch... ");
  pinMode (pinNumber, INPUT);
  Serial.println("DONE\n");
}
