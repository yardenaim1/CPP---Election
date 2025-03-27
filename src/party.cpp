#include "party.h"

namespace Election
{
	Party::Party() : _name(), _idCandid(0), members(vector<CivilianArray>(1)), hasMembers(false), totalElectorates(0), totalVotes(0)
	{
	}

	Party::Party(const string& name, int idCandid) : _name(name), _idCandid(idCandid), members(vector<CivilianArray>(1)), hasMembers(false), totalElectorates(0), totalVotes(0)
	{
	}

	Party::Party(const Party& other) : Party(other._name,other._idCandid)
	{
		totalElectorates = other.totalElectorates;
		totalVotes = other.totalVotes;
		hasMembers = other.hasMembers;
		members = other.members;
	}

	Party::Party(ifstream& inFile)
	{
		load(inFile);
	}

	const Party& Party::operator=(const Party& other)
	{
		if (this != &other)
		{
			_name = other._name;
			_idCandid = other._idCandid;
			totalElectorates = other.totalElectorates;
			totalVotes = other.totalVotes;
			members = other.members;
			hasMembers = other.hasMembers;
		}

		return *this;
	}

	bool Party::operator<(const Party& other)
	{
		return totalElectorates < other.totalElectorates;
	}

	std::ostream& operator<<(std::ostream& os, const Party& other)
	{
		os << other._name <<" party.";
		if (!other.hasMembers)
		{
			os << " No representatives." << std::endl;
		}
		else
		{
			int size = other.members.size();
			os << std::endl << " Representatives: " << std::endl << std::endl;
			for (int i = 0; i < size; i++)
			{
				os << "District " << i << ":" << std::endl;
				os << other.members[i];
			}
		}

		return os;
	}

	void Party::addMember(const Civilian& civ, int districtNum)
	{
		if (!hasMembers)
			hasMembers = true;
		if (districtNum >= members.size()) // first instance of a new member in this district
			members.push_back(CivilianArray());

		members[districtNum].add(civ);
	}

	void Party::reSize(int newSize)
	{
		if (members.size() < newSize)
			members.resize(newSize);
	}

	void Party::save(ofstream& outFile) const
	{
		int len = _name.length(), size = members.size();
		outFile.write(rcastcc(&len), sizeof(len));
		outFile.write(rcastcc(_name.c_str()), len);
		outFile.write(rcastcc(&_idCandid), sizeof(_idCandid));
		outFile.write(rcastcc(&size), sizeof(size));
		outFile.write(rcastcc(&totalElectorates), sizeof(totalElectorates));
		outFile.write(rcastcc(&totalVotes), sizeof(totalVotes));
		outFile.write(rcastcc(&hasMembers), sizeof(hasMembers));
		if (!outFile.good())
			throw "couldn't write to file.";

		for (auto& itr : members)
		{
			itr.save(outFile);
		}
	}

	void Party::load(ifstream& inFile)
	{
		int len, size;
		inFile.read(rcastc(&len), sizeof(int));
		if (!inFile.good())
			throw "Error reading from file.";

		//_name.resize(len + 1);
		char* name;
		try { name = new char[len + 1]; }
		catch (bad_alloc& ex) { throw ex; }

		inFile.read(rcastc(name), len);
		inFile.read(rcastc(&_idCandid), sizeof(_idCandid));
		inFile.read(rcastc(&size), sizeof(size));
		inFile.read(rcastc(&totalElectorates), sizeof(totalElectorates));
		inFile.read(rcastc(&totalVotes), sizeof(totalVotes));
		inFile.read(rcastc(&hasMembers), sizeof(hasMembers));
		if (!inFile.good())
		{
			delete[] name;
			throw "Error reading from file.";
		}

		name[len] = '\0';
		_name = string(name, len);
		//_name = string(name);
		delete[] name;

		if (size > members.size())
			reSize(size);

		for (auto& itr : members)
		{
			itr.load(inFile);
		}
	}

	PartyArr::PartyArr(int size) : arr(vector<Party>())
	{
		arr.reserve(size);
	}

	PartyArr::PartyArr(ifstream& inFile)
	{
		load(inFile);
	}

	void PartyArr::save(ofstream& outFile) const
	{
		int size = arr.size();
		outFile.write(rcastcc(&size), sizeof(size));
		if (!outFile.good())
			throw "couldn't write to file.";

		for (auto& itr : arr)
		{
			itr.save(outFile);
		}
	}

	void PartyArr::load(ifstream& inFile)
	{
		int size;
		inFile.read(rcastc(&size), sizeof(size));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!arr.empty())
			arr.clear();
		
		for (int i = 0; i < size; i++)
			arr.push_back(Party(inFile));
	}

	void PartyArr::addParty(const string& name, int id)
	{
		arr.push_back(Party(name, id));
	}

	void PartyArr::addMember(const Civilian& civ, int partyNum, int districtNum)
	{
		arr[partyNum].addMember(civ, districtNum);
	} 

	void PartyArr::printMembers(int partyNum) const
	{
		if (arr.empty())
			std::cout << "No parties." << std::endl;
		else
			std::cout << arr[partyNum];
	}

	void PartyArr::printMembersResults(int partyNum, int districtNum, int numWon) const
	{
		int limit, logMemberSize = arr[partyNum].members[districtNum].getLogSize();
		if (numWon > logMemberSize)
			limit = logMemberSize;
		else
			limit = numWon;
		if (limit == 0)
			std::cout << "No representatives. " << std::endl;
		else
		{
			for (int i = 0; i < limit; i++)
			{
				std::cout << i + 1 << ". " << arr[partyNum].members[districtNum].getCiv(i);
			}
		}
	}
}