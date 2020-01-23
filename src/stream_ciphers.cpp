//============================================================================
// Name        : Project_2.cpp
// Author      : Rizwan Kassamali
// Version     :
// Description : Stream Cipher Encryption
//============================================================================

#include <iostream>
#include <cmath>

char* encode(char *plaintext, unsigned long key);
char* decode(char *cihpertext, unsigned long key);
void swap_char(unsigned int j, unsigned int i, unsigned char s[]);


int main() {
	char str0[] { "Hello world!" };
	unsigned long key = 51323;

	std::cout << "Input: " << str0 << "" << std::endl << std::endl;

	char *ciphertext { encode(str0, key) };

	std::cout << "Encrypted: " << ciphertext << "" << std::endl << std::endl;

	char *plaintext { decode(ciphertext, key) };

	std::cout << "Decrypted: " << plaintext << "" << std::endl;

	return 0;
}


char* encode(char *plaintext, unsigned long key) {

// Convert the key to binary ///////////////////////

	unsigned int binaryNum[64] { 0 };

	for (unsigned int a = 0; key > 0; key /= 2, a++)
		binaryNum[a] = key % 2;

//Part One, Scramble the state array ///////////////////

	unsigned char s[256];

	for (int x = 0; x < 256; ++x)
		s[x] = x;

	unsigned char i = 0, j = 0, k = 0;

	for (int c = 0; c < 256; ++c, k = i % 64) {
		j = (j + s[i] + binaryNum[k]) % 256;
		swap_char(j, i, s);
		i = (i + 1) % 256;
	}

// Find length on plain text array and how many null chars to add //////////////////////

	unsigned int len = 0, pad = 0;

	while (plaintext[len] != '\0')
		++len;

	if (len % 4 != 0)
		pad = 4 - (len % 4);

	unsigned int newlen = len + pad;

// Create new array and copy all values adding null values at the end

	char *stage2 = new char[newlen] { '\0' };

	for (unsigned int x = 0; x < len; ++x)
		stage2[x] = plaintext[x];

// Get R and XOR with the key //////////////////////////

	unsigned char ctxt[newlen] { '\0' };

	for (unsigned int w = 0; w < newlen; ++w) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		swap_char(j, i, s);
		ctxt[w] = stage2[w] ^ s[((s[i] + s[j]) % 256)];

	}

// ASCII Amour ////////////////////////

	char *complete = new char[(newlen * 5 / 4) + 1];

	for (unsigned int go = 0, rounds = 0; go < newlen; ++rounds) {
		unsigned int fourchars[32] { 0 }, adder = 24, base = 1, dec = 0;

		//get 32 bit number from 4 chars at a time
		for (unsigned int y = 0; y < 4; ++y, ++go, adder -= 8)
			for (unsigned int f = 0; f < 8; ++f, ctxt[go] /= 2)
				fourchars[f + adder] = ctxt[go] % 2;

		//to decimal from binary
		for (unsigned int x = 0; x < 32; ++x, base *= 2)
			dec += fourchars[x] * base;

		//decimal number to base85
		for (unsigned int x = 0; x < 5; ++x, dec /= 85)
			complete[(4 - x) + (5 * rounds)] = dec % 85 + 33;
	}
	return complete;
}

char* decode(char *ciphertext, unsigned long key) {

// Get length of cipher text array and make new array to hold values before we XOR

	unsigned int len = 0;
	while (ciphertext[len] != '\0')
		++len;

	unsigned int newlen = (4 * len / 5) + 1;

	char pre_xor[newlen] { '\0' };

//Undo ASCII Armour

	for (unsigned int go = 0, rounds = 0; go < len; ++rounds) {
		unsigned long dec = 0;

		for (int m = 0; m < 5; ++m, ++go)
			dec += (pow(85, 4 - m) * (ciphertext[go] - 33));

		int bin[32] { 0 };

		for (unsigned int a = 0; dec > 0; dec /= 2, a++)
			bin[a] = dec % 2;

		for (unsigned int m = 0, adder = 0; m < 4; ++m, adder += 8)
			for (unsigned int n = 0, base = 1; n < 8; ++n, base *= 2)
				pre_xor[(3 - m) + 4 * rounds] += bin[n + adder] * base;
	}

// Convert the key to binary

	unsigned int keyAsBits[64] { 0 };

	for (unsigned int a = 0; key > 0; key /= 2, a++)
		keyAsBits[a] = key % 2;

// Scramble the key

	unsigned char s[256];

	for (int x = 0; x < 256; ++x)
		s[x] = x;

	unsigned int i = 0, j = 0, k = 0;

	for (int c = 0; c < 256; ++c, k = i % 64) {
		j = (j + s[i] + keyAsBits[k]) % 256;
		swap_char(j, i, s);
		i = (i + 1) % 256;
	}

// Find R and XOR with the cipher text after ASCII armor has been taken off

	char *plaintext = new char[newlen] { '\0' };

	for (unsigned int w = 0; w < newlen; ++w) {
		i = (i + 1) % 256;
		j = (j + s[i]) % 256;
		swap_char(j, i, s);
		plaintext[w] = pre_xor[w] ^ s[((s[i] + s[j]) % 256)];
	}
	plaintext[newlen - 1] = '\0';

	return plaintext;
}

void swap_char(unsigned int j, unsigned int i, unsigned char s[]) {
	unsigned int temp;
	temp =  s[j], s[j] = s[i], s[i] = temp;
}
