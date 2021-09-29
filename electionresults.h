#pragma once
#include "electionround.h"

namespace Election
{
	class ElectionResults // base for the classes that hold results of elections
	{	
	protected:
		vector<int> resElectorate;
		vector<int> resVotes;
		int _districtSize, _partySize;

	public:
		// ctors + dtor
		ElectionResults(int partySize, int districtSize);
		virtual ~ElectionResults() = default;
		
		// print results methods
		void showDistrictResults(int districtNum, vector<double>& resPercent, int winner, const string& WinnerName, const District& district, const ElectionRound& election) const;
		void partyMemberResults(vector<double>& percentArr, int districtNum, int districtElectorate, const ElectionRound& election) const;
		virtual void showResults() = 0;
	};

	class NormalResults : public ElectionResults // results of a normal round of election
	{
		friend class NormalRound;
	private:
		vector<int> winners;
		vector<vector<double>> resPercent;
		const NormalRound& election;

	public:
		// ctors + dtor
		NormalResults(int partySize, int districtSize, const NormalRound& electionRound);
		~NormalResults() = default;

		// print results methods
		void showResults() override;		
	};

	class SimpleResults : public ElectionResults // results of a simple round of election
	{
		friend class SimpleRound;
	private:
		int winner;
		vector<double> resPercent;
		const SimpleRound& election;

	public:
		// ctors + dtor
		SimpleResults(int partySize, int districtSize, const SimpleRound& electionRound);
		~SimpleResults() = default;

		// print results methods
		void showResults() override;
	};
}

