# ESP32 Kodu

## 1. [ESP-WIFI-RICKROLLER](esp-wifi-rickroller/esp-wifi-rickroller.ino)

Her 30 saniyede bir SSID'sini (ağ adını) değiştiren bir Wi-Fi erişim noktası (AP) çalıştırır. Ağ adları, Rick Astley'in "Never Gonna Give You Up" şarkısının sözlerine dayanmaktadır.

Ağa bağlanan herkese bir karşılama ekranı (captive portal) gösterilir ve bağlantı, SPIFFS aracılığıyla ESP32 üzerinde kaydedilir. Hiçbir istemci detayı kaydedilmez, yalnızca bir bağlantının yapıldığı ve ESP32'nin açıldığı andan itibaren geçen milisaniyeye dayanan bir zaman damgası tutulur.

[esp-wifi-rickroller](https://github.com/marcelstoer/esp-wifi-rickroller) projesine dayanmaktadır, ancak günlük kaydı (logging) ve günlükleri görüntülemek/sıfırlamak için bir yönetici sayfasıyla geliştirilmiştir.

*ESP32-WROOM-32U* üzerinde test edilmiştir.