#include <ESP8266WiFi.h>  

IPAddress local_IP(192, 168, 4, 1);  
IPAddress gateway(192, 168, 4, 1);  
IPAddress subnet(255, 255, 255, 0);  

#define MAX_SRV_CLIENTS 4

WiFiClient serverClients[MAX_SRV_CLIENTS];   //定义最多多少个client可以连接本server(一般不要超过4个)
WiFiServer server(local_IP, 8080);

void setup() {  
  // put your setup code here, to run once:  
  Serial.begin(115200);  
  WiFi.softAPConfig(local_IP, gateway, subnet);  
  WiFi.softAP("SoftAP001", "abcdefg");  
  Serial.print("Soft-AP IP address = ");  
  Serial.println(WiFi.softAPIP());  
  
  server.begin();              //启动服务器
  server.setNoDelay(true);     //关闭小包合并包功能，不会延时发送数据
  Serial.print("OK, server's ready. Its IP is: ");
  Serial.print(WiFi.localIP());
  Serial.println("      its port is: 8080");

}  
  
void loop() {  
  // put your main code here, to run repeatedly:  
  uint8_t i;
  
  if (server.hasClient()) {  //判断是否有新的client请求进来
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      
      //释放旧无效或者断开的client
      if (!serverClients[i] || !serverClients[i].connected()) {
        if (!serverClients[i]) {
          //serverClients[i]    判断指定序号的客户端是否有效
          serverClients[i].stop();  //停止指定客户端的连接
        }
        
        serverClients[i] = server.available();//分配最新的client
        Serial.print("A new client: "); 
        Serial.println(i);
        break; //跳出一层for循环
      }
    }
    
    //当达到最大连接数 无法释放无效的client，需要拒绝连接
    if (i == MAX_SRV_CLIENTS) {
      WiFiClient client = server.available();
      client.stop();
      Serial.println("Connection is refused.");
    }
  }

  
  //检测client发过来的数据，并发送到串口
  for (i = 0; i < MAX_SRV_CLIENTS; i++) {
    if (serverClients[i] && serverClients[i].connected()) {
      if (serverClients[i].available()) {
        //serverClients[i].available()   判断指定客户端是否有可读数据
        while (serverClients[i].available()) {
          Serial.write(serverClients[i].read());
        }
      }
    }
  }
 
  if (Serial.available()) {
    //把串口调试器发过来的数据 发送给client
    size_t len = Serial.available();  //返回可读数据的长度
    uint8_t sbuf[len];
    Serial.readBytes(sbuf, len);
    //push UART data to all connected telnet clients
    for (i = 0; i < MAX_SRV_CLIENTS; i++) {
      if (serverClients[i] && serverClients[i].connected()) {
        serverClients[i].write(sbuf, len);//向客户端发送数据
        delay(1);
      }
    }
  }
}