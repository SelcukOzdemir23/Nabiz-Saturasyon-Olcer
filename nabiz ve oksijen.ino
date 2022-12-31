
//MAX30100 modülünün projeye dahil edilmesi
#include "MAX30100_PulseOximeter.h"

//HC-06 Bluetooth Parçasınnın Kütüohanesinin dahil edilmesi. 
#include <SoftwareSerial.h>

//RTX ve TX bacaklarını bağladığımız 8 ve 9 numaralı pinler ile hc06 oluşturuluyor. 
SoftwareSerial hc06(8,9); 

//Rapor verilecek süre belirlerniyor
#define REPORTING_PERIOD_MS     1000

//Nabız ölçücü nesne tanımlanıyor. 
PulseOximeter pox;

// son atımın gerçekleştiği zaman
uint32_t tsLastReport = 0;

// Bir darbe ve atma algılandığında serial'e Attı yazar. 
void onBeatDetected() {
    Serial.println("Attı!");
}

void setup() {
  //Serial ve bluetooth parçası 9600 baud'da başlatılıyor. 
    Serial.begin(9600);
    hc06.begin(9600);
    //Bilgi veriliyor. 
    Serial.print("Nabız ve Satürasyon Ölçüçü Kuruluyor..");

    // Sensörünün kurulup kurulmadığını belirleme
    if (!pox.begin()) {
        Serial.println("BAŞARISIZ");
        for(;;);
    } else {
        Serial.println("BAŞARILI :-)");
    }

	// LED sürücüsü için 7,6 mA kullanacak şekilde sensörü yapılandırın
	pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);

    // Atım algılandığında yukarıda tanımladığımız method tetiklenecek. 
    pox.setOnBeatDetectedCallback(onBeatDetected);
}

void loop() {
    // Sensörden okuma yap. 
    pox.update();

    // Güncellenmiş kalp atış hızı ve SpO2 seviyelerini alın
    if (millis() - tsLastReport > REPORTING_PERIOD_MS) {

        //Serial'e nabız ve satürasyon değerlerinin düzenli şekilde yazılması. 
        Serial.print("Kalp Atış Hızı:");
        Serial.print(pox.getHeartRate());
        Serial.print("bpm / SpO2:");
        Serial.print(pox.getSpO2());
        Serial.println("%");
        
        //bluetooth modülü kullanılarak telefondaki terminale nabız ve satürasyon değerlerinin yazılması. 
        hc06.print("Kalp Atış Hızı:");
        hc06.print(pox.getHeartRate());
        hc06.print("bpm / SpO2:");
        hc06.print(pox.getSpO2());
        hc06.println("%");

        //Eğer ki nabız 70'den fazla ise modül tehlike yazıyor. Ses'te yapılabilirdi. 
         if(pox.getHeartRate()>70)
         {
           hc06.println("HASTAYI KONTROL EDİNİZ!!!");
           
           
        }

        
        //Son rapor tarihi güncelleniyor. 
        tsLastReport = millis();
    }
}