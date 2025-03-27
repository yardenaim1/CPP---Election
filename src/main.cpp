#include <iostream>
#include "electionround.h"
#include "menufunctions.h"

// defines to handle try-catch blocks of code
#define CATCH catch (const char* msg){ cout << "Error: " << msg << endl << endl;}
#define CATCH_FILE catch (const char* msg){ cout << "Error: " << msg << endl << "Could not create an election round." << endl; exit(1); }
#define CATCH_ALLOC catch (bad_alloc& ex) { cout << "Error: " << ex.what() << endl; exit(2);}

void Menu()
{
	ElectionRound* election;
	usChoice choice;
	int choiceNum;
	bool flag = true;

	printStartMenu();
	cin >> choiceNum;
	while (choiceNum < 1 || choiceNum > 3) // valid menu options																
	{
		cout << "Invalid input." << endl << endl;
		printStartMenu();
		cin >> choiceNum;
	}
	try 
	{
		if (choiceNum == 3) // exit 
			return;
		else
			election = startingOption(choiceNum); // create election round
	}
	CATCH_FILE
	CATCH_ALLOC

	cout << "Welcome to the elections of " << election->getDay() << " / " << election->getMonth() << " / " << election->getYear() << endl << endl;

	while (flag)
	{
		printMenu();
		cin >> choiceNum;
		choice = static_cast<usChoice>(choiceNum);

		switch (choice)
		{
			case usChoice::DISTRICT: // adding a district
			{
				try { addDistrict(election); }
				CATCH
				CATCH_ALLOC

				break;
			}
			case usChoice::CIVILIAN:  // adding a civilian
			{
				try { addCivilian(election); }
				CATCH
				break;
			}
			case usChoice::PARTY:  // adding a party
			{ 
				try{ addParty(election); }
				CATCH

 				break;
			}
			case usChoice::CIVTOPARTY: // adding a civilian as a district rep for a party
			{
				try { makeCivPartyRep(election); }
				CATCH

				break;
			}
			case usChoice::DISPDISTRICT: // display districts
			{
				election->printDistricts();
				cout << endl;
				break;
			}
			case usChoice::DISPCIV: // display civilians
			{
				election->printCivs();
				break;
			}
			case usChoice::DISPPARTY: // display parties
			{
				election->printParties();
				cout << endl;
				break;
			}
			case usChoice::VOTE: // cast vote
			{
				try { castVote(election); }
				CATCH

				break;
			}
			case usChoice::RESULT: // show results
			{
				try { showResults(election); }
				CATCH
				CATCH_ALLOC

				break;
			}
			case usChoice::EXIT:
			{
				flag = false;
				cout << "Elections are over. Have a nice day." << endl;
				
				break;
			}
			case usChoice::SAVEROUND: // save elections to file
			{
				try { saveRound(election); }
				CATCH
				break;
			}
			case usChoice::LOADROUND: // load elections from file
			{
				cout << "Please enter file name:";
				char fileName[MAXNAME];
				cin >> fileName;
				ifstream inFile(fileName, ios::binary);
				if (!inFile) {
					cout << "Error opening the file " << endl;
					break;
				}
				delete election;
				try { election = ElectionGenerator::loadElection(inFile); }
				CATCH_FILE
				CATCH_ALLOC

				cout << endl << fileName << " successfully loaded." << endl << endl;
				break;
			}
			default :
			{
				cout << endl << "Invalid input." << endl << endl;
				break;
			}
		}
	}	
}

int main()
{
	try 
	{
		Menu();
	}
	catch (const std::exception ex)
	{
		std::cout << "Error: " << ex.what() << std::endl;
	}
	return 0;
}