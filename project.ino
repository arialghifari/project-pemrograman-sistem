#include <LiquidCrystal.h>

// Inisialisasi LCD
// LiquidCrystal lcd(rs, e, d4, d5, d6, d7)
// rs = register select, pin untuk membedakan antara perintah dan data
// e = enable, pin untuk menghidupkan lcd
// d4-d7 = data pins, pin untuk menuliskan karakter ke lcd
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

const int buzz_pin = 12, lampu_pin = 13, echo_pin = 11, trig_pin = 10;
int orang = 0, maks = 5, batas_in_out = 50, status_in = 0, status_out = 0;
float display_jarak = 0;

void setup()
{
  delay(10000); // delay untuk mempersiapkan lcd
  pinMode(lampu_pin, OUTPUT);
  pinMode(trig_pin, OUTPUT);
  pinMode(echo_pin, INPUT);
  pinMode(buzz_pin, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);

  // Set LCD Awal
  lcd.setCursor(0, 0);
  lcd.print("Orang:");
  lcd.setCursor(9, 0);
  lcd.print("Max:");
}

void loop()
{
  float durasi, jarak_cm;

  // Memancarkan sinyal untuk mendeteksi jarak ke objek.
  // Untuk mendapatkan hasil sinyal yang baik, maka harus
  // dilakukan secara bertahap, mulai dari LOW dan bertahap
  // menuju ke HIGH untuk memastikan pengiriman dan penerimaan
  // sinyal yang baik.
  digitalWrite(trig_pin, LOW);
  delayMicroseconds(2);
  digitalWrite(trig_pin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trig_pin, LOW);

  // Durasi atau waktu delay didapatkan dari transmitter
  // yang mengirimkan sinyal ultrasonik, kemudian ketika
  // sinyal tersebut mengenai suatu objek maka akan
  // dipantulkan kembali dan diterima oleh receiver. Durasi
  // disini dalam mikrodetik.
  durasi = pulseIn(echo_pin, HIGH);

  // Kalkulasi kecepatan suara ke cm.
  // Jarak didapatkan dari durasi yang dibagi 2 karena 
  // hanya diperlukan untuk menghitung jarak dari transmitter
  // ke objek yang ada. Kecepatan suara adalah
  // 343m/s maka ketika dikonversi ke mikrodetik menjadi
  // 0,0343cm/mikrodetik kemudian kita kalikan dengan durasi
  // tadi dan akan mendapatkan jarak per centimeter.
  jarak_cm = (durasi / 2) * 0.0343;

  // Kendali jarak masuk dan jarak keluar
  if (jarak_cm > 0 && jarak_cm <= batas_in_out) {
    delay(700);
    status_in = 1;
  } else if (jarak_cm > batas_in_out && jarak_cm <= 100) {
    delay(1400);
    status_out = 1;
  } else {
    status_in = 0;
    status_out = 0;
  }

  // Kendali penambahan dan pengurangan orang ketika
  // masuk dan keluar
  if (status_in != status_out) {
    if (status_in == 1) {
      orang++;
      
      display_jarak = jarak_cm;
      
      digitalWrite(buzz_pin, HIGH);
      delay(50);
      digitalWrite(buzz_pin, LOW);
    } else if (status_out == 1) {
      orang--;
      
      display_jarak = jarak_cm;
      
      digitalWrite(buzz_pin, HIGH);
      delay(50);
      digitalWrite(buzz_pin, LOW);
      delay(50);
      digitalWrite(buzz_pin, HIGH);
      delay(50);
      digitalWrite(buzz_pin, LOW);
    }
    status_in = 0;
    status_out = 0;
    
    Serial.println("=======================");
    Serial.print("Jarak: ");
    Serial.print(jarak_cm);
    Serial.println("cm");
    Serial.print("Kapasitas Maksimal: ");
    Serial.println(maks);
    Serial.print("Jumlah Orang: ");
    Serial.println(orang);
  }

  // Kendali lampu berdasarkan jumlah orang yang ada.
  // Jika ada orang maka lampu akan otomatis menyala,
  // sedangkan jika tidak ada maka lampu akan mati.
  if (orang > 0) {
    digitalWrite(lampu_pin, HIGH);
  } else {
    digitalWrite(lampu_pin, LOW);
    orang = 0;
  }

  // Kendali buzzer yang akan mengeluarkan bunyi ketika jumlah
  // orang melebihi kapasitas yang telah ditentukan
  if(orang > maks) {
    Serial.println("========================================");
    Serial.println("HARAP KELUAR, SUDAH MELEBIHI KAPASITAS!!");
    Serial.print("Kapasitas Maksimal: ");
    Serial.println(maks);
    Serial.print("Jumlah Orang: ");
    Serial.println(orang);

    digitalWrite(buzz_pin, HIGH);
    lcd.setCursor(0, 1);
    lcd.print("OVER KAPASITAS!!");
    lcd.display();
    
    delay(150);
    
    digitalWrite(buzz_pin, LOW);
    lcd.noDisplay();
  } else {
    lcd.display();
    noTone(buzz_pin);
  }

  // Kendali lcd  
  lcd.setCursor(6, 0);
  lcd.print(orang);
  
  lcd.setCursor(13, 0);
  lcd.print(maks);

  lcd.setCursor(0, 1);
  lcd.print("Jarak(cm):");
  lcd.setCursor(10, 1);
  lcd.print(display_jarak);
  lcd.setCursor(15, 1);
  lcd.print(" ");
  
  delay(150);
}
