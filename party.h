#pragma once
#include "civilian.h"

namespace Election
{
	class Party // a single party
	{
		friend class PartyArr;
	private:
		string _name;
		int _idCandid;
		int totalElectorates, totalVotes;
		bool hasMembers;
		vector <CivilianArray> members;
		
	public:
		// ctors + dtor
		Party();
		Party(const string& name, int idCandid);
		Party(const Party& other);
		Party(ifstream& inFile);
		~Party() = default;

		// operator overloading
		const Party& operator=(const Party& other);
		bool operator<(const Party& other);
		friend std::ostream& operator<<(std::ostream& os, const Party& other);

		// setters
		bool setName(const string& name) { _name = name; return true; }
		bool setIDCandid(int id) { _idCandid = id; return true; }
		bool setElectorates(int electorates) { totalElectorates = electorates; return true; }
		bool setVotes(int votes) { totalVotes = votes; return true; }


		// getters
		const string& getName() const { return _name; }
		const int getIDCandid() const { return _idCandid; }
		const int getSize() const { return members.size(); }
		const int getMember_District_Size(int districtNum) const { return members[districtNum].getLogSize(); }
		const bool checkMembers() const { return hasMembers; }
		const int getElectorates() const { return totalElectorates; }
		const int getVotes() const { return totalVotes; }
		Civilian* getMemberPtr(int districtNum, int idx) const { return const_cast<Civilian*>(members[districtNum].getCivPtr(idx)); }

		// data & update related methods
		void addMember(const Civilian& civ, int districtNum);
		void reSize(int newSize);

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};

	class PartyArr // an array of all of the parties in the elections
	{
	private:
		vector<Party> arr;

	public:
		// ctors + dtor
		PartyArr() : arr(vector<Party>()) {}
		PartyArr(int size);
		PartyArr(ifstream& inFile);
		~PartyArr() = default;

		// operator overloading
		const Party& operator[](int idx) { return arr[idx]; }

		// setters
		 bool setPartyElectorates(int partyNum, int electorates) { arr[partyNum].setElectorates(electorates); return true; }
		 bool setPartyVotes(int partyNum, int votes) { arr[partyNum].setVotes(votes); return true; }

		// getters
		 const string& getName(int partyNum) const { return arr[partyNum]._name; }
		const int getIDCandid(int partyNum) const { return arr[partyNum]._idCandid; }
		const int getLogSize() const { return arr.size(); }
		const int getPhySize() const { return arr.capacity(); }
		const int getMembersSize(int partyNum) const { return arr[partyNum].members.size(); }
		const int getElectorates(int partyNum) const { return arr[partyNum].totalElectorates; }
		const int getVotes(int partyNum) const { return arr[partyNum].totalVotes; }
		const Party* getPartyRef(int partyNum) const { return &arr[partyNum]; }

		// data & update related methods
		void addParty(const string& name, int id);
		void addMember(const Civilian& civ, int partyNum, int districtNum);
		void reSizeMembers(int newSize, int partyNum) { arr[partyNum].reSize(newSize); }
		void printMembers(int partyNum) const;
		void printMembersResults(int partyNum, int districtNum, int numWon) const;

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};	
}