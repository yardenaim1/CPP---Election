#pragma once
#include <fstream>
#include <vector>
using namespace std;
#define rcastcc reinterpret_cast<const char*> 
#define rcastc reinterpret_cast<char*> 

namespace Election
{
	class VoteParty  // this class reflects votes per party (in a single district)
	{
		friend class VotesTotal; 
	private:
		vector<int> votes;

	public:
		// ctors + dtor
		VoteParty(int size = 1);
		~VoteParty() = default;

		const VoteParty& operator=(const VoteParty& other);
		void reSizeParty(int partySize);

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);	
	};

	class VotesTotal // this class reflects all of the votes in the elections
	{
	private:
		vector<VoteParty> votesArr;

	public:
		// ctors + dtor
		VotesTotal(int size = 1);
		VotesTotal(ifstream& inFile);
		~VotesTotal() = default;

		// getters
		const int getNumVotes(int districtNum, int partyNum) const { return votesArr[districtNum].votes[partyNum]; }
		const int getSizeDistrict() const { return votesArr.size(); }
		const int getSizeParty() const { return votesArr[0].votes.size(); }

		// data & update related methods
		void vote(int partyNum, int districtNum);
		void reSizeDistrict(int districtSize);
		void reSizeParty(int partySize);

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};
}