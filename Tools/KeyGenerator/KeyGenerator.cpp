// KeyGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>
#include <string>
#include <fstream>

#include "randomNumberGeneratorCryptoSecure.h"

int main(int argc, char **argv)
{
	if (argc == 1) {
		std::cout << "Key length is missing." << std::endl;
		std::cout << "File name is missing." << std::endl;
		return -1;
	}

	int keyLength = std::atoi(argv[1]);
	std::string fileName = argv[2];

	std::cout << "Key length: " << keyLength << std::endl;
	std::cout << "File name: " << fileName << std::endl;

	std::ofstream keyFile;
	keyFile.open(fileName);
	if (!keyFile.is_open()) {
		std::cout << "Failed to open the file " << fileName << " ." << std::endl;
		return -1;
	}

	keyFile << "const int keyLength = " << keyLength << ";" << std::endl;

	keyFile << "const int key[] = {";
	RandomNumberGeneratorCryptoSecure randomNumberGenerator; 
	for (int i = 0; i < keyLength; i++) {
		unsigned __int64 randomNumber = randomNumberGenerator.GetPositiveRandomInt64(255);
		if (i < keyLength-1) {
			keyFile << randomNumber << ", ";
		} else {
			keyFile << randomNumber;
		}
	}
	keyFile << "};" << std::endl;
	keyFile.close();
	
	return 0;
}