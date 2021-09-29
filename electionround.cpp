#include "electionround.h"
#include "electionresults.h"

namespace Election
{
	ElectionRound::ElectionRound(int day, int month, int year) : voteStarted(false)
	{
		if (month < 1 || month > 12)
			throw "Invalid month";
		if (MAXDAYS[month] < day || day < 1)
			throw "Invalid day";
		if (year < 1)
			throw "Invalid year";

		_day = day, _month = month, _year = year;

	}

	ElectionRound::ElectionRound(ifstream& inFile)
	{
		int d, m, y;
		inFile.read(rcastc(&_day), sizeof(int));
		inFile.read(rcastc(&_month), sizeof(int));
		inFile.read(rcastc(&_year), sizeof(int));
		inFile.read(rcastc(&voteStarted), sizeof(voteStarted));
		if (!inFile.good())
			throw "Error reading from file.";

		parties.load(inFile);
		votes.load(inFile);

	}

	void ElectionRound::printParties() const
	{
		int partyLogSize = parties.getLogSize();
		if (partyLogSize)
		{
			for (int i = 0; i < partyLogSize; i++)
			{
				std::cout << std::endl << i << ". ";
				parties.printMembers(i);
				std::cout << std::endl;
			}
		}
		else
			std::cout << "No parties to show." << std::endl;
	}

	void ElectionRound::addParty(const string& name, int idCandid)
	{
		parties.addParty(name, idCandid);

		if (parties.getPhySize() != votes.getSizeParty()) // increase votes array to reflect current party size
			votes.reSizeParty(parties.getPhySize());
	}

	void ElectionRound::addPartyMember(const Civilian& civ, int partyNum, int districtNum)
	{
		parties.addMember(civ, partyNum, districtNum);
	}

	bool ElectionRound::nameEqualParty(const string& name) const
	{
		bool found = false;
		for (int i = 0; i < parties.getLogSize() && !found; i++)
		{
			if (parties.getName(i) == name)
				found = true;

		}
		return found;
	}

	void ElectionRound::save(ofstream& outFile) const
	{
		int d = _day, m = _month, y = _year;
		ElectionGenerator::ElectionType type = ElectionGenerator::getType(this);
		outFile.write(rcastcc(&type), sizeof(type));
		outFile.write(rcastcc(&d), sizeof(d));
		outFile.write(rcastcc(&m), sizeof(m));
		outFile.write(rcastcc(&y), sizeof(y));
		outFile.write(rcastcc(&voteStarted), sizeof(voteStarted));
		if (!outFile.good())
			throw "couldn't write to file.";
		parties.save(outFile);
		votes.save(outFile);
	}

	void ElectionRound::setVote(int partyNum, Civilian* civ, int districtNum)
	{
		if (!voteStarted)
			voteStarted = true;
		votes.vote(partyNum, districtNum);
		civ->setVoted();
	}

	void ElectionRound::resultDistrict(vector<int>& resultsVotes, vector<double>& resPercent, vector<int>& resultsElectorate,int &maxIndex, int districtNum, const District& district)
	{
		int currVote, maxRes = 0, partiesLogSize = parties.getLogSize(), sumVote = district.getVoterCount();
		int type = typeid(district) == typeid(DividedDistrict);
		for (int j = 0; j < partiesLogSize; j++)
		{
			currVote = votes.getNumVotes(districtNum, j);

			resultsVotes[j] += currVote; // update total #  of votes

			if (currVote > maxRes)
			{
				maxRes = currVote;
				maxIndex = j;
			}
			resPercent[j] = (static_cast<double>(currVote) / sumVote) * 100; // update vote % of party 'j' in received district
			if (type) // divided district
				resultsElectorate[j] += district.getElectorate() * (resPercent[j] / 100); // give electorates according to %
		}
	}
	
	void ElectionRound::setResults(vector<int>& resVotes, vector<int>& resultsElectorate)
	{
		int partiesLogSize = parties.getLogSize();  

		for (int i = 0; i < partiesLogSize; i++) // update parties to reflect election results
		{
			setPartyVotes(i, resVotes[i]);
			setPartyElectorates(i, resultsElectorate[i]);
		}
	}

	void ElectionRound::sortParties(Party** arr, int size, const PartyArr& parties)
	{
		for (int i = 0; i < size; i++) // init array of party pointers
			arr[i] = const_cast<Party*>(parties.getPartyRef(i));

		sort(arr, size); // sort parties according to no. of electorates via template function

	}

	 // NormalRound methods 
	NormalRound::NormalRound(ifstream& inFile) : ElectionRound(inFile), districts(DistrictArr(inFile))
	{
	}

	bool NormalRound::nameEqualDistrict(const string& name) const
	{
		bool found = false;
		for (int i = 0; i < districts.getLogSize() && !found; i++)
		{
			if (districts.getName(i) == name)
				found = true;
		}
		return found;
	}

	void NormalRound::addDistrict(const string& name, int electorate, int type)
	{
		districts.addDistrict(name, electorate, type);
		int districtLogSize = districts.getLogSize(),
			districtPhySize = districts.getPhySize();

		if (districtPhySize != votes.getSizeDistrict()) // increase votes array to reflect current district size
			votes.reSizeDistrict(districtPhySize);

		if (parties.getLogSize() && districtLogSize > parties.getMembersSize(0)) // increase member lists of the parties to reflect current district size
		{
			for (int i = 0; i < parties.getLogSize(); i++)
			{
				if (parties.getMembersSize(i) < districtLogSize)
					parties.reSizeMembers(districtLogSize, i);
			}
		}
	}

	void NormalRound::addCivilian(const string& name, int id, int year, int districtNum)
	{
		districts.addCivilian(name, id, year, districtNum);
	}

