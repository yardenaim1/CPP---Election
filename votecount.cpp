#include "votecount.h"

namespace Election
{
	VoteParty::VoteParty(int size) : votes(vector<int>(size))
	{
	}

	void VoteParty::save(ofstream& outFile) const
	{
		int size = votes.size();
		outFile.write(rcastcc(&size), sizeof(size));

		for (auto& itr : votes)
			outFile.write(rcastcc(&itr), sizeof(itr));

		if (!outFile.good())
			throw "couldn't write to file.";

	}

	void VoteParty::load(ifstream& inFile)
	{
		int size;
		inFile.read(rcastc(&size), sizeof(size));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!votes.empty())
			votes.clear();

		votes.resize(size);

		for (int i = 0; i < size; i++)
		{
			inFile.read(rcastc(&votes[i]), sizeof(int));
		}
	}

	const VoteParty& VoteParty::operator=(const VoteParty& other)
	{
		if (this != &other)
		{
			if (!votes.empty())
				votes.clear();

			votes = other.votes;
		}
		return *this;
	}

	void VoteParty::reSizeParty(int partySize)
	{
		votes.resize(partySize);
	}

	VotesTotal::VotesTotal(int size) : votesArr(vector<VoteParty>(size))
	{
	}

	VotesTotal::VotesTotal(ifstream& inFile)
	{
		load(inFile);
	}

	void VotesTotal::save(ofstream& outFile) const
	{
		int size = votesArr.size();
		outFile.write(rcastcc(&size), sizeof(size));
		if (!outFile.good())
			throw "couldn't write to file.";

		for (int i = 0; i < size; i++)
		{
			votesArr[i].save(outFile);
		}
	}

	void VotesTotal::load(ifstream& inFile)
	{
		int size;
		inFile.read(rcastc(&size), sizeof(size));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!votesArr.empty())
			votesArr.clear();

		votesArr.resize(size);
		for (int i = 0; i < size; i++)
		{
			votesArr[i].load(inFile);
		}
	}

	void VotesTotal::vote(int partyNum, int districtNum)
	{
		votesArr[districtNum].votes[partyNum]++;
	}

	void VotesTotal::reSizeDistrict(int districtSize)
	{
		votesArr.resize(districtSize);
	}

	void VotesTotal::reSizeParty(int partySize)
	{
		for (auto& itr : votesArr)
		{
			itr.reSizeParty(partySize);
		}
	}
}