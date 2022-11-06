/*
 * @Author github.com/marcofanti89
 * 
 * Automatic reboot of raspberry:
 * Check every 30 secs that there is a TCP server listening on port 22 (SSH service)
 * If the connection is unsuccessful for too long, reboot the Rpi by unplugging and replugging it
 * 
 * Automatic reboot of router+modem
 * Check every 30 seconds that thereis connectivity with outside (trying to solve dns name using dns server 8.8.8.8)
 * If connection unsuccessful for too long, reboot both modem and router
 */

#include <SPI.h>
#include <Ethernet.h>
#include <Dns.h>

byte arduinoMac[] = { 0xA8, 0x61, 0x0A, 0xAE, 0xA8, 0x3D };

//this will only be used in case dhcp configuration fails
byte arduinoIp[] = {192,168,1,253};
byte dnsIp[] = {192,168,1,1};
byte gatewayIp[] = {192,168,1,1};
byte subnetMask[] = {255,255,255,0};

byte googleDnsIp[] = {8,8,8,8};

boolean usingDhcp;

//ip address of the raspberry pi
byte rpiIp[] = {192,168,1,2};

//30 seconds
#define INTERVAL 30000
#define RPI_MIN_FAILURES_BEFORE_REBOOT 6

//reboot router if connections fails for 3 minutes
//but don't reboot more often than 50 minutes
#define ROUTER_MIN_FAILURES_BEFORE_REBOOT 6
#define ROUTER_MIN_LOOPS_BETWEEN_REBOOTS 100

// milliseconds to keep raspberry off
#define RPI_POWEROFF_MS 10000
#define ROUTER_POWEROFF_MS 10000

//SPI pin to set to control sd card
#define SD_SELECT 4
//pin to control the relays
#define RELAY_ONE 7
#define RELAY_TWO 6

char serverNameToTestDns[] = "www.google.com";

unsigned int rpi_failure_consecutive;
unsigned int router_failures_since_last_reboot;
unsigned int router_failures_consecutive;
unsigned int router_loops_from_last_reboot;

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
  pinMode (RELAY_TWO, INPUT);
  
  // start Ethernet
  if(Ethernet.begin(arduinoMac) == 0){
    
    usingDhcp = false;
    
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found. ");
      while (true) {
        Serial.println("Doing nothing... Sorry, can't run without hardware. :(");
        delay(10000);
      }
    }
    Serial.println("Configuring Arduino with static ip");
    Ethernet.begin(arduinoMac, arduinoIp, dnsIp, gatewayIp, subnetMask);
    
  } else {
    usingDhcp = true;
  }
  
  rpi_failure_consecutive = 0;
  router_failures_consecutive = 0;
  router_failures_since_last_reboot = 0;
  router_loops_from_last_reboot = 0;
}

// ---------------------------------------------
// -------------- MAIN LOOP --------------------
// ---------------------------------------------

void loop()
{
  testRpiConnectivity();
  testDnsServerConnectivity();
  
  delay(INTERVAL);
  
  renewDhcp();
  router_loops_from_last_reboot++;
}

// ---------------------------------------------
// -------------- Functions---------------------
// ---------------------------------------------

//////////////// to Raspberry /////////////////////////////////


void testRpiConnectivity(){
  Serial.print("Connecting to RPi SSH Server... ");
  
  if(client.connect(rpiIp, 22)){
    Serial.println("SUCCESS!! ");
    client.stop();
    rpi_failure_consecutive = 0;
  } else {
    rpi_failure_consecutive++;
    Serial.println("Fail ");
    Serial.print("Current failure count: ");
    Serial.println(rpi_failure_consecutive);
  }
  
  if(rpi_failure_consecutive >= RPI_MIN_FAILURES_BEFORE_REBOOT){
    resetPin(RELAY_ONE, RPI_POWEROFF_MS);
    rpi_failure_consecutive = 0;
  }
}

/////////// to DNS Server //////////////////////////////////

void testDnsServerConnectivity(){
  DNSClient dns;
  IPAddress remote_addr;
  
  dns.begin(googleDnsIp);

  if(dns.getHostByName(serverNameToTestDns, remote_addr)){
    //when there is no coneectivity, is returning success but solved with address 0.0.0.0
    if(((byte)remote_addr[0] == 0) && ((byte)remote_addr[1] == 0) 
      && ((byte)remote_addr[2] == 0) && ((byte)remote_addr[3] == 0)){

      dnsFailure();
    } else {
      Serial.print("DNS Success, name solved to: ");
      Serial.println(remote_addr);

      router_failures_consecutive = 0;
    }
  } else {
    dnsFailure();
  }
}

void dnsFailure(){
  Serial.println("DNS Failure :(");
  router_failures_consecutive++;
  router_failures_since_last_reboot++;

  boolean shouldReboot = router_loops_from_last_reboot >= ROUTER_MIN_LOOPS_BETWEEN_REBOOTS 
    && router_failures_consecutive >= ROUTER_MIN_FAILURES_BEFORE_REBOOT;

  if(shouldReboot){
    router_failures_consecutive = 0;
    router_failures_since_last_reboot = 0;
    router_loops_from_last_reboot = 0;
    
    resetPin(RELAY_TWO, ROUTER_POWEROFF_MS);
  }
}

////////////////////////////////////////////////////////////

void renewDhcp(){
  if(!usingDhcp){
    return;
  }

  int dhcpRenewResult = Ethernet.maintain();
}

void resetPin(int pinNumber, unsigned int wait_time){

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
