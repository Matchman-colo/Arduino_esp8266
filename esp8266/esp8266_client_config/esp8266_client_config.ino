/*
  ESP8266-NodeMCU无线终端模式连接WiFi

  NodeMCU会使用预存的密码信息尝试连接WiFi, 
  如果同时找到多个预存WiFi，NodeMCU将会尝试连接信号最强的WiFi。
*/
 
#include <ESP8266WiFi.h>                        // 本程序使用ESP8266WiFi库
#include <ESP8266WiFiMulti.h>                   // 预存多个WiFi需要使用

WiFiClient tcpclient;
ESP8266WiFiMulti wifiMulti;                     // 新建ESP8266WiFiMulti对象,对象名称是'wifiMulti'
                                            
void setup() {
  Serial.begin(115200);                         // 初始化串口通讯波特率为115200

  /* 通过addAP函数添加需要存储的WiFi名称和密码 */
  wifiMulti.addAP("whhu", "12345678");
  wifiMulti.addAP("ESP8266", "");
  /* 可以添加更多的addAP函数来存储更多的WIFI连接信息 */

  WiFi.mode(WIFI_STA);                          // 设置Wifi工作模式为STA,默认为AP+STA模式
  Serial.println("\r\nConnecting to WiFi...");  // 串口监视器输出正在连接网络
                                      
  while (wifiMulti.run() != WL_CONNECTED)       // 通过wifiMulti.run()，NodeMCU将会在当前环境中搜索addAP函数所存储的WiFi。
  {                                             // 如果搜到多个存储的WiFi那么NodeMCU将会连接信号最强的那一个WiFi信号。
    delay(200);                                 // 一旦连接WiFI成功，wifiMulti.run()将会返回“WL_CONNECTED”。
    Serial.print("waiting for...\r\n");                                
  }                                             

  Serial.print("Connected to ");                // NodeMCU通过串口监视器输出连接的WiFI名称
  Serial.println(WiFi.SSID());
  Serial.print("Local IP address:\t");                // NodeMCU的IP地址
  Serial.println(WiFi.localIP());
}

const char* host = "192.168.4.1";
const uint16_t port = 8080;


void loop() {
  static uint8_t count = 0;
  static uint8_t first_char = '\0';

  Serial.print("connecting to host:\t");
  Serial.print(host);
  Serial.print("\tport:");
  Serial.println(port);

  if(!tcpclient.connect(host,port)){
    Serial.println("connection failed");
    delay(2000);
    return;
  }

  while(tcpclient.connected()){
    //检测server发过来的数据，并发送到串口
    while (tcpclient.available()) {
       // 读取服务器发送的数据
      size_t size = tcpclient.available();
      uint8_t buffer[size+1];
      int bytesReceived = tcpclient.readBytes(buffer, size);
      // 将接收到的数据发送到串口
      Serial.write(buffer, bytesReceived);
    }

    if (Serial.available()) {
      delay(10);
    //把串口调试器发过来的数据 发送给server
      size_t len = Serial.available();  //返回可读数据的长度
      uint8_t sbuf[len];
      Serial.readBytes(sbuf, len);
      //push UART data to server
      tcpclient.write(sbuf, len);
    }
  }

  // 断开连接
  tcpclient.stop();
  Serial.println("Disconnected from server.");       
}
