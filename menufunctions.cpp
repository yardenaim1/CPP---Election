#include "menufunctions.h"

void printStartMenu()
{
	cout << "To start an election, choose an option:" << endl;
	cout << "1. Create a new election round " << endl;
	cout << "2. Load an existing election round " << endl;
	cout << "3. Exit " << endl;
}

void printMenu()
{
	cout << "Main Menu\n__________\n\nChoose one of the following options:\n\n";
	cout << "1. Add district \n2. Add civilian\n3. Add party\n";
	cout << "4. Add a civilian as a party rep \n5. Display districts \n6. Display civilians\n";
	cout << "7. Display parties \n8. Vote \n9. Show results \n10. Exit\n11. Save election round\n12. Load election round" << endl;
}



ElectionRound* startingOption(int choiceNum)
{	
		if (choiceNum == 1) // create new election round
		{
			int day, month, year, roundType;
			cout << "Please enter the date (Day, Month, Year - with enter between them): " << endl;
			cin >> day >> month >> year;
			cout << "Choose election type:" << endl << "1. Normal election round" << endl << "2. Simple election round" << endl;
			cin >> roundType;

			while (roundType < 1 || roundType > 2) // valid starting menu options
			{
				cout << "Invalid input." << endl << endl;
				cout << "Choose election type:" << endl << "1. Normal election round" << endl << "2. Simple election round" << endl;
				cin >> roundType;
			}
			try 
			{
				if (roundType == 1) //normal round
				{
					return new NormalRound(day, month, year);
				}
				else // simple round
				{
					int electorate;
					cout << "Enter number of electorates: ";
					cin >> electorate;
					return new SimpleRound(day, month, year, electorate);
				}
			}

			catch (bad_alloc& ex)
			{
				throw ex;
			}

		}
		else   // load from file
		{
			cout << "Please enter file name:";
			char fileName[MAXNAME];
			cin >> fileName;
			ifstream inFile(fileName, ios::binary);
			if (!inFile) {
				throw "Error opening the file";
			}
			cout << endl << fileName << " successfully loaded." << endl << endl;
			return ElectionGenerator::loadElection(inFile);

		}	
}

void addDistrict(ElectionRound* election)
{
	if (typeid(*election) == typeid(SimpleRound))
	{
		throw "cannot add districts in a simple election round.";
	}
	int electorates, type;
	string districtName;
	cout << "Enter name of district, number of electorates: ";
	cin >> districtName >> electorates;
	while (electorates < 1) // assuming at least 1 electorate for each district
	{
		cout << endl << "Invalid input. Please enter correct number of electorates: ";
		cin >> electorates;
	}
	cout << "Choose district type (0 - unified / 1 - divided): ";
	cin >> type;
	while (type < 0 || type > 1) 
	{
		cout << endl << "Invalid input. Please enter a valid type of district: ";
		cin >> type;
	}
	if (election->nameEqualDistrict(districtName))
	{
		throw "Invalid input. This district already exists. ";
	}

	election->addDistrict(districtName, electorates, type);
	cout << endl << districtName << " added as a ";
	if (type)
		cout << "divided ";
	else
		cout << "unified ";
	cout << "district." << endl << endl;
}

void addCivilian(ElectionRound* election)
{
	if (!election->getDistrictLogSize()) // no districts case
	{
		throw "No districts available. Please enter a district first.";
	}

	int year, id, districtNum;
	string civName;
	cout << "Enter name, ID, year of birth and district number: ";
	cin >> civName >> id >> year >> districtNum;

	if (typeid(*election) == typeid(NormalRound) && election->getDistrictLogSize() <= districtNum)  // input of a non-existing district in a normal round
	{
		throw "Invalid input. No such district.";
	}
	else if (year <= 0 || year > election->getYear() || (election->getYear() - year) < 18) // input of a birth year later than election / civilian at least 18 years old / positive number
	{
		throw "Invalid input. Enter a valid year of birth.";
	}
	else if (election->getCivPtr(id)) // input of an already added civilian
	{
		throw "Invalid input. This civilian already exists.";
	}
	else
	{
		try { election->addCivilian(civName, id, year, districtNum); }
		catch (bad_alloc& ex) { cout << ex.what() << endl; throw "could not allocate memory for Civillian"; }
		cout << endl << civName << " added as a civilian to district " << election->getDistrictName(districtNum) << endl;
	}
}

