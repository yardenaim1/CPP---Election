#include "electionresults.h"

namespace Election
{
	ElectionResults::ElectionResults(int partySize, int districtSize) : resElectorate(vector<int>()), resVotes(vector<int>()),
																		_districtSize(districtSize), _partySize(partySize)
	{
		resElectorate.resize(partySize);
		resVotes.resize(partySize);
	}

	void ElectionResults::showDistrictResults(int districtNum, vector<double>& resPercent, int winner, const string& WinnerName, const District& district, const ElectionRound& election) const
	{
		for (int j = 0; j < _partySize; j++)
		{
			std::cout << election.parties.getName(j) << " has " << election.votes.getNumVotes(districtNum, j) << " votes - " << resPercent[j] << "%" << std::endl;
		}

		std::cout << std::endl << "A total of " << district.getVoterPercent() << "% have voted in this district." << std::endl;
		std::cout << "This district is won by the " << election.parties.getName(winner) << " party - lead by "
			<< WinnerName << std::endl << std::endl;

		partyMemberResults(resPercent, districtNum, district.getElectorate(), election); // print party representatives in district
	}

	void ElectionResults::partyMemberResults(vector<double>& percentArr, int districtNum, int districtElectorate, const ElectionRound& election) const
	{
		int numReps;
		std::cout << "District representatives" << std::endl << "________________________" << std::endl << std::endl;
		for (int i = 0; i < _partySize; i++)
		{
			std::cout << election.parties.getName(i) << ": " << std::endl;
			numReps = (percentArr[i] * districtElectorate) / 100;
			election.parties.printMembersResults(i, districtNum, numReps);
			std::cout << std::endl << "________________________" << std::endl;
		}
	}

	NormalResults::NormalResults(int partySize, int districtSize, const NormalRound& electionRound) : ElectionResults(partySize, districtSize), election(electionRound),
																										winners(vector<int>()), resPercent(vector<vector<double>>())
	{
		winners.resize(districtSize);
		resPercent.resize(districtSize);
		for (int i = 0; i < districtSize; i++)
		{
			resPercent[i].resize(partySize);
		}
	}

	void NormalResults::showResults()
	{
		int sumVote, type;
		std::cout << "Results:" << std::endl << "__________" << std::endl << std::endl;
		for (int i = 0; i < _districtSize; i++)
		{
			type = typeid(election.districts[i]) == typeid(UnifiedDistrict);
			std::cout << "District - " << election.districts.getName(i) << " | Electorates - " << election.districts.getElectorate(i);
			if (type)
				std::cout << " | Type - Unified District";
			else
				std::cout << " | Type - Divided District";
			std::cout << std::endl;

			sumVote = election.districts.getVoterCount(i);
			if (!sumVote) // no votes in the district
				std::cout << "No voting results to show in " << election.districts.getName(i) << std::endl << std::endl;
			else
			{
				const string& WinnerName = election.districts.getCivPtr(election.parties.getIDCandid(winners[i]))->getName();
				
				showDistrictResults(i, resPercent[i], winners[i], WinnerName, election.districts[i], election);
			}
		}

		Party** results = new Party*[_partySize];
		ElectionRound::sortParties(results, _partySize, election.parties); // sort by no. of electorates 

		std::cout << std::endl << "Final results:  " << std::endl << "_____________" << std::endl << std::endl;
		for (int i = 0; i < _partySize; i++)
		{
			std::cout << "# " << i + 1 << " - " << results[i]->getName() << ", lead by " << election.districts.getCivPtr(results[i]->getIDCandid())->getName()
				<< " with " << results[i]->getElectorates() << " electorates and " << results[i]->getVotes() << " votes" << std::endl << std::endl;
		}

		delete[] results;
	}

	
	SimpleResults::SimpleResults(int partySize, int districtSize, const SimpleRound& electionRound) : ElectionResults(partySize, districtSize), resPercent(vector<double>()), election(electionRound)
	{
		resPercent.resize(partySize);
	}

	void SimpleResults::showResults()
	{
		const string& WinnerName = election.district.findCiv(election.parties.getIDCandid(winner))->getName();
		
		showDistrictResults(0, resPercent, winner, WinnerName, election.district, election);

		Party** results = new Party * [_partySize];
		ElectionRound::sortParties(results, _partySize, election.parties); // sort by no. of electorates 

		std::cout << std::endl << "Final results:  " << std::endl << "_____________" << std::endl << std::endl;
		for (int i = 0; i < _partySize; i++)
		{
			std::cout << "# " << i + 1 << " - " << results[i]->getName() << ", lead by " << election.district.findCiv(election.parties.getIDCandid(i))->getName()
				<< " with " << results[i]->getElectorates() << " electorates and " << results[i]->getVotes() << " votes" << std::endl << std::endl;
		}

		delete[] results;
	}
}