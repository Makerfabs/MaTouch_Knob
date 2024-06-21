#include "WifiAsyncWebServer.h"
extern lv_obj_t *img1;
const char *host = "dial";
const int default_webserverporthttp = 80;

AsyncWebServer *server;  // initialise webserver

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<head>
	<meta charset="utf-8">
	<title>wifi config</title>
</head>
<body>
		<div>
			<div>
				WIFI配网
			</div>
			<div>
				<span>WIFI名称:</span> <input type="text" id="wifi">
			</div>
			<div>
				<span>密&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp&nbsp码:</span> <input type="password" id="code">
			</div>
			<input type="submit" class="btn" value="设 置wifi" onclick="send_wifi()">

			            <p>换图功能支持png和jpg,屏幕分辨率240*240  建议指针分辨率为32*32的png图片   换完图片后需要重启才能生效</p>
			<p>图片预览：</p>
			<img id="image-preview" src="" height="240"alt="Image preview...">
			<form id="form" enctype="multipart/form-data" method="post">
            <input type="file" accept="image/png,image/jpeg" name="img1" id="img1"  onchange="onchangeFile('img1')" />
            <input type="button" onclick="uploadFile('img1')" value="上传主页面"/>
            </form>

			<form id="form" enctype="multipart/form-data" method="post">
            <input type="file" accept="image/png,image/jpeg" name="img2" id="img2" onchange="onchangeFile('img2')" />
            <input type="button" onclick="uploadFile('img2')" value="上传旋钮页面"/>
            </form>

			<form id="form" enctype="multipart/form-data" method="post">
            <input type="file" accept="image/png,image/jpeg" name="pointer" id="pointer" onchange="onchangeFile('pointer')" />
            <input type="button" onclick="uploadFile('pointer')" value="上传指针"/>
            </form>

            <p>Free Storage: %FREESPIFFS% | Used Storage: %USEDSPIFFS% | Total Storage: %TOTALSPIFFS%</p>
			<p>%FILELIST%</p>
            <form id="form1" enctype="multipart/form-data" method="post" action="/update">
				<input type="file" name="update" id="update" />
            <input type="button" onclick="uploadbin()" value="上传bin文件"/>
            </form>
             <div class="percentage">上传进度0</div>
        </div>
</body>
<script>
	function send_wifi() {
		var input_ssid = document.getElementById("wifi").value;
		var input_code = document.getElementById("code").value;
		var xhttp = new XMLHttpRequest();
		xhttp.open("POST", "/wifi_data", true);
		xhttp.onreadystatechange = function() {
		    if (xhttp.readyState == 4) {
		         if (xhttp.status == 200) {
						alert(账号密码设置成功,WIFI将会重新连接);
		            console.log(xhttp.responseText);
		        } else if (xhttp.status == 0) {
		            alert("Server closed the connection abruptly!");
		            location.reload()
		       } else {
		            alert(xhttp.status + " Error!\n" + xhttp.responseText);
		           location.reload()
		        }
		    }
		 };
		var data = {
			"wifi_name": input_ssid,
			"wifi_code": input_code
		}
		xhttp.send("wifi_name="+input_ssid+"&wifi_code="+input_code);
	}
	function onchangeFile(params){
		let preview = document.getElementById('image-preview');
		let fileInput = document.getElementById(params);
		// 清除进度条
		document.querySelector(".percentage").innerHTML = "上传进度"+"0"
		// 清除背景图片:
				preview.style.backgroundImage = '';
				if (!fileInput.value) {
					return;
				}
				let file = fileInput.files[0];
				let size = file.size;
				if (size >= 200 * 1024) {
					alert('文件大小超出限制,最大不能超过200KB');
					return false;
				}
				// 获取File信息:
				if (!['image/jpeg', 'image/png'].includes(file.type)) {
					alert('不是有效的图片文件!');
					return;
				}
				// 读取文件:
				let reader = new FileReader();
				reader.onload = function(e) {
					let data = e.target.result;
					console.log(preview, 'a标签')
					preview.src = data
				};
				// 以DataURL的形式读取文件:
				reader.readAsDataURL(file);
	}
	function uploadFile(params) {
        var fd = new FormData();
        var file = document.getElementById(params).files[0];
        // 获取File信息:
		if (!['image/jpeg', 'image/png'].includes(file.type)) {
		alert('不是有效的图片文件!');
		return;
		}
		if(['image/jpeg'].includes(file.type))
        	fd.append(params, file,params+".jpg");
        else
        	fd.append(params, file,params+".png");
        var xhr = new XMLHttpRequest();
            xhr.upload.addEventListener("progress", function (e) {
                let percentage = e.loaded / e.total * 100
                document.querySelector(".percentage").innerHTML = "上传进度"+`${percentage.toFixed(2)}`
            })
        xhr.open("POST", "/upload");
        xhr.send(fd);
      }
    function uploadbin() {
        var fd = new FormData();
        fd.append("update", document.getElementById("update").files[0]);
        var xhr = new XMLHttpRequest();
            xhr.upload.addEventListener("progress", function (e) {
                let percentage = e.loaded / e.total * 100
                document.querySelector(".percentage").innerHTML = "上传进度"+`${percentage.toFixed(2)}`
            })
        xhr.open("POST", "/update");
        xhr.send(fd);
      }
