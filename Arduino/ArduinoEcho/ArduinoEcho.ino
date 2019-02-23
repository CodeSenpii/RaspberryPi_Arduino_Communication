#include <Ethernet2.h>
#include <EthernetUdp2.h>

#define MAX_RCV_BUF 256
#define UDP_COM_RATE 0.5 //Hz

struct Heartbeat
{
  int count;  // 2byte == short on windows
  int randomVal; 
};  // total length == 4 bytes

byte g_mac[] = {
  0x90, 0xA2, 0xDA, 0x10, 0x3A, 0x1C
};
IPAddress g_myIp(10, 0, 0, 100);
unsigned int g_myPort = 8888;

IPAddress g_targetIp(10, 0, 0, 2);
unsigned int g_targetPort = 8000;

EthernetUDP g_handle;

byte g_rcvBuffer[MAX_RCV_BUF];

unsigned long g_timeNow = 0;
unsigned long g_lastUdpComTime = 0;

void txPacket(byte* txBuffer, int dataLen, IPAddress remoteIpNum, unsigned int remotePortNum, EthernetUDP handle)
{
    handle.beginPacket(remoteIpNum, remotePortNum);
    handle.write(txBuffer, dataLen);
    handle.endPacket();
}

bool checkForPacket(byte* rcvBuf, int maxBufLen, int* packetSize, EthernetUDP handle)
{
    //startoff assuming we dont have a packet
    bool foundPacket = false;
    
    //find the size (if any) of packet in buffer
    *packetSize = handle.parsePacket();
    
    //if the packet is -1 or 0 length (no packet) pass it, else (go inside if) it has a length, read it!
    if (*packetSize)
    {
        //Read in the packet
        handle.read(rcvBuf, maxBufLen);

        //Tell the caller we found a packet          
        foundPacket = true;
    }
    
    return foundPacket;
}

void setup() 
{
    // Setup the ethernet library
    Ethernet.begin(g_mac, g_myIp);

    // Startup serial
    Serial.begin(9600);

    // Print out begining
    Serial.println("Starting up...");
    delay(5000);

    g_handle.begin(g_myPort);
}

void loop() 
{
    // Only handle coms every second
    g_timeNow = micros();
    if(g_timeNow-g_lastUdpComTime >= (1.0/UDP_COM_RATE)*1000000.0)
    {
        Serial.println("Handle Coms v2");
        receiveHeartbeat();
        g_lastUdpComTime = g_timeNow;
    }  
}

void receiveHeartbeat()
{
    int packetSize;
    bool foundPacket = checkForPacket((byte*)&g_rcvBuffer, MAX_RCV_BUF, &packetSize,  g_handle);
    if(foundPacket)
    {
        Heartbeat* hbt = (Heartbeat*)&g_rcvBuffer;
        Serial.print("Heartbeat! Count: ");
        Serial.print(hbt->count);
        Serial.print(", random value");
        Serial.println(hbt->randomVal);
    
        sendHeartbeat(hbt->count, hbt->randomVal);
    }
}

void sendHeartbeat(int count, int val)
{
    Serial.println("Send " + count);
    // Create a packet
    Heartbeat hbt;
    hbt.count = count;
    hbt.randomVal = val;

    // Send the data
    txPacket((byte*)&hbt, sizeof(Heartbeat), g_targetIp, g_targetPort, g_handle);
}
