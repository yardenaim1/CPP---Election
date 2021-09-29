#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "electionround.h"
#include "electionresults.h"

using namespace Election;
using namespace std;

enum class usChoice // user choices for menu
{
	DISTRICT = 1, CIVILIAN, PARTY, CIVTOPARTY, DISPDISTRICT, DISPCIV, DISPPARTY, VOTE, RESULT, EXIT, SAVEROUND , LOADROUND
};

static constexpr int MAXNAME = 20; // we assume the user inputs the name as a single word with no white space

void printStartMenu();
void printMenu();
ElectionRound* startingOption(int choiceNum);
void addDistrict(ElectionRound* election);
void addCivilian(ElectionRound* election);
void addParty(ElectionRound* election);
void makeCivPartyRep(ElectionRound* election);
void castVote(ElectionRound* election);
void showResults(ElectionRound* election);
void saveRound(ElectionRound* election);