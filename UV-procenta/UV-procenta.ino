//Tento program vam bude vracet hodnoty v mw/cm^2

// Arduino senzor UVA a UVB záření

// Návod na použití:
// nahrajte kod na arduino. Poté odkryjte sensor, aby na něj
// dopadala referenční hodnota záření a stiskněte vestavěné tlačítko RESET na Arduinu
// Až začne Sériový monitor vypisovat hodnoty, můžete začít měřit propustnost různých látak jejich kladením před sensor

// Pro detaily k přepočtu se podívejte do souboru UV-mw.ino
float U_max=573;//=2.8V   ----Přepočítané hodnoty napětí na hodnoty vrácené 
float U_min=196;//=0.96V  /   funkcí analogRead()
float Int_max=15.0;
float Int_min=0.0;

// pojmenujeme pin A0 jako:
#define vstupUV A0

// proměnná pro uložení referenční intenzity:
float UV_ref;
// -//- aktuální intenzity:
float UV_akt;

void setup() {
	// zahájení komunikace po sériové lince (USB) rychlostí 9600 baud
	Serial.begin(9600);

	// nastaví piny vstupUV a kontrola 3v3 jako vstupy
	pinMode(vstupUV, INPUT);

	// určí hodnotu referenční intenzitu:
	UV_ref = hodnotaNaMiliWatt(analogReadPrumer(vstupUV, 10, 100, 10));
	Serial.print(UV_ref);
	Serial.print("\t mW/cm^2 \t");
}

void loop() {
	int hodnota = analogReadPrumer(vstupUV, 10, 100, 10);
	UV_akt = hodnotaNaMiliWatt(hodnota);
	Serial.print(UV_akt);
	Serial.print("\t mW/cm^2 \t");
	Serial.print(UV_akt/UV_ref*100.0);
	Serial.println(" %\n");
}

int analogReadPrumer(int pin, int extremesToRemove, int measures, int prodleva){
	long prumer = 0;
	int value;
	int maxEx[extremesToRemove]={0,0,0,0,0,0,0,0,0,0};
	int minEx[extremesToRemove]={0,0,0,0,0,0,0,0,0,0};
	for(int i=0;i<measures;i++){
		value = analogRead(pin);
		int j=extremesToRemove-1;
		int y;
		while(j>=0 && value>maxEx[j]){
			if(value<maxEx[j-1]){
				y=value;
				value=maxEx[j];
				maxEx[j]=y;
				j++;
				while(j<extremesToRemove){
					y=value;
					value=maxEx[j];
					maxEx[j]=y;
					j++;
				}
				j=-1;
			}
			j--;
		}
		j=0;
		// Stejně jako celá předchozí smyčka, ale zde hledáme 10 
		// nejmenších čísel
		while(j>=0 && value<maxEx[j]){
			if(value>maxEx[j-1]){
				y=value;
				value=maxEx[j];
				maxEx[j]=y;
				j++;
				while(j<extremesToRemove){
					y=value;
					value=maxEx[j];
					maxEx[j]=y;
					j++;
				}
				j=-1;
			}
			j--;
		}
		// nakonec přičte hodnotu(bud z AG a nebo tu. která vypadla 
		// z extrémů) a vyčká nečinně po dobu prodlevy.
		prumer+=value;
		delay(prodleva);
	}
	// vydělí průměr počtem měření (2*10 extrémních jsme odstranili)
	// a ten následně vrátí jako výslednou hodnotu funkce
	prumer=prumer/(measures-2*extremesToRemove);
	return int(prumer);
}

// funkce pro převod jednotek z analogRead (0-1023) na Volty
float hodnotaNaVolt(int unit){
	// nejprve převede integer (celé číslo) unit na float (desetinné číslo)
	// a poté ho vydělí 1023 (nejvyšší hodnota z AR) a vynásobí pěti (odpovídající
	// hodnota napětí)
	float volt=(float(unit)*5.0/1023);
	return volt;
}

// -//- z analogRead na mW/cm^2
float hodnotaNaMiliWatt(int unit){
	// nejprve převede integer (celé číslo) unit na float (desetinné číslo) a přepočítá,
	// vše uloží do proměnné miliWatt, kterou vrátí
	float miliWatt=(float(unit)-U_min)*(Int_max-Int_min)/(U_max-U_min)+Int_min;
	return miliWatt;
}
