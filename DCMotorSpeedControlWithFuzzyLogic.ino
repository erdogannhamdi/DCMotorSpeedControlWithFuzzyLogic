#include <FuzzyRule.h>
#include <FuzzyComposition.h>
#include <Fuzzy.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzyOutput.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzySet.h>
#include <FuzzyRuleAntecedent.h>
#include <QTRSensors.h>
#include <Wire.h>
#include <I2Cdev.h>
#include <MPU6050.h>
#define NUM_SENSORS   3     // number of sensors used
#define TIMEOUT       2500  // waits for 2500 microseconds for sensor outputs to go low
#define EMITTER_PIN   2     // emitter is controlled by digital pin 2
QTRSensorsRC qtrrc((unsigned char[]) {
  A13, A14, A15
},
NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues[NUM_SENSORS];
QTRSensorsRC qtrrc2((unsigned char[]) {
  A10, A11, A12
},
NUM_SENSORS, TIMEOUT, EMITTER_PIN);
unsigned int sensorValues2[NUM_SENSORS];
const int in_1  = 10;
const int in_2  = 11;
const int pwm1 = 4;
const int in_3  = 8;
const int in_4  = 9;
const int pwm2 = 5;
const int led = 53;
MPU6050 ACC_GYR;
int16_t accx, accy, accz;
int16_t gyrx, gyry, gyrz;
int degerz, eskidegerz=0 , degerx , eskidegerx=0;
float hiz;

Fuzzy* fuzzy = new Fuzzy();

//Eğim üyelik fonksiyonları

FuzzySet* CokAzEgim = new FuzzySet(0, 0, 0, 10);
FuzzySet* AzEgim = new FuzzySet(0, 10, 10, 20);
FuzzySet* OrtaEgim = new FuzzySet(10, 20, 20, 30);
FuzzySet* FazlaEgim = new FuzzySet(20, 30, 30, 40);
FuzzySet* CokFazlaEgim = new FuzzySet(30, 40, 40, 40);
FuzzySet* TersCokAzEgim = new FuzzySet(90, 90, 90, 100);
FuzzySet* TersAzEgim = new FuzzySet(90, 100, 100, 110);
FuzzySet* TersOrtaEgim = new FuzzySet(100, 110, 110, 120);
FuzzySet* TersFazlaEgim = new FuzzySet(110, 120, 120, 130);
FuzzySet* TersCokFazlaEgim = new FuzzySet(120, 130, 130, 130);

//Viraj üyelik fonksiyonları
FuzzySet* CokHafifViraj = new FuzzySet(0, 0, 0, 8);
FuzzySet* HafifViraj = new FuzzySet(0, 8, 8, 16);
FuzzySet* NormalViraj = new FuzzySet(8, 16, 16, 24);
FuzzySet* KeskinViraj = new FuzzySet(16, 24, 24, 32);
FuzzySet* CokKeskinViraj = new FuzzySet(24, 32, 32, 32);

//Hız üyelik fonksiyoları
FuzzySet* CokYavas = new FuzzySet(0, 0, 0, 40);
FuzzySet* Yavas = new FuzzySet(0, 40, 40, 80);
FuzzySet* Orta = new FuzzySet(40, 60, 60, 100);
FuzzySet* Hizli = new FuzzySet(80, 120, 120, 140);
FuzzySet* CokHizli = new FuzzySet(120, 140, 140, 140);

void setup() {

  Serial.begin(9600);
  // put your setup code here, to run once:
  pinMode(in_1, OUTPUT);
  pinMode(in_2, OUTPUT);
  pinMode(pwm1, OUTPUT);
  pinMode(in_3, OUTPUT);
  pinMode(in_4, OUTPUT);
  pinMode(pwm2, OUTPUT);
  pinMode(led, OUTPUT);
  Wire.begin();
  ACC_GYR.initialize();
  Serial.println(ACC_GYR.testConnection() ? "BASARILI" : "BASARISIZ");
  //titreşim azaltma
  Wire.beginTransmission(0x68);
  Wire.write(0x1A);  // the config address
  Wire.write(0x06);  // the config value
  Wire.endTransmission(true);
  //1.input
  FuzzyInput* egim = new FuzzyInput(1);
  egim->addFuzzySet(CokAzEgim);
  egim->addFuzzySet(AzEgim);
  egim->addFuzzySet(OrtaEgim);
  egim->addFuzzySet(FazlaEgim);
  egim->addFuzzySet(CokFazlaEgim);
  egim->addFuzzySet(TersCokAzEgim);
  egim->addFuzzySet(TersAzEgim);
  egim->addFuzzySet(TersOrtaEgim);
  egim->addFuzzySet(TersFazlaEgim);
  egim->addFuzzySet(TersCokFazlaEgim);
  fuzzy->addFuzzyInput(egim);
  //2.input
  FuzzyInput* viraj = new FuzzyInput(2);
  viraj->addFuzzySet(CokHafifViraj);
  viraj->addFuzzySet(HafifViraj);
  viraj->addFuzzySet(NormalViraj);
  viraj->addFuzzySet(KeskinViraj);
  viraj->addFuzzySet(CokKeskinViraj);
  fuzzy->addFuzzyInput(viraj);
  //1.output
  FuzzyOutput* hiz = new FuzzyOutput(1);
  hiz->addFuzzySet(CokYavas);
  hiz->addFuzzySet(Yavas);
  hiz->addFuzzySet(Orta);
  hiz->addFuzzySet(Hizli);
  hiz->addFuzzySet(CokHizli);
  fuzzy->addFuzzyOutput(hiz);
  //Kurallar
  //İNİŞ
  //1
  FuzzyRuleAntecedent* egimCokAzVeVirajCokHafif = new FuzzyRuleAntecedent();
  egimCokAzVeVirajCokHafif->joinWithAND(CokAzEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenOrta0 = new FuzzyRuleConsequent();
  thenOrta0->addOutput(Orta);

  FuzzyRule* Kural1 = new FuzzyRule(1, egimCokAzVeVirajCokHafif, thenOrta0);
  fuzzy->addFuzzyRule(Kural1);

  //2
  FuzzyRuleAntecedent* egimCokAzVeVirajHafif = new FuzzyRuleAntecedent();
  egimCokAzVeVirajHafif->joinWithAND(CokAzEgim, HafifViraj);
  FuzzyRuleConsequent* thenYavas = new FuzzyRuleConsequent();
  thenYavas->addOutput(Yavas);

  FuzzyRule* Kural2 = new FuzzyRule(2, egimCokAzVeVirajHafif, thenYavas);
  fuzzy->addFuzzyRule(Kural2);

  //3
  FuzzyRuleAntecedent* egimCokAzVeVirajNormal = new FuzzyRuleAntecedent();
  egimCokAzVeVirajNormal->joinWithAND(CokAzEgim, NormalViraj);
  FuzzyRuleConsequent* thenYavas0 = new FuzzyRuleConsequent();
  thenYavas0->addOutput(Yavas);

  FuzzyRule* Kural3 = new FuzzyRule(3, egimCokAzVeVirajNormal, thenYavas0);
  fuzzy->addFuzzyRule(Kural3);

  //4
  FuzzyRuleAntecedent* egimCokAzVeVirajKeskin = new FuzzyRuleAntecedent();
  egimCokAzVeVirajKeskin->joinWithAND(CokAzEgim, KeskinViraj);
  FuzzyRuleConsequent* thenCokYavas = new FuzzyRuleConsequent();
  thenCokYavas->addOutput(CokYavas);

  FuzzyRule* Kural4 = new FuzzyRule(4, egimCokAzVeVirajKeskin, thenCokYavas);
  fuzzy->addFuzzyRule(Kural4);

  //5
  FuzzyRuleAntecedent* egimCokAzVeVirajCokKeskin = new FuzzyRuleAntecedent();
  egimCokAzVeVirajCokKeskin->joinWithAND(CokAzEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas0 = new FuzzyRuleConsequent();
  thenCokYavas0->addOutput(CokYavas);

  FuzzyRule* Kural5 = new FuzzyRule(5, egimCokAzVeVirajCokKeskin, thenCokYavas0);
  fuzzy->addFuzzyRule(Kural5);

  //6
  FuzzyRuleAntecedent* egimAzVeVirajCokHafif = new FuzzyRuleAntecedent();
  egimAzVeVirajCokHafif->joinWithAND(AzEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenYavas50 = new FuzzyRuleConsequent();
  thenYavas50->addOutput(Yavas);

  FuzzyRule* Kural11 = new FuzzyRule(11, egimAzVeVirajCokHafif, thenYavas50);
  fuzzy->addFuzzyRule(Kural11);

  //7
  FuzzyRuleAntecedent* egimAzVeVirajHafif = new FuzzyRuleAntecedent();
  egimAzVeVirajHafif->joinWithAND(AzEgim, HafifViraj);
  FuzzyRuleConsequent* thenYavas2 = new FuzzyRuleConsequent();
  thenYavas2->addOutput(Yavas);

  FuzzyRule* Kural12 = new FuzzyRule(12, egimAzVeVirajHafif, thenYavas2);
  fuzzy->addFuzzyRule(Kural12);

  //8
  FuzzyRuleAntecedent* egimAzVeVirajNormal = new FuzzyRuleAntecedent();
  egimAzVeVirajNormal->joinWithAND(AzEgim, NormalViraj);
  FuzzyRuleConsequent* thenCokYavas50 = new FuzzyRuleConsequent();
  thenCokYavas50->addOutput(CokYavas);

  FuzzyRule* Kural13 = new FuzzyRule(13, egimAzVeVirajNormal, thenCokYavas50);
  fuzzy->addFuzzyRule(Kural13);

  //9
  FuzzyRuleAntecedent* egimAzVeVirajKeskin = new FuzzyRuleAntecedent();
  egimAzVeVirajKeskin->joinWithAND(AzEgim, KeskinViraj);
  FuzzyRuleConsequent* thenCokYavas51= new FuzzyRuleConsequent();
  thenCokYavas51->addOutput(CokYavas);

  FuzzyRule* Kural14 = new FuzzyRule(14, egimAzVeVirajKeskin, thenCokYavas51);
  fuzzy->addFuzzyRule(Kural14);

  //10
  FuzzyRuleAntecedent* egimAzVeVirajCokKeskin = new FuzzyRuleAntecedent();
  egimAzVeVirajCokKeskin->joinWithAND(AzEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas1 = new FuzzyRuleConsequent();
  thenCokYavas1->addOutput(CokYavas);

  FuzzyRule* Kural15 = new FuzzyRule(15, egimAzVeVirajCokKeskin, thenCokYavas1);
  fuzzy->addFuzzyRule(Kural15);

  //11
  FuzzyRuleAntecedent* egimOrtaVeVirajCokHafif = new FuzzyRuleAntecedent();
  egimOrtaVeVirajCokHafif->joinWithAND(OrtaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenCokYavas52 = new FuzzyRuleConsequent();
  thenCokYavas52->addOutput(CokYavas);

  FuzzyRule* Kural21 = new FuzzyRule(21, egimOrtaVeVirajCokHafif, thenCokYavas52);
  fuzzy->addFuzzyRule(Kural21);

  //12
  FuzzyRuleAntecedent* egimOrtaVeVirajHafif = new FuzzyRuleAntecedent();
  egimOrtaVeVirajHafif->joinWithAND(OrtaEgim, HafifViraj);
  FuzzyRuleConsequent* thenCokYavas53 = new FuzzyRuleConsequent();
  thenCokYavas53->addOutput(CokYavas);

  FuzzyRule* Kural22 = new FuzzyRule(22, egimOrtaVeVirajHafif, thenCokYavas53);
  fuzzy->addFuzzyRule(Kural22);

  //13
  FuzzyRuleAntecedent* egimOrtaVeVirajNormal = new FuzzyRuleAntecedent();
  egimOrtaVeVirajNormal->joinWithAND(OrtaEgim, NormalViraj);
  FuzzyRuleConsequent* thenCokYavas54 = new FuzzyRuleConsequent();
  thenCokYavas54->addOutput(CokYavas);

  FuzzyRule* Kural23 = new FuzzyRule(23, egimOrtaVeVirajNormal, thenCokYavas54);
  fuzzy->addFuzzyRule(Kural23);

  //14
  FuzzyRuleAntecedent* egimOrtaVeVirajKeskin = new FuzzyRuleAntecedent();
  egimOrtaVeVirajKeskin->joinWithAND(OrtaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenCokYavas2 = new FuzzyRuleConsequent();
  thenCokYavas2->addOutput(CokYavas);

  FuzzyRule* Kural24 = new FuzzyRule(24, egimOrtaVeVirajKeskin, thenCokYavas2);
  fuzzy->addFuzzyRule(Kural24);

  //15
  FuzzyRuleAntecedent* egimOrtaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  egimOrtaVeVirajCokKeskin->joinWithAND(OrtaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas3 = new FuzzyRuleConsequent();
  thenCokYavas3->addOutput(CokYavas);

  FuzzyRule* Kural25 = new FuzzyRule(25, egimOrtaVeVirajCokKeskin, thenCokYavas3);
  fuzzy->addFuzzyRule(Kural25);

  //16
  FuzzyRuleAntecedent* egimFazlaVeVirajCokHafif = new FuzzyRuleAntecedent();
  egimFazlaVeVirajCokHafif->joinWithAND(FazlaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenCokYavas55 = new FuzzyRuleConsequent();
  thenCokYavas55->addOutput(CokYavas);

  FuzzyRule* Kural31 = new FuzzyRule(31, egimFazlaVeVirajCokHafif, thenCokYavas55);
  fuzzy->addFuzzyRule(Kural31);

  //17
  FuzzyRuleAntecedent* egimFazlaVeVirajHafif = new FuzzyRuleAntecedent();
  egimFazlaVeVirajHafif->joinWithAND(FazlaEgim, HafifViraj);
  FuzzyRuleConsequent* thenCokYavas56 = new FuzzyRuleConsequent();
  thenCokYavas56->addOutput(CokYavas);

  FuzzyRule* Kural32 = new FuzzyRule(32, egimFazlaVeVirajHafif, thenCokYavas56);
  fuzzy->addFuzzyRule(Kural32);

  //18
  FuzzyRuleAntecedent* egimFazlaVeVirajNormal = new FuzzyRuleAntecedent();
  egimFazlaVeVirajNormal->joinWithAND(FazlaEgim, NormalViraj);
  FuzzyRuleConsequent* thenCokYavas4 = new FuzzyRuleConsequent();
  thenCokYavas4->addOutput(CokYavas);

  FuzzyRule* Kural33 = new FuzzyRule(33, egimFazlaVeVirajNormal, thenCokYavas4);
  fuzzy->addFuzzyRule(Kural33);

  //19
  FuzzyRuleAntecedent* egimFazlaVeVirajKeskin = new FuzzyRuleAntecedent();
  egimFazlaVeVirajKeskin->joinWithAND(FazlaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenCokYavas5 = new FuzzyRuleConsequent();
  thenCokYavas5->addOutput(CokYavas);

  FuzzyRule* Kural34 = new FuzzyRule(34, egimFazlaVeVirajKeskin, thenCokYavas5);
  fuzzy->addFuzzyRule(Kural34);

  //20
  FuzzyRuleAntecedent* egimFazlaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  egimFazlaVeVirajCokKeskin->joinWithAND(FazlaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas6 = new FuzzyRuleConsequent();
  thenCokYavas6->addOutput(CokYavas);

  FuzzyRule* Kural35 = new FuzzyRule(35, egimFazlaVeVirajCokKeskin, thenCokYavas6);
  fuzzy->addFuzzyRule(Kural35);

  //21
  FuzzyRuleAntecedent* egimCokFazlaVeVirajCokHafif = new FuzzyRuleAntecedent();
  egimCokFazlaVeVirajCokHafif->joinWithAND(CokFazlaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenCokYavas7 = new FuzzyRuleConsequent();
  thenCokYavas7->addOutput(CokYavas);

  FuzzyRule* Kural41 = new FuzzyRule(41, egimCokFazlaVeVirajCokHafif, thenCokYavas7);
  fuzzy->addFuzzyRule(Kural41);

  //22
  FuzzyRuleAntecedent* egimCokFazlaVeVirajHafif = new FuzzyRuleAntecedent();
  egimCokFazlaVeVirajHafif->joinWithAND(CokFazlaEgim, HafifViraj);
  FuzzyRuleConsequent* thenCokYavas8 = new FuzzyRuleConsequent();
  thenCokYavas8->addOutput(CokYavas);

  FuzzyRule* Kural42 = new FuzzyRule(42, egimCokFazlaVeVirajHafif, thenCokYavas8);
  fuzzy->addFuzzyRule(Kural42);

  //23
  FuzzyRuleAntecedent* egimCokFazlaVeVirajNormal = new FuzzyRuleAntecedent();
  egimCokFazlaVeVirajNormal->joinWithAND(CokFazlaEgim, NormalViraj);
  FuzzyRuleConsequent* thenCokYavas9 = new FuzzyRuleConsequent();
  thenCokYavas9->addOutput(CokYavas);

  FuzzyRule* Kural43 = new FuzzyRule(43, egimCokFazlaVeVirajNormal, thenCokYavas9);
  fuzzy->addFuzzyRule(Kural43);

  //24
  FuzzyRuleAntecedent* egimCokFazlaVeVirajKeskin = new FuzzyRuleAntecedent();
  egimCokFazlaVeVirajKeskin->joinWithAND(CokFazlaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenCokYavas10 = new FuzzyRuleConsequent();
  thenCokYavas10->addOutput(CokYavas);

  FuzzyRule* Kural44 = new FuzzyRule(44, egimCokFazlaVeVirajKeskin, thenCokYavas10);
  fuzzy->addFuzzyRule(Kural44);

  //25
  FuzzyRuleAntecedent* egimCokFazlaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  egimCokFazlaVeVirajCokKeskin->joinWithAND(CokFazlaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas11 = new FuzzyRuleConsequent();
  thenCokYavas11->addOutput(CokYavas);

  FuzzyRule* Kural45 = new FuzzyRule(45, egimCokFazlaVeVirajCokKeskin, thenCokYavas11);
  fuzzy->addFuzzyRule(Kural45);
  
  //TERSSS
  //ÇIKIŞ
  
  //26
  FuzzyRuleAntecedent* TersegimCokAzVeVirajCokHafif = new FuzzyRuleAntecedent();
  TersegimCokAzVeVirajCokHafif->joinWithAND(TersCokAzEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenOrta3 = new FuzzyRuleConsequent();
  thenOrta3->addOutput(Orta); 
  
  FuzzyRule* Kural6 = new FuzzyRule(6, TersegimCokAzVeVirajCokHafif, thenOrta3);
  fuzzy->addFuzzyRule(Kural6);

  //27
  FuzzyRuleAntecedent* TersegimCokAzVeVirajHafif = new FuzzyRuleAntecedent();
  TersegimCokAzVeVirajHafif->joinWithAND(TersCokAzEgim, HafifViraj);
  FuzzyRuleConsequent* thenOrta4 = new FuzzyRuleConsequent();
  thenOrta4->addOutput(Orta);
  
  FuzzyRule* Kural7 = new FuzzyRule(7, TersegimCokAzVeVirajHafif, thenOrta4);
  fuzzy->addFuzzyRule(Kural7);

  //28
  FuzzyRuleAntecedent* TersegimCokAzVeVirajNormal= new FuzzyRuleAntecedent();
  TersegimCokAzVeVirajNormal->joinWithAND(TersCokAzEgim, NormalViraj);
  FuzzyRuleConsequent* thenYavas10 = new FuzzyRuleConsequent();
  thenYavas10->addOutput(Yavas);
  
  FuzzyRule* Kural8 = new FuzzyRule(8, TersegimCokAzVeVirajNormal, thenYavas10);
  fuzzy->addFuzzyRule(Kural8);

  //29
  FuzzyRuleAntecedent* TersegimCokAzVeVirajKeskin= new FuzzyRuleAntecedent();
  TersegimCokAzVeVirajKeskin->joinWithAND(TersCokAzEgim, KeskinViraj);
  FuzzyRuleConsequent* thenYavas11 = new FuzzyRuleConsequent();
  thenYavas11->addOutput(Yavas);
  
  FuzzyRule* Kural9 = new FuzzyRule(9, TersegimCokAzVeVirajKeskin, thenYavas11);
  fuzzy->addFuzzyRule(Kural9);

  //30
  FuzzyRuleAntecedent* TersegimCokAzVeVirajCokKeskin= new FuzzyRuleAntecedent();
  TersegimCokAzVeVirajCokKeskin->joinWithAND(TersCokAzEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas12 = new FuzzyRuleConsequent();
  thenCokYavas12->addOutput(CokYavas);
  
  FuzzyRule* Kural10 = new FuzzyRule(10, TersegimCokAzVeVirajCokKeskin, thenCokYavas12);
  fuzzy->addFuzzyRule(Kural10);

  //31
  FuzzyRuleAntecedent* TersegimAzVeVirajCokHafif = new FuzzyRuleAntecedent();
  TersegimAzVeVirajCokHafif->joinWithAND(TersAzEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenHizli0 = new FuzzyRuleConsequent();
  thenHizli0->addOutput(Orta);

  FuzzyRule* Kural16 = new FuzzyRule(16, TersegimAzVeVirajCokHafif, thenHizli0);
  fuzzy->addFuzzyRule(Kural16);

  //32
  FuzzyRuleAntecedent* TersegimAzVeVirajHafif = new FuzzyRuleAntecedent();
  TersegimAzVeVirajHafif->joinWithAND(TersAzEgim, HafifViraj);
  FuzzyRuleConsequent* thenOrta5 = new FuzzyRuleConsequent();
  thenOrta5->addOutput(Orta);

  FuzzyRule* Kural17 = new FuzzyRule(17, TersegimAzVeVirajHafif, thenOrta5);
  fuzzy->addFuzzyRule(Kural17);

  //33
  FuzzyRuleAntecedent* TersegimAzVeVirajNormal = new FuzzyRuleAntecedent();
  TersegimAzVeVirajNormal->joinWithAND(TersAzEgim, NormalViraj);
  FuzzyRuleConsequent* thenYavas12 = new FuzzyRuleConsequent();
  thenYavas12->addOutput(Yavas);

  FuzzyRule* Kural18 = new FuzzyRule(18, TersegimAzVeVirajNormal, thenYavas12);
  fuzzy->addFuzzyRule(Kural18);

  //34
  FuzzyRuleAntecedent* TersegimAzVeVirajKeskin = new FuzzyRuleAntecedent();
  TersegimAzVeVirajKeskin->joinWithAND(TersAzEgim, KeskinViraj);
  FuzzyRuleConsequent* thenYavas13 = new FuzzyRuleConsequent();
  thenYavas13->addOutput(Yavas);

  FuzzyRule* Kural19 = new FuzzyRule(19, TersegimAzVeVirajKeskin, thenYavas13);
  fuzzy->addFuzzyRule(Kural19);

  //35
  FuzzyRuleAntecedent* TersegimAzVeVirajCokKeskin = new FuzzyRuleAntecedent();
  TersegimAzVeVirajCokKeskin->joinWithAND(TersAzEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas13 = new FuzzyRuleConsequent();
  thenCokYavas13->addOutput(CokYavas);

  FuzzyRule* Kural20 = new FuzzyRule(20, TersegimAzVeVirajCokKeskin, thenCokYavas13);
  fuzzy->addFuzzyRule(Kural20);

  //36
  FuzzyRuleAntecedent* TersegimOrtaVeVirajCokHafif = new FuzzyRuleAntecedent();
  TersegimOrtaVeVirajCokHafif->joinWithAND(TersOrtaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenHizli1 = new FuzzyRuleConsequent();
  thenHizli1->addOutput(Hizli);

  FuzzyRule* Kural26 = new FuzzyRule(26, TersegimOrtaVeVirajCokHafif, thenHizli1);
  fuzzy->addFuzzyRule(Kural26);

  //37
  FuzzyRuleAntecedent* TersegimOrtaVeVirajHafif = new FuzzyRuleAntecedent();
  TersegimOrtaVeVirajHafif->joinWithAND(TersOrtaEgim, HafifViraj);
  FuzzyRuleConsequent* thenHizli2 = new FuzzyRuleConsequent();
  thenHizli2->addOutput(Hizli);

  FuzzyRule* Kural27 = new FuzzyRule(27, TersegimOrtaVeVirajHafif, thenHizli2);
  fuzzy->addFuzzyRule(Kural27);

  //38
  FuzzyRuleAntecedent* TersegimOrtaVeVirajNormal = new FuzzyRuleAntecedent();
  TersegimOrtaVeVirajNormal->joinWithAND(TersOrtaEgim, NormalViraj);
  FuzzyRuleConsequent* thenYavas14 = new FuzzyRuleConsequent();
  thenYavas14->addOutput(Yavas);

  FuzzyRule* Kural28 = new FuzzyRule(28, TersegimOrtaVeVirajNormal, thenYavas14);
  fuzzy->addFuzzyRule(Kural28);

  //39
  FuzzyRuleAntecedent* TersegimOrtaVeVirajKeskin = new FuzzyRuleAntecedent();
  TersegimOrtaVeVirajKeskin->joinWithAND(TersOrtaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenYavas15 = new FuzzyRuleConsequent();
  thenYavas15->addOutput(Yavas);

  FuzzyRule* Kural29 = new FuzzyRule(29, TersegimOrtaVeVirajKeskin, thenYavas15);
  fuzzy->addFuzzyRule(Kural29);

  //40
  FuzzyRuleAntecedent* TersegimOrtaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  TersegimOrtaVeVirajCokKeskin->joinWithAND(TersOrtaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas14 = new FuzzyRuleConsequent();
  thenCokYavas14->addOutput(CokYavas);

  FuzzyRule* Kural30 = new FuzzyRule(30, TersegimOrtaVeVirajCokKeskin, thenCokYavas14);
  fuzzy->addFuzzyRule(Kural30);


  //41
  FuzzyRuleAntecedent* TersegimFazlaVeVirajCokHafif = new FuzzyRuleAntecedent();
  TersegimFazlaVeVirajCokHafif->joinWithAND(TersFazlaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenCokHizli0 = new FuzzyRuleConsequent();
  thenCokHizli0->addOutput(CokHizli);

  FuzzyRule* Kural36 = new FuzzyRule(36, TersegimFazlaVeVirajCokHafif, thenCokHizli0);
  fuzzy->addFuzzyRule(Kural36);

  //42
  FuzzyRuleAntecedent* TersegimFazlaVeVirajHafif = new FuzzyRuleAntecedent();
  TersegimFazlaVeVirajHafif->joinWithAND(TersFazlaEgim, HafifViraj);
  FuzzyRuleConsequent* thenCokHizli1 = new FuzzyRuleConsequent();
  thenCokHizli1->addOutput(CokHizli);

  FuzzyRule* Kural37 = new FuzzyRule(37, TersegimFazlaVeVirajHafif, thenCokHizli1);
  fuzzy->addFuzzyRule(Kural37);

  //43
  FuzzyRuleAntecedent* TersegimFazlaVeVirajNormal = new FuzzyRuleAntecedent();
  TersegimFazlaVeVirajNormal->joinWithAND(TersFazlaEgim, NormalViraj);
  FuzzyRuleConsequent* thenOrta6 = new FuzzyRuleConsequent();
  thenOrta6->addOutput(Orta);

  FuzzyRule* Kural38 = new FuzzyRule(38, TersegimFazlaVeVirajNormal, thenOrta6);
  fuzzy->addFuzzyRule(Kural38);

  //44
  FuzzyRuleAntecedent* TersegimFazlaVeVirajKeskin = new FuzzyRuleAntecedent();
  TersegimFazlaVeVirajKeskin->joinWithAND(TersFazlaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenYavas16 = new FuzzyRuleConsequent();
  thenYavas16->addOutput(Yavas);

  FuzzyRule* Kural39 = new FuzzyRule(39, TersegimFazlaVeVirajKeskin, thenYavas16);
  fuzzy->addFuzzyRule(Kural39);

  //45
  FuzzyRuleAntecedent* TersegimFazlaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  TersegimFazlaVeVirajCokKeskin->joinWithAND(TersFazlaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas15 = new FuzzyRuleConsequent();
  thenCokYavas15->addOutput(CokYavas);

  FuzzyRule* Kural40 = new FuzzyRule(40, TersegimFazlaVeVirajCokKeskin, thenCokYavas15);
  fuzzy->addFuzzyRule(Kural40);

  //46
  FuzzyRuleAntecedent* TersegimCokFazlaVeVirajCokHafif = new FuzzyRuleAntecedent();
  TersegimCokFazlaVeVirajCokHafif->joinWithAND(TersCokFazlaEgim, CokHafifViraj);
  FuzzyRuleConsequent* thenCokHizli2 = new FuzzyRuleConsequent();
  thenCokHizli2->addOutput(CokHizli);

  FuzzyRule* Kural46 = new FuzzyRule(46, TersegimCokFazlaVeVirajCokHafif, thenCokHizli2);
  fuzzy->addFuzzyRule(Kural46);

  //47
  FuzzyRuleAntecedent* TersegimCokFazlaVeVirajHafif = new FuzzyRuleAntecedent();
  TersegimCokFazlaVeVirajHafif->joinWithAND(TersCokFazlaEgim, HafifViraj);
  FuzzyRuleConsequent* thenCokHizli3 = new FuzzyRuleConsequent();
  thenCokHizli3->addOutput(CokHizli);

  FuzzyRule* Kural47 = new FuzzyRule(47, TersegimCokFazlaVeVirajHafif, thenCokHizli3);
  fuzzy->addFuzzyRule(Kural47);

  //48
  FuzzyRuleAntecedent* TersegimCokFazlaVeVirajNormal = new FuzzyRuleAntecedent();
  TersegimCokFazlaVeVirajNormal->joinWithAND(TersCokFazlaEgim, NormalViraj);
  FuzzyRuleConsequent* thenOrta7 = new FuzzyRuleConsequent();
  thenOrta7->addOutput(Orta);

  FuzzyRule* Kural48 = new FuzzyRule(48, TersegimCokFazlaVeVirajNormal, thenOrta7);
  fuzzy->addFuzzyRule(Kural48);

  //49
  FuzzyRuleAntecedent* TersegimCokFazlaVeVirajKeskin = new FuzzyRuleAntecedent();
  TersegimCokFazlaVeVirajKeskin->joinWithAND(TersCokFazlaEgim, KeskinViraj);
  FuzzyRuleConsequent* thenYavas17 = new FuzzyRuleConsequent();
  thenYavas17->addOutput(Yavas);

  FuzzyRule* Kural49 = new FuzzyRule(49, TersegimCokFazlaVeVirajKeskin, thenYavas17);
  fuzzy->addFuzzyRule(Kural49);

  //50
  FuzzyRuleAntecedent* TersegimCokFazlaVeVirajCokKeskin = new FuzzyRuleAntecedent();
  TersegimCokFazlaVeVirajCokKeskin->joinWithAND(TersCokFazlaEgim, CokKeskinViraj);
  FuzzyRuleConsequent* thenCokYavas16 = new FuzzyRuleConsequent();
  thenCokYavas16->addOutput(CokYavas);

  FuzzyRule* Kural50 = new FuzzyRule(50, TersegimCokFazlaVeVirajCokKeskin, thenCokYavas16);
  fuzzy->addFuzzyRule(Kural50);
  
}

void loop() {

  digitalWrite(led, HIGH);
  ACC_GYR.getMotion6(&accx, &accy, &accz, &gyrx, &gyry , &gyrz); //
  degerz = map(gyrz, -32768, 32767, 0, 180);
  degerx = map(accx, -17000, 17000, 0, 180);
  
  degerz -= 90;
  if (degerz < 0)
  {
    degerz *= -1;
  }
  if (degerx <= 90)
  {
    degerx = 90 - degerx;
  }  

  if( degerz != eskidegerz && degerx != eskidegerx )
  {
    fuzzy->setInput(1, degerx);
    fuzzy->setInput(2, degerz);
    fuzzy->fuzzify();
  
    hiz = fuzzy->defuzzify(1);

    eskidegerz = degerz;
    eskidegerx = degerx;
    
  }

  float solMotor,sagMotor;
  solMotor=sagMotor=hiz;
  sagMotor-=9;
  solMotor+=4;
  float orta = hiz * 0.55;//0,7//0.61
  float keskin = hiz * 0.35; // yakalanan degerler  0.66  0.57//0.41

  qtrrc.read(sensorValues);
  qtrrc2.read(sensorValues2);
  int s1, s2, s3, s4, s5, s6;
  s1 = sensorValues2[0];
  s2 = sensorValues2[1];
  s3 = sensorValues2[2];
  s4 = sensorValues[0];
  s5 = sensorValues[1];
  s6 = sensorValues[2];

  if( s3 < 100 && s2 <200 && s1 > 500 || s6 > 300 && s5 < 300 && s4 < 300 )
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, solMotor);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, sagMotor);
  }
  else if( s3 < 100 && s2 > 200 || s1 > 500 && s6 < 300 && s5 < 300 && s4 < 300 )
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, orta);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, sagMotor);
  }
  else if( s3 > 100 || s2 > 200 && s1 < 500 && s6 < 300 && s5 < 300 && s4 < 300 )
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, keskin);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, sagMotor);
  }
  else if( s3 < 100 && s2 < 200 && s1 < 500 && s6 > 300 && s5 > 300 && s4 < 300 )
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, solMotor);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, orta);
  }
  else if( s3 < 100 && s2 < 200 && s1 < 500 && s6 < 300 && s5 > 300 || s4 > 300 )
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, solMotor);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, keskin);
  }
  else
  {
    digitalWrite(in_1 , HIGH);
    digitalWrite(in_2 , LOW);
    analogWrite(pwm1, 0);
    digitalWrite(in_3 , LOW);
    digitalWrite(in_4 , HIGH);
    analogWrite(pwm2, 0);
  }


}

