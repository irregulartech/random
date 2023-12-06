/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5Core sample source code
*                          配套  M5Core 示例源代码
* Visit for more information: https://docs.m5stack.com/en/unit/uhf_rfid
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/unit/uhf_rfid
* Modified 2023 by Zero_Chaos
*
* Product: UHF RFID UNIT
* Date: 2022/7/8
*******************************************************************************
Please connect to Port C / 请连接端口C
How to use: / 如何使用
1. Polling Card EPC
2. Select Card EPC
3. Write or Read Card Info
*/

#include <M5Core2.h>
#include <M5GFX.h>

#include <Unit_UHF_RFID.h>

M5GFX display;
M5Canvas canvas(&display);
Unit_UHF_RFID uhf;

String info = "";

void setup() {
    M5.begin();  // Init M5Core.  初始化 M5Core
    // Serial2.begin(unsigned long baud, uint32_t config, int8_t rxPin, int8_t
    // txPin, bool invert) uhf.begin(HardwareSerial *serial = &Serial2, int
    // baud=115200, uint8_t RX = 16, uint8_t TX = 17, bool debug = false);
    uhf.begin(&Serial2, 115200, 33, 32, false);
    //   uhf.begin();
    while (1) {
        info = uhf.getVersion();
        if (info != "ERROR") {
            Serial.println(info);
            break;
        }
    }

    // max: 26dB
    uhf.setTxPower(2600);

    display.begin();
    canvas.setColorDepth(1);  // mono color
    canvas.setFont(&fonts::efontCN_14);
    canvas.createSprite(display.width(), display.height());
    canvas.setTextSize(2);
    canvas.setPaletteColor(1, GREEN);
    canvas.setTextScroll(true);
    canvas.println(info);
    canvas.println("1.BtnB Polling Card EPC");
    canvas.println("2.BtnC Select Card EPC");
    canvas.println("3.BtnA Write or Read Card Info");
    showBattery();
    canvas.pushSprite(0, 0);
}

uint8_t write_buffer[]  = {0xab, 0xcd, 0xef, 0xdd};
uint8_t reade_buffer[4] = {0};

void log(String info) {
    Serial.println("Write Data...");
    canvas.println(info);
    canvas.pushSprite(0, 0);
}

void showBattery() {
    float batVoltage = M5.Axp.GetBatVoltage();
    float batPercentage = ( batVoltage < 3.2 ) ? 0 : ( batVoltage - 3.2 ) * 100;
    char Perc[10];
    sprintf(Perc, "%f", batPercentage);
    log(String("Battery: ") + Perc);
}

void scanloop(String trash) {
    showBattery();
    log("polling...");
    M5.Axp.SetLDOEnable(3, true);
    delay(150);
    M5.Axp.SetLDOEnable(3, false);
    while (true) {
        delay(5);
        M5.update();
        if (M5.BtnB.wasPressed()) {
          log("polling stopped.");
          showBattery();
          M5.Axp.SetLDOEnable(3, true);
          delay(150);
          M5.Axp.SetLDOEnable(3, false);
          M5.update();
          break;
        }
        uint8_t result = uhf.pollingMultiple(20);
        Serial.printf("scan result: %d\r\n", result);
        if (result > 0) {
            for (uint8_t i = 0; i < result; i++) {
                M5.Axp.SetLDOEnable(3, true);
                //log("epc: " + uhf.cards[i].epc_str + "  rssi: " + uhf.cards[i].rssi_str);
                int8_t rrssi = 0;
                // FF is -1 c9 is -55
                rrssi = uhf.cards[i].rssi - 256;
                //log("epc: " + uhf.cards[i].epc_str + "  rssi: " + uhf.cards[i].rssi);
                log("epc: " + uhf.cards[i].epc_str + "  rssi: " + rrssi);
                delay(75);
                M5.Axp.SetLDOEnable(3, false);
            }
        }
    }
}

void loop() {
    if (M5.BtnA.wasPressed()) {
        log("Write Data...");
        // uhf.writeCard(uint8_t *data, size_t size, uint8_t membank, uint16_t
        // sa, uint32_t access_password = 0);
        if (uhf.writeCard(write_buffer, sizeof(write_buffer), 0x04, 0,
                          0x00000000)) {
            log("Write OK");
        } else {
            log("Write ERROR");
        }
        delay(1000);
        log("Read Data...");
        // uhf.readCard(uint8_t *data, size_t size, uint8_t membank, uint16_t
        // sa, uint32_t access_password = 0);
        if (uhf.readCard(reade_buffer, sizeof(reade_buffer), 0x04, 0,
                         0x00000000)) {
            log("Read OK");
            log("Data Content");
            for (uint8_t i = 0; i < sizeof(reade_buffer); i++) {
                Serial.printf("%x", reade_buffer[i]);
                canvas.printf("%x", reade_buffer[i]);
            }
            canvas.pushSprite(0, 0);
        } else {
            log("Read ERROR");
        }
    }
    if (M5.BtnB.wasPressed()) {
        M5.update();
        scanloop("trash");
        //log("polling...");
        //uint8_t result = uhf.pollingOnce();
        //uint8_t result = uhf.pollingMultiple(5000);
        //Serial.printf("scan result: %d\r\n", result);
        //if (result > 0) {
        //    for (uint8_t i = 0; i < result; i++) {
                //log("pc: " + uhf.cards[i].pc_str);
                //log("epc: " + uhf.cards[i].epc_str);
                //log("rssi: " + uhf.cards[i].rssi_str);
                //log("-----------------");
                //delay(10);
            //}
        //} else {
        //  log("nothing found");
        //}
    }
    if (M5.BtnC.wasPressed()) {
        if (uhf.select(uhf.cards[0].epc)) {
            log("Select OK");
        } else {
            log("Select ERROR");
        }
        log("Current Select EPC:");
        log(uhf.selectInfo());
    }
    delay(10);
    M5.update();
}
