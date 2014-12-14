/*
 * SterownikLED.cpp
 *
 *  Created on: 25 lis 2014
 *      Author: Dominik
 */

#include "SterownikLED.h"

SterownikLED::SterownikLED() {
	poprzednia_wartosc = -1;
	ilosc_niezmiennych_wartosci = 0;
	mode = 0;
	czas_pomiedzy_pomiarami = WORK_TIME;

}

SterownikLED::~SterownikLED() {
	// TODO Auto-generated destructor stub
}

void SterownikLED::init() {
	ustawRejestryDDRC();
	ustawRejestryTimer0();
}

void SterownikLED::ustawRejestryDDRC() {
	DDRC |= (1 << PC0);
	DDRC |= (1 << PC1);
	DDRC |= (1 << PC2);
}

void SterownikLED::ustawRejestryTimer0() {
	TCCR0 |= (1 << CS00); // �r�d�em CLK, preskaler 1
	TIMSK |= (1 << TOIE0);          //Przerwanie overflow (przepe�nienie timera)
}

void SterownikLED::setReferences(volatile uint8_t & R, volatile uint8_t & G, volatile uint8_t & B, volatile uint8_t & odl) {
	reference_R = &R;
	reference_G = &G;
	reference_B = &B;
	reference_odl = &odl;

}

void SterownikLED::ustawRGB(volatile uint8_t red, volatile uint8_t green, volatile uint8_t blue) {
	*reference_R = red;
	*reference_G = green;
	*reference_B = blue;
}

void SterownikLED::pokaz_swiatel_RGB(volatile uint8_t & R, volatile uint8_t & G, volatile uint8_t & B) {
	uint8_t i = 0;
	R = 0;
	G = 0;
	B = 0;
	for (i = 0; i < MAX; i++) {
		R = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		R = i;
		_delay_ms(5);
	}
	for (i = 0; i < MAX; i++) {
		G = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		G = i;
		_delay_ms(5);
	}
	for (i = 0; i < MAX; i++) {
		B = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		B = i;
		_delay_ms(5);
	}

	for (i = 0; i < MAX; i++) {
		B = i;
		R = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		B = i;
		R = i;
		_delay_ms(5);
	}
	for (i = 0; i < MAX; i++) {
		B = i;
		G = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		B = i;
		G = i;
		_delay_ms(5);
	}

	for (i = 0; i < MAX; i++) {
		R = i;
		G = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		R = i;
		G = i;
		_delay_ms(5);
	}

	for (i = 0; i < MAX; i++) {
		R = i;
		G = i;
		B = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		R = i;
		G = i;
		B = i;
		_delay_ms(5);
	}

	for (i = 0; i < MAX; i++) {
		R = i;
		G = i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		R = i;
		_delay_ms(5);
	}

	for (i = 0; i < MAX; i++) {
		R = i;
		G = 100 - i;
		_delay_ms(5);
	}
	for (i = MAX; i > 0; i--) {
		B = 100 - i;
		_delay_ms(5);
	}

}

void SterownikLED::mode_wjazdDoGarazu() {
	int pochodna;
	if (poprzednia_wartosc == -1)
		poprzednia_wartosc = *reference_odl; /* pierwszy przebieg */
	else {

		pochodna = poprzednia_wartosc - *reference_odl; /* je�eli >0 obiekt zbli�a si� do czujnika */

		if (*reference_odl > granica_zielone && pochodna > 0) { /* Warunki w przypadku zbli�ania si� do �ciany  */
			ustawRGB(0, 0, 0);
		} else if (*reference_odl <= granica_zielone && *reference_odl > granica_zolte && pochodna > 1) {
			ustawRGB(0, MAX, 0);
		} else if (*reference_odl <= granica_zolte && *reference_odl > granica_czerwone && pochodna > 1) {
			ustawRGB(MAX, MAX, 0);
		} else if (*reference_odl <= granica_czerwone && pochodna > 1) {
			ustawRGB(MAX, 0, 0);
		} else if (pochodna < -1 && *reference_odl <= (granica_czerwone + histereza)) {
			ustawRGB(MAX, 0, 0);
		} else if (pochodna < -1 && *reference_odl > (granica_czerwone + histereza)
				&& *reference_odl <= (granica_zolte + histereza)) {
			ustawRGB(MAX, MAX, 0);
		} else if (pochodna < -1 && *reference_odl > (granica_zolte + histereza)
				&& *reference_odl <= (granica_zielone + histereza)) {
			ustawRGB(0, MAX, 0);
		} else if (pochodna < -1 && *reference_odl > (granica_zielone + histereza)) {
			ustawRGB(0, 0, 0);
		} else if (pochodna >= -1 && pochodna <= 1 && ilosc_niezmiennych_wartosci < 22 && *reference_odl <= granica_czerwone) {
			ilosc_niezmiennych_wartosci++;
		}
	}

	if (pochodna < -1 && pochodna > 1) {
		ilosc_niezmiennych_wartosci = 0;
	}
	if (ilosc_niezmiennych_wartosci > 20) {
		ustawRGB(MAX, MAX, MAX);
	}
	poprzednia_wartosc = *reference_odl;

}

void SterownikLED::mode_wyjazdZGarazu() {

}

void SterownikLED::ustawSwiatlo() {
	mode = 1;
	if (mode == 1) {
		mode_wjazdDoGarazu();
	}
}

uint8_t SterownikLED::getCzasPomiedzyPomiarami() const {
	return czas_pomiedzy_pomiarami;
}

void SterownikLED::setCzasPomiedzyPomiarami(uint8_t czasPomiedzyPomiarami) {
	czas_pomiedzy_pomiarami = czasPomiedzyPomiarami;
}

