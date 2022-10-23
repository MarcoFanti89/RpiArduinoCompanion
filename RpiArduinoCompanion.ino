/*
 * @Author github.com/marcofanti89
 * 
 * Check every 30 secs that there is a TCP server listening on port 22 (SSH service)
 * If the connection is unsuccessful for 5 minutes
  
 */

#include <SPI.h>         
#include <Ethernet.h>

//mac address for ethernet shield c5:9b:ab:ab:22:31
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
#define MAX_FAILURES_BEFORE_RETRY 10

// milliseconds the RUN pin will be set to zero
//to reboot the rpi
#define RESET_MS 200

//SPI pin to set to control sd card
#define SD_SELECT 4
//pin RUN of the rpi3
#define RPI_RUN 7

int rpi_intervals_from_last_success;

EthernetClient client;

void setup() 
{
  Serial.begin(9600);

  pinMode (SD_SELECT, OUTPUT);
  digitalWrite (SD_SELECT, HIGH); //turn SD card off so doe not interfere

  pinMode (RPI_RUN, INPUT);
  
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
    
  rpi_intervals_from_last_success = 0;
}

void loop()
{
  Serial.print("Connecting to RPi SSH Server... ");
  int connectResult = client.connect(rpiIp, 22);
  
  if(connectResult){
    Serial.println("SUCCESS!! ");
    
    rpi_intervals_from_last_success = 0;
  } else {
    Serial.print("FAILED :( Last successful connection was ");
    Serial.print(rpi_intervals_from_last_success);
    Serial.println(" cycles ago");
  }
  client.stop();

  delay(INTERVAL);

  if(rpi_intervals_from_last_success >= MAX_FAILURES_BEFORE_RETRY){
    resetPi();
  }
  
  rpi_intervals_from_last_success++;
  renewDhcp();
}

void renewDhcp(){
  if(!usingDhcp){
    return;
  }

  int dhcpRenewResult = Ethernet.maintain();
  Serial.print("Dhcp renew: ");
  Serial.println(dhcpRenewResult);
}

void resetPi(){
  Serial.print("Reboot button pressed... ");
  pinMode (RPI_RUN, OUTPUT);
  digitalWrite (RPI_RUN, LOW);
  delay(RESET_MS);
  pinMode (RPI_RUN, INPUT);
  Serial.println("DONE\n");
  rpi_intervals_from_last_success = 0;
}
