#include <18f4550.h>
#device adc=10
#fuses nomclr,nowdt,intrc_io
#use delay(clock=8000000)
#use rs232(uart1, baud=9600)

// --- LCD Pin Definitions ---
#define LCD_ENABLE_PIN PIN_D2
#define LCD_RS_PIN     PIN_D0
#define LCD_RW_PIN     PIN_D1
#define LCD_DATA4      PIN_D4
#define LCD_DATA5      PIN_D5
#define LCD_DATA6      PIN_D6
#define LCD_DATA7      PIN_D7
#include <lcd.c>

// --- Sensor & Actuator Pins ---
#define trig      pin_B1
#define echo      pin_B0
#define relay_pin pin_B3

// --- DHT11 Library ---
#include "DHT11.c"

// --- WiFi Credentials ---
#define SSID     "NETWORK_NAME"
#define PASS     "PASSWORD"
#define HOST     "IP ADDRESS"

// --- Sensor Variables ---
float rhum0, rhumf0, rhum1, rhumf1, rhum2, rhumf2;
float rhum3, rhumf3, rhum4, rhumf4, rhum5, rhumf5;
float temperature, relative_hum, vol, distance, elapsed_time;
int valueread = 0, cont = 0, cont1 = 0, cont2 = 0, cont3 = 0;
double temp, relhum, contvol, moisture;

// --- Flags (set by interrupts, read by main loop) ---
int1 moisture_ready = 0;
int1 dht_ready = 0;
int1 volume_ready = 0;

// ============================================================
// Timer3 Interrupt: Read 6 soil moisture sensors
// ============================================================
#int_timer3
void read_moisture()
{
   cont++;
   set_adc_channel(0); valueread = read_adc(); rhum0 = valueread / 7; rhumf0 += rhum0; delay_ms(10);
   set_adc_channel(1); valueread = read_adc(); rhum1 = valueread / 7; rhumf1 += rhum1; delay_ms(10);
   set_adc_channel(2); valueread = read_adc(); rhum2 = valueread / 7; rhumf2 += rhum2; delay_ms(10);
   set_adc_channel(3); valueread = read_adc(); rhum3 = valueread / 7; rhumf3 += rhum3; delay_ms(10);
   set_adc_channel(4); valueread = read_adc(); rhum4 = valueread / 7; rhumf4 += rhum4; delay_ms(10);
   set_adc_channel(5); valueread = read_adc(); rhum5 = valueread / 7; rhumf5 += rhum5; delay_ms(10);

   if (cont == 857)
   {
      cont = 0;
      moisture = (rhumf0 + rhumf1 + rhumf2 + rhumf3 + rhumf4 + rhumf5) / 5148;
      rhumf0 = rhumf1 = rhumf2 = rhumf3 = rhumf4 = rhumf5 = 0;
      moisture_ready = 1;
   }
   clear_interrupt(int_timer3);
   set_timer3(45535);
}

// ============================================================
// Timer1 Interrupt: Read DHT11 temperature & humidity
// ============================================================
#int_timer1
void read_dht()
{
   DHT11_init();
   unsigned char state = 0;
   state = get_data();
   cont1++;

   switch(state)
   {
      case 1:
      case 2:
         break;
      case 3:
         break;
      default:
         temp = (float)values[2] + (float)values[3] / 1000.0;
         relhum = (float)values[0] + (float)values[1] / 1000.0;
         temperature = temp + temperature;
         relative_hum = relhum + relative_hum;
         cont2++;
         if (cont1 == 5950)
         {
            temperature = temperature / cont2;
            relative_hum = relative_hum / cont2;
            cont1 = 0;
            cont2 = 0;
            dht_ready = 1;
         }
         break;
   }
   clear_interrupt(int_timer1);
   set_timer1(45535);
}

// ============================================================
// Timer0 Interrupt: Read ultrasonic distance & volume
// ============================================================
#int_rtcc
void read_ultrasonic()
{
   cont3++;
   output_high(trig);
   delay_us(10);
   output_low(trig);

   while(!input(echo)) {}

   int16 t_start = get_timer2();
   while(input(echo)) {}
   int16 t_end = get_timer2();

   elapsed_time = (float)(t_end - t_start);
   distance = (elapsed_time / 2.0) / 29.15;
   vol = (110.0 - distance) * 5.5418;
   contvol = contvol + vol;

   if (cont3 == 6000)
   {
      contvol = contvol / 6000;
      cont3 = 0;
      volume_ready = 1;
   }
}

// ============================================================
// ESP8266 Helper: Send AT command and wait
// ============================================================
void esp_send(char* cmd, int16 wait_ms)
{
   printf("%s\r\n", cmd);
   delay_ms(wait_ms);
}

// ============================================================
// ESP8266: Connect to WiFi
// ============================================================
void wifi_connect()
{
   esp_send("AT", 1000);
   esp_send("AT+CWMODE=1", 1000);
   printf("AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, PASS);
   delay_ms(5000);
}

// ============================================================
// ESP8266: Send sensor data via HTTP GET
// ============================================================
void wifi_send_data()
{
   char request[160];

   printf("AT+CIPSTART=\"TCP\",\"%s\",80\r\n", HOST);
   delay_ms(2000);

   sprintf(request, "GET /data/second.php?temperature=%f&humidity=%f&moisture=%f&volume=%f HTTP/1.0\r\nHost: %s\r\n\r\n",
           temperature, relative_hum, moisture, contvol, HOST);

   printf("AT+CIPSEND=%u\r\n", strlen(request));
   delay_ms(500);
   printf("%s", request);
   delay_ms(3000);

   esp_send("AT+CIPCLOSE", 1000);
}

// ============================================================
// Main
// ============================================================
void main()
{
   // Timer setup
   setup_timer_3(T3_INTERNAL | T3_DIV_BY_1);
   setup_timer_1(T1_INTERNAL | T1_DIV_BY_1);
   setup_timer_0(RTCC_INTERNAL | RTCC_DIV_128);
   setup_timer_2(T2_INTERNAL | T2_DIV_BY_1);
   set_timer3(45535);
   set_timer1(45535);
   set_timer0(100);

   // Interrupts
   enable_interrupts(GLOBAL);
   enable_interrupts(int_timer3);
   enable_interrupts(int_timer1);
   enable_interrupts(int_timer0);
   clear_interrupt(int_timer3);
   clear_interrupt(int_timer1);
   clear_interrupt(int_timer0);

   // ADC
   setup_adc(ADC_CLOCK_INTERNAL);
   setup_adc_ports(all_analog);

   // Relay pin
   output_low(relay_pin);

   // LCD
   lcd_init();
   printf(lcd_putc, "\fOrchard System\nStarting...");
   delay_ms(2000);

   // WiFi
   wifi_connect();
   printf(lcd_putc, "\fWiFi Connected");
   delay_ms(1000);

   while(true)
   {
      // Irrigation control
      if (moisture_ready)
      {
         if (moisture <= 70)
         {
            output_high(relay_pin);
         }
         if (moisture >= 77)
         {
            output_low(relay_pin);
         }
         moisture_ready = 0;
      }

      // Update LCD when DHT data is ready
      if (dht_ready)
      {
         lcd_gotoxy(1, 1);
         printf(lcd_putc, "\fT:%3.1fC H:%3.1f%%", temperature, relative_hum);
         lcd_gotoxy(1, 2);
         printf(lcd_putc, "M:%3.1f%% V:%3.1fL", moisture, contvol);
         dht_ready = 0;
      }

      // Send data to server when volume is ready
      if (volume_ready)
      {
         wifi_send_data();
         volume_ready = 0;
      }
   }
}
