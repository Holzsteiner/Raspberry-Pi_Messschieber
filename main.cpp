#include <cstdio>
#include <wiringPi.h>
#include <time.h>

int main(void)
{
	timespec lt = {}, ct = {};
	
	wiringPiSetup();
	pinMode(13, INPUT);		//MISO für data als input
	pinMode(14, INPUT);		//SCLK für clock als input

	bool buffer[24] = {};		//24 bit daten-speicher
	
	int clkSignal = 0, i = 0;
	clock_gettime(CLOCK_MONOTONIC, &lt);
	while (true) {
		delayMicroseconds(40);		//Last aufheben

		int _clkSignal = digitalRead(14);	//clock signal lesen (bei 0V low, bei >0V high)

		if (clkSignal == 0 && _clkSignal == 1) {		//jetziges clock-Signal mit letztem abgleichen (von low auf high)
			clock_gettime(CLOCK_MONOTONIC, &ct);
			long tdiff = ct.tv_nsec - lt.tv_nsec;		//vergangene Zeit seit letztem clock-Signal berechnen
			lt = ct;

			if (tdiff > 10000000) {				//falls diese vergangene Zeit über 10 ms liegt, dann i auf 0 stellen (was den Anfang des 24 bit-Signals definiert)
				i = 0;
			}
			
			i++;

			if (i == 24) {

				int ret = 0;
				int tmp;
				for (int i = 0; i < 16; i++) {
					tmp = buffer[i];
					ret |= tmp << (i - 1);		//nachdem man am 24. bit angekommen ist, die 2 byte lange zahl aus dem 24 bit speicher in eine ganzzahl konvertieren
				}

				printf("%f mm\n", (float)ret/100.0f);	//Anzeige in mm(die ganzzahl durch 100 Teilen, da 100stel mm übertragen werden)
				i = 0;		//i auf 0 setzten, damit der 24-bit-buffer vom anfang an wieder beschrieben wird

				delay(100);				//Last wieder aufheben, da das nächste signal erst nach 100 ms oder später kommt
			}
			
			buffer[i] = digitalRead(13);			//24 bit-Speicher füllen 	
		}

		clkSignal = _clkSignal;					//altes clock signal mit dem neuen überschreiben, für nächsten loop
	}
	
	return 0;
}