</script>
)rawliteral";

//format bytes
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}
bool exists(String path) {
  bool yes = false;
  File file = FILESYSTEM.open(path, "r");
  if (!file.isDirectory()) {
    yes = true;
  }
  file.close();
  return yes;
}
String listFiles(bool ishtml) {
  // server.sendHeader("Connection", "close");
  String path = "/";
  Serial.println("handleFileList: " + path);


  File root = FILESYSTEM.open(path);
  path = String();

  String output = "";
  if (root.isDirectory()) {
    File file = root.openNextFile();
    while (file) {
      if (output != "") {
        output += "<br/>";
      }
      output += "{\"type\":\"";
      output += (file.isDirectory()) ? "dir" : "file";
      output += "\",\"name\":\"";
      output += String(file.path()).substring(1);
      output += "\"}";
      file = root.openNextFile();
    }
  }
  return output;
}
String processor(const String &var) {
  if (var == "FILELIST") {
    return listFiles(true);
  }
  if (var == "FREESPIFFS") {
    return formatBytes((FILESYSTEM.totalBytes() - FILESYSTEM.usedBytes()));
  }

  if (var == "USEDSPIFFS") {
    return formatBytes(FILESYSTEM.usedBytes());
  }

  if (var == "TOTALSPIFFS") {
    return formatBytes(FILESYSTEM.totalBytes());
  }

  return String();
}
void handleWifiConfig(AsyncWebServerRequest *request) {
#if 0  //调试用
  Serial.println("post wifi config");
  Serial.println(request->args());
  Serial.println(request->version());
  Serial.println(request->method());
  Serial.println(request->url());
  Serial.println(request->host());
  Serial.println(request->contentType());
  Serial.println(request->contentLength());
  Serial.println(request->multipart());

  int headers = request->headers();
  int i;
  for(i=0;i<headers;i++){
    AsyncWebHeader* h = request->getHeader(i);
    Serial.printf("HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    // 下面行功能同上面两行
    //Serial.printf("HEADER[%s]: %s\n", request->headerName(i).c_str(), request->header(i).c_str()); 
  }
  int params = request->params();
  for(int i=0;i<params;i++){
    AsyncWebParameter* p = request->getParam(i);
    if(p->isFile()){ //p->isPost() is also true
      Serial.printf("FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
    } else if(p->isPost()){
      Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
    } else { //GET
      Serial.printf("GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
    }
  }
#endif
  //  Serial.println(request->argName(0));     // get request argument name by number

  if (request->hasArg("wifi_name")) {
    const String wifi_name = request->arg("wifi_name");
    const String wifi_pass = request->arg("wifi_code");
    Serial.println(wifi_name);
    Serial.println(wifi_pass);

    Preferences prefs;     // 声明Preferences对象
    prefs.begin("frist");  // 打开命名空间mynamespace
    Serial.println(prefs.putString("wifi_name", wifi_name));
    Serial.println(prefs.putString("wifi_code", wifi_pass));
    prefs.end();
    if (wifi_name.length() > 0) {
      ESP.restart();
    }
    request->send_P(200, "text/html", "seccess");
  }
}
// handles uploads   用于图片上传
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);

  if (!index) {
    logmessage = "Upload Start: " + String(filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = FILESYSTEM.open("/" + filename, "w");
    Serial.println(logmessage);
  }

  if (len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
    Serial.println(logmessage);
  }

  if (final) {
    logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    Serial.println(logmessage);
    Serial.println(filename);
    if (filename.indexOf("jpg") > 0) {
      filename.replace("jpg", "png");
      Serial.println(filename);
      FILESYSTEM.remove("/" + filename);

    } else {
      filename.replace("png", "jpg");
      Serial.println(filename);
      FILESYSTEM.remove("/" + filename);
    }

    request->redirect("/");
  }
}
// handles updata   用于ota
void handleUpdate(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);

  if (!index) {  //开始上传
    logmessage = "Upload Start: " + String(filename);
    if (!Update.begin()) {  //start with max available size
      Update.printError(Serial);
    }
    Serial.println(logmessage);
  }

  if (len) {
    // stream the incoming chunk to the opened file
    if (Update.write(data, len) != len) {
      Update.printError(Serial);
    }
    logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
    Serial.println(logmessage);
  }

  if (final) {  //bin文件上传完成  重启esp32
    logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
    if (Update.end(true)) {  //true to set the size to the current progress
      Serial.println(logmessage);
    } else {
      Update.printError(Serial);
    }
    Serial.printf("Rebooting...\n");
    request->send_P(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }
}
void configureWebServer() {
  server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + +" " + request->url();
    Serial.println(logmessage);
    request->send_P(200, "text/html", index_html, processor);
  });
  // run handleUpload function when any file is uploaded
  server->on(
    "/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(200);
    },
    handleUpload);
  // run handleUpdate function when bin file is uploaded
  server->on(
    "/update", HTTP_POST, [](AsyncWebServerRequest *request) {
      request->send(200);
    },
    handleUpdate);
  server->on("/wifi_data", HTTP_POST, handleWifiConfig);
}
void ffat_init() {
  //第一次烧录程序 初始化FFat
  Preferences prefs;
  prefs.begin("frist");          // 打开命名空间mynamespace
  if (prefs.getUInt("ffat", 0))  // 获取当前命名空间中的键名为"ffat"的值   如果没有该元素则返回默认值0
  {
    if (!FILESYSTEM.begin(FORMAT_FILESYSTEM_IF_FAILED, "/LFS")) {
      Serial.println("FFat Mount Failed");
      return;
    }
    Serial.printf("FFAT启动\n");
    Serial.printf("Total space: %10u\n", FILESYSTEM.totalBytes());
    Serial.printf("Free space: %10u\n", FILESYSTEM.freeBytes());
    File root = FILESYSTEM.open("/");
    File file = root.openNextFile();
    while (file) {
      String fileName = file.name();
      size_t fileSize = file.size();
      Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), formatBytes(fileSize).c_str());
      file = root.openNextFile();
    }
    file.close();
    Serial.printf("\n");
  } else {
    FILESYSTEM.format();
    Serial.printf("ffat初始化成功\n");
    prefs.putUInt("ffat", 1);  // 将数据保存到当前命名空间的"count"键中
    Serial.printf("重启ESP32\n");
    ESP.restart();
  }
  prefs.end();  // 关闭当前命名空间
}
void clear_wifi_name()
{
  Preferences prefs;     // 声明Preferences对象
    prefs.begin("frist");  // 打开命名空间mynamespace
    prefs.remove("wifi_name"); 
    Serial.printf("wifi_name: %s\n\n", prefs.getString("wifi_name", "not found"));
    prefs.end();
}
void wifi_server_begin(void *parameter) {
  // put your setup code here, to run once:
  char wifi_mode;
  wifi_mode = 0;  //没有状态
  //第一次烧录程序 初始化FFat
  Preferences prefs;                                  // 声明Preferences对象
  prefs.begin("frist");                               // 打开命名空间mynamespace
  if (prefs.getString("wifi_name", "").length() > 1)  // 获取当前命名空间中的键名为"ffat"的值   如果没有该元素则返回默认值0
  {
    String str1 = prefs.getString("wifi_name", "");
    const char *wifi_name = str1.c_str();  //去掉const会显示编译错误
    Serial.println(wifi_name);
    String str2 = prefs.getString("wifi_code", "");
    const char *wifi_pass = str2.c_str();  //去掉const会显示编译错误
    Serial.println(wifi_pass);
    Serial.printf("有wifi名字\n");

    //WIFI INIT
    Serial.printf("Connecting to %s\n", wifi_name);
    WiFi.mode(WIFI_STA);
    WiFi.begin(wifi_name, wifi_pass);
    wifi_mode = 1;  //  STA模式
    int wifi_max_count = 0;

    String wifi_count = "";
    while (WiFi.status() != WL_CONNECTED) {
      wifi_max_count++;
      vTaskDelay(500);
      Serial.print(".");
      wifi_count += ".";
      lv_label_set_text(ui_Label3, wifi_count.c_str());
      if (wifi_max_count == MAX_STA_COUNT) {
        wifi_mode = 2;
            prefs.remove("wifi_name"); 
        Serial.printf("wifi_name: %s\n\n", prefs.getString("wifi_name", "not found"));
        // clear_wifi_name();
        break;
      }
    }
  } else {
    Serial.printf("没有wifi名字\n");
    wifi_mode = 2;
  }
  if (wifi_mode == 1) {
    Serial.println("WIFI-STA连接成功");
    Serial.println("\n\nNetwork Configuration:");
    Serial.println("----------------------");
    Serial.print("         SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("  Wifi Status: ");
    Serial.println(WiFi.status());
    Serial.print("Wifi Strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("          MAC: ");
    Serial.println(WiFi.macAddress());
    Serial.print("           IP: ");
    Serial.println(WiFi.localIP());
    lv_label_set_text(ui_Label3, ("STA:" + WiFi.localIP().toString()).c_str());
  }
  if (wifi_mode == 2) {
    char mac_tmp[6];
    const char *ssid = mac_tmp;
    sprintf(mac_tmp, "%02X\r\n", (uint32_t)(ESP.getEfuseMac() >> (24)));
    sprintf(mac_tmp, "ESP32-%c%c%c%c%c%c", mac_tmp[4], mac_tmp[5], mac_tmp[2], mac_tmp[3], mac_tmp[0], mac_tmp[1]);
    WiFi.mode(WIFI_AP);
    while (!WiFi.softAP(ssid, "")) {};  //启动AP
    Serial.println("AP启动成功");
    Serial.print("ip:");
    Serial.println(WiFi.softAPIP());
    lv_label_set_text(ui_Label3, ("AP:" + WiFi.softAPIP().toString()).c_str());


  }
  prefs.end();  // 关闭当前命名空间
  MDNS.begin(host);
  Serial.print("Open http://");
  Serial.print(host);
  Serial.println(".local to see the file browser");


  // configure web server
  Serial.println("\nConfiguring Webserver ...");
  server = new AsyncWebServer(default_webserverporthttp);
  configureWebServer();

  // startup web server
  Serial.println("Starting Webserver ...");
  server->begin();
  while (1) {
    vTaskDelay(10);
  }
}