void addParty(ElectionRound* election)
{
	string partyName;
	int idCandid;
	cout << "Enter party name, and ID of the presidency candidate of the party: ";
	cin >> partyName >> idCandid;

	if (!(election->civilianExists(idCandid))) // check if party leader exists
		throw "No matching civilian found for this ID.";

	if (election->nameEqualParty(partyName)) // check if this party name exists
		throw "This party name is taken.";

	election->addParty(partyName, idCandid);
	cout << endl << partyName << " added as a party." << endl << endl;
}

void makeCivPartyRep(ElectionRound* election)
{
	const Civilian* civ;
	int id, pNum, dNum;
	cout << "Enter ID of the civilian to add, party number and district number: ";
	cin >> id >> pNum >> dNum;

	if (pNum < 0 || pNum >= election->getPartiesLogSize()) // input of a non-valid number or non-existing party
		throw "Invalid input. No such party exists";

	if (typeid(*election) == typeid(NormalRound) && (dNum < 0 || dNum >= election->getDistrictLogSize())) // input of a non-valid number or non-existing district in a normal round
		throw "Invalid input. No such district exists";

	civ = election->getCivPtr(id);
	if (!civ)
		throw "Invalid ID input. No such civilian exists";


	if (typeid(*election) == typeid(SimpleRound)) // if election is a simple round - change input from user to match the only district index 
		dNum = 0;

	election->addPartyMember(*civ, pNum, dNum);
	cout << endl << civ->getName() << " added as a " << election->getPartyName(pNum) << " representative in district " << dNum << endl << endl;
}

void castVote(ElectionRound* election)
{
	int id, pNum, dNum;
	Civilian* civ;

	cout << "To vote - enter an ID and the party to vote for: ";
	cin >> id >> pNum;
	if (pNum < 0 || pNum > election->getPartiesLogSize()) // invalid party number
		throw "Invalid party input.";
	else // check if a civilian with this ID exists
	{
		civ = const_cast<Civilian*>(election->getCivPtr(id, &dNum));
		if (!civ)
			throw "Invalid ID input. No such civilian exists";

		else if (civ->getVoteState() == true) // check if he didn't already vote
		{
			throw "This person has already voted.";
		}
		else
		{
			election->setVote(pNum, civ, dNum);
			cout << "Vote registered." << endl << endl;
		}
	}
}

void showResults(ElectionRound* election)
{
	if (election->checkVoteStarted())
	{
		cout << endl << "Results of the elections of " << election->getDay() << " / " << election->getMonth() << " / " << election->getYear() << endl << endl;
		ElectionResults* results = nullptr;
		NormalRound* norm = dynamic_cast<NormalRound*>(election);
		SimpleRound* simp = dynamic_cast<SimpleRound*>(election);

		// check type of current election to get correct ctor for results class
		try
		{
			if (typeid(*election) == typeid(NormalRound) && norm)
				results = new NormalResults(election->getPartiesLogSize(), election->getDistrictLogSize(), *norm);
			if (typeid(*election) == typeid(SimpleRound) && simp)
				results = new SimpleResults(election->getPartiesLogSize(), election->getDistrictLogSize(), *simp);
		}
		catch (bad_alloc& ex) { throw ex; }
		election->results(results);
		results->showResults();
		delete results;
	}
	else
		throw "No results to show.";
}

void saveRound(ElectionRound* election)
{
	char fileName[MAXNAME];
	cout << "Please enter file name: ";
	cin >> fileName;

	ofstream outFile(fileName, ios::binary);
	if (!outFile)
		throw "Error opening the file";

	election->save(outFile);
	cout << endl << "Elections saved to " << fileName << endl << endl;
}