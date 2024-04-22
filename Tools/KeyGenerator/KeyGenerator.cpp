// KeyGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.

// OpenTwin header
#include "OTRandom/RandomNumberGeneratorCryptoSecure.h"

// std header
#include <iostream>
#include <string>
#include <fstream>

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
	
	keyFile << "#pragma once" << std::endl << std::endl;
	keyFile << "namespace ot {" << std::endl << std::endl;

	keyFile << "\t//! @brief The length of the encryption key" << std::endl;
	keyFile << "\tconst int EncryptionKeyLength = " << keyLength << ";" << std::endl << std::endl;

	keyFile << "\t//! @brief OpenTwin encryption key" << std::endl;
	keyFile << "\tconst int EncryptionKey[] = {";
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

	keyFile << "}" << std::endl;
	keyFile.close();
	
	return 0;
}