	void NormalRound::addPartyMember(const Civilian& civ, int partyNum, int districtNum)
	{
		int districtLogSize = districts.getLogSize();

		if (districtLogSize > parties.getMembersSize(0)) // increase member lists of the parties to reflect current district size
		{
			for (int i = 0; i < parties.getLogSize(); i++)
			{
				if (parties.getMembersSize(i) < districtLogSize) 
					parties.reSizeMembers(districtLogSize, i);
			}
		}
		ElectionRound::addPartyMember(civ, partyNum, districtNum);
	}

	void NormalRound::printCivs() const
	{
		bool hasCivs = false;
		for (int i = 0; i < districts.getLogSize() && !hasCivs; i++)
		{
			if (districts.getCivCount(i))
				hasCivs = true;
		}
		if (hasCivs)
			districts.printCivs();
		else
			std::cout << "No civilians to show. " << std::endl;
	}

	void NormalRound::setVote(int partyNum, Civilian* civ, int districtNum)
	{
		ElectionRound::setVote(partyNum, civ, districtNum);
		districts.IncreaseVoteCount(districtNum);
	}

	bool NormalRound::civilianExists(int id) const
	{
		return (getCivPtr(id) != nullptr);
	}

	void NormalRound::printDistricts() const
	{
		std::cout << districts;
	}
	
	void NormalRound::results(ElectionResults* res)
	{
		int partiesLogSize = parties.getLogSize(), DisLogSize = districts.getLogSize(), sumVote, maxIndex = 0, districtType;
		NormalResults* normalRes = dynamic_cast<NormalResults*>(res);
		if (!normalRes)
			throw "could not show the results.";

		vector<int>& resultsElectorate = normalRes->resElectorate;
		vector<int>& resultsVotes = normalRes->resVotes;
		vector<vector<double>>& resPercent = normalRes->resPercent;
		vector<int>& winners = normalRes->winners;

		for (int i = 0; i < DisLogSize; i++)
		{
			sumVote = districts.getVoterCount(i);
			if (typeid(districts[i]) == typeid(DividedDistrict))
				districtType = 1;
			else
				districtType = 0;

			if (sumVote) // check if there's data to calculate
			{
				ElectionRound::resultDistrict(resultsVotes, resPercent[i], resultsElectorate, maxIndex, i, districts[i]);

				if (!districtType) // unified district
					resultsElectorate[maxIndex] += districts.getElectorate(i); // give all electorates to winning party

				winners[i] = maxIndex;
				maxIndex = 0;
			}
		}

		setResults(resultsVotes, resPercent, resultsElectorate);
	}

	void NormalRound::setResults(vector<int>& resVotes, vector<vector<double>>& resPercent, vector<int>& resultsElectorate)
	{
		int partiesLogSize = parties.getLogSize(), DisLogSize = districts.getLogSize(), sum, electorate;
		for (int i = 0; i < DisLogSize; i++)
		{
			electorate = districts.getElectorate(i);
			sum = 0;
			for (int j = 0; j < partiesLogSize; j++)
			{
				districts.updateRepresentatives(sum, (electorate * resPercent[i][j] / 100), i, parties[j]); // update party representatives in district 'i' 
				sum += electorate * resPercent[i][j] / 100;
			}
		}

		ElectionRound::setResults(resVotes, resultsElectorate);
	}


	// SimpleRound methods
	SimpleRound::SimpleRound(int day, int month, int year, int electorates) : ElectionRound(day, month, year), district("Main District", electorates)
	{
	}

	SimpleRound::SimpleRound(ifstream& inFile) : ElectionRound(inFile)
	{
		DistrictGenerator::DistrictType type;
		inFile.read(rcastc(&type), sizeof(type));
		if (!inFile.good())
			throw "Error reading from file.";

		district.load(inFile);
	}

	void SimpleRound::addCivilian(const string& name, int id, int year, int districtNum)
	{
		district.addCiv(name, id, year);
	}

	void SimpleRound::printDistricts() const
	{
		std::cout << district;
	}

	void SimpleRound::printCivs() const
	{
		if (district.getNumCiv())
		{
			district.printCivilianList();
		}
		else
			std::cout << "No civilians to show. " << std::endl;
	}

	bool SimpleRound::civilianExists(int id) const
	{
		return (getCivPtr(id) != nullptr);
	}

	void SimpleRound::setVote(int partyNum, Civilian* civ, int districtNum)
	{
		ElectionRound::setVote(partyNum, civ, 0);
		district.IncreaseVoterCount();
	}
	
	void SimpleRound::results(ElectionResults* res)
	{
		int partiesLogSize = parties.getLogSize(), sumVote, winner;

		SimpleResults* simpleRes = dynamic_cast<SimpleResults*>(res);
		if (!simpleRes)
			throw "could not show the results.";

		vector<int>& resultsElectorate = simpleRes->resElectorate;
		vector<int>& resultsVotes = simpleRes->resVotes;
		vector<double>& resPercent = simpleRes->resPercent;

		ElectionRound::resultDistrict(resultsVotes, resPercent, resultsElectorate, simpleRes->winner, 0, district);

		setResults(resultsVotes, resPercent, resultsElectorate);
	}

	void SimpleRound::setResults(vector<int>& resVotes, vector<double>& resPercent, vector<int>& resultsElectorate)
	{
		int partiesLogSize = parties.getLogSize(), electorate = district.getElectorate(), sum = 0;
		for (int i = 0; i < partiesLogSize; i++)
		{
			district.updateRepresentatives(sum, (electorate * resPercent[i] / 100), 0, parties[i]); // update party representatives main district
			sum += electorate * resPercent[i] / 100;
		}

		ElectionRound::setResults(resVotes, resultsElectorate);
	}
}