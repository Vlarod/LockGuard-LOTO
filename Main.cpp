#include <thread>
#include <mutex>
#include <iostream>
#include <map>
#include <stdlib.h>   
#include <random>
#include <string>
#include <condition_variable>
#include "LockGuardMy.h"

#define LOWER_LIMIT 0
#define UPPER_LIMIT 25
#define NUMBER_OF_NUMBERS 5
#define NOT_FOUND 0
#define OFFSET 1


std::mutex *mutex;
std::map<int, int> * generatedNumbersPlayer;
std::map<int, int> * drawnNumbers;
std::minstd_rand0 generator;
int score = 0;

void printGeneratedNumbers(std::map<int, int> * generatedNumbers, std::string title){
	std::cout << title;
	std::map<int, int>::iterator it;

	for (it = generatedNumbers->begin(); it != generatedNumbers->end(); it++)
	{
		std::cout << it->first << "; ";
	}
}

int getRandomNumber(int min, int max) {
	return min + generator() % ((max + 1) - min);
}

void generateNumbers(std::map<int, int> * generatedNumbersMap) {
	for (int i = 0; i < NUMBER_OF_NUMBERS; i++) {
		int newNumber = getRandomNumber(LOWER_LIMIT, UPPER_LIMIT);

		if (generatedNumbersMap->count(newNumber)) {
			i--;
		}
		else {
			generatedNumbersMap->insert(std::pair<int, int>(newNumber, newNumber));
		}
	}
}

void getResults() {
	std::cout << "Results Waiting\n";
	myLG::LockGuardMy<std::mutex> lockGuardMy(mutex);
	std::cout << "Results Processing\n";

	std::map<int, int>::iterator it;
	for (it = generatedNumbersPlayer->begin(); it != generatedNumbersPlayer->end(); it++)
	{
		int currentNumber = it->first;

		if (drawnNumbers->count(currentNumber)) {
			score += 2;
		}
	}
}

void getResultsSecondChance() {
	std::cout << "ResultsSec Waiting\n";
	myLG::LockGuardMy<std::mutex> lockGuardMy(mutex);
	std::cout << "ResultsSec Processing\n";

	std::map<int, int>::iterator it;
	for (it = generatedNumbersPlayer->begin(); it != generatedNumbersPlayer->end(); it++)
	{
		int currentNumber = it->first;
		if (drawnNumbers->count(currentNumber + OFFSET) || drawnNumbers->count(currentNumber - OFFSET)) {
			score++;
		}
	}
}

void enterNumbersUser() {

	std::cout << "-----WELCOME in game LOTO 5 from 25-----\nWrite 5 numbers from range 0 - 25!\n";
	int currentnumber;
	for (int i = 0; i < NUMBER_OF_NUMBERS; i++) {
		std::cin >> currentnumber;

		if (currentnumber >= LOWER_LIMIT && currentnumber <= UPPER_LIMIT && generatedNumbersPlayer->count(currentnumber) == NOT_FOUND) {
			generatedNumbersPlayer->insert(std::pair<int, int>(currentnumber, currentnumber));
		}
		else {
			i--;
			std::cout << "Entred number is not from range 0 - 25 or is used yet! Choose another one!\n";
		}
	}
}

void printResults() {
	printGeneratedNumbers(generatedNumbersPlayer, "\nGenerated numbers for Player: ");
	printGeneratedNumbers(drawnNumbers, "\n\nDrawn numbers: ");
	std::cout << "\n\nYour Score: " << score << "\n\n\n";
}

int main() {
	mutex = new std::mutex;
	generatedNumbersPlayer = new std::map<int, int>();
	drawnNumbers = new std::map<int, int>();

	generator.seed(time(NULL));
	std::thread enter(enterNumbersUser);
	std::thread drawnN(generateNumbers, drawnNumbers);

	drawnN.join();
	enter.join();

	std::thread results(getResults);
	std::thread resultsSecondChance(getResultsSecondChance);

	results.join();
	resultsSecondChance.join();

	printResults();

	delete generatedNumbersPlayer;
	delete drawnNumbers;
	delete mutex;
	
	system("pause");

	return 0;
}
