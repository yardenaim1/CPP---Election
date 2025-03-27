#pragma once
#include "district.h"
#include "party.h"
#include "votecount.h"

constexpr int MAXDAYS[13] = { -1, 31, 28, 31, 30 ,31 ,30 ,31 ,31 ,30 ,31 ,30 ,31 };

namespace Election
{
	class ElectionResults;
	class SimpleResults;
	class NormalResults;

	// an abstract base class that serves as a template for the other two types of election rounds
	// saves and handles all of the data required to handle them, apart from the districts which are hadnled separately in the derived classes
	class ElectionRound 
	{
		friend class ElectionResults;
	protected:
		int _day, _month, _year;
		bool voteStarted;
		PartyArr parties;
		VotesTotal votes;

	public:
		// ctors + dtor
		ElectionRound(int day, int month, int year);
		ElectionRound(ifstream& inFile);
		ElectionRound(const ElectionRound& other) = delete;
		virtual ~ElectionRound() = default;

		// getters
		const int getDay() const { return _day; }
		const int getMonth() const { return _month; }
		const int getYear() const { return _year; }
		virtual const int getDistrictLogSize() const = 0;
		virtual const string& getDistrictName(int districtNum) const = 0;
		const int getPartiesLogSize() const { return parties.getLogSize(); }
		const string& getPartyName(int partyNum)const { return parties.getName(partyNum); }
		virtual const Civilian* getCivPtr(int id) const = 0;
		virtual const Civilian* getCivPtr(int id, int* districtNum) const = 0;
		const bool checkVoteStarted() const { return voteStarted; }
		
		// data & update related methods
		void printParties() const;
		virtual void printDistricts() const = 0;
		virtual void printCivs() const = 0;
		void addParty(const string& name, int idCandid);
		virtual void addCivilian(const string& name, int id, int year, int districtNum) = 0;
		virtual void addDistrict(const string& name, int electorate, int type) = 0;
		virtual void addPartyMember(const Civilian& civ, int partyNum, int districtNum);
		bool nameEqualParty(const string& name) const;
		virtual bool nameEqualDistrict(const string& name) const = 0;
		virtual bool civilianExists(int id) const = 0;

		// vote related methods
		virtual void setVote(int partyNum, Civilian* civ, int districtNum);
		virtual void results(ElectionResults *res) = 0; 
		virtual void resultDistrict(vector<int>& resultsVotes, vector<double>& resPercent, vector<int>& resultsElectorate, int& maxIndex, int districtNum, const District& district);
		virtual void setResults(vector<int>& resVotes, vector<int>& resultsElectorate);
		virtual bool setPartyVotes(int partyNum, int sumVotes) { parties.setPartyVotes(partyNum, sumVotes); return true; }
		virtual bool setPartyElectorates(int partyNum, int electorates) { parties.setPartyElectorates(partyNum, electorates); return true; }

		// static sort method
		static void sortParties(Party** arr, int size, const PartyArr& parties);

		// file method
		virtual void save(ofstream& outFile) const = 0;
	};


	class NormalRound : public ElectionRound // class for normal round of elections with more than a single district
	{
		friend class NormalResults;

	private:
		DistrictArr districts;

	public:
		// ctors + dtor
		NormalRound(int day, int month, int year) : ElectionRound(day, month, year) {}
		NormalRound(ifstream& inFile);

		// getters
		const Civilian* getCivPtr(int id) const { return districts.getCivPtr(id); }
		const Civilian* getCivPtr(int id, int* districtNum) const { return districts.getCivPtr(id, districtNum); }
		const int getDistrictLogSize() const { return districts.getLogSize(); }
		const string& getDistrictName(int districtNum) const { return districts.getName(districtNum); }

		// data & update related methods
		virtual void addDistrict(const string& name, int electorate, int type);
		virtual void addCivilian(const string& name, int id, int year, int districtNum);
		virtual void addPartyMember(const Civilian& civ, int partyNum, int districtNum);
		virtual void printCivs() const;
		virtual void printDistricts() const;
		virtual bool nameEqualDistrict(const string& name) const;
		virtual bool civilianExists(int id) const;

		// vote related methods
		void setVote(int partyNum, Civilian* civ, int districtNum);
		void results(ElectionResults* res);
		void setResults(vector<int>& resVotes, vector<vector<double>>& resPercent, vector<int>& resultsElectorate);

		// file method
		void save(ofstream& outFile) const { ElectionRound::save(outFile); districts.save(outFile); }
	};

	class SimpleRound : public ElectionRound // class for simple round of elections with a single divided district
	{
		friend class SimpleResults;

	private:
		DividedDistrict district;		

	public:
		// ctors + dtor
		SimpleRound(int day, int month, int year, int electorates);
		SimpleRound(ifstream& inFile);

		// getters
		const Civilian* getCivPtr(int id) const { return district.findCiv(id);}
		const Civilian* getCivPtr(int id, int* districtNum) const { *districtNum = 0; return getCivPtr(id); }
		const string& getDistrictName(int districtNum) const { return district.getName(); }
		const int getDistrictLogSize() const { return 1; }

		// data & update related methods
		virtual void addCivilian(const string& name, int id, int year, int districtNum = 0);
		virtual void addDistrict(const string& name, int electorate, int type) { return; } // empty implementation to match base class signature
		virtual void printDistricts() const;
		virtual void printCivs() const;
		virtual bool nameEqualDistrict(const string& name) const { return district.getName() == name; }
		virtual bool civilianExists(int id) const;

		// vote related methods
		void setVote(int partyNum, Civilian* civ, int districtNum = 0);
		void results(ElectionResults* res);
		void setResults(vector<int>& resVotes, vector<double>& resPercent, vector<int>& resultsElectorate);

		// file method
		void save(ofstream& outFile) const { ElectionRound::save(outFile); district.save(outFile); }
	};


	class ElectionGenerator // helper class to load elections from file
	{
	public:
		enum class ElectionType {normal = 0 , simple };

		static ElectionRound* loadElection(ifstream& inFile) // returns allocation of matching type of election round constructed from input file
		{
			ElectionType type;
			inFile.read(rcastc(&type), sizeof(ElectionType));
			if (!inFile.good())
				throw "Error reading from file.";
			try
			{
				if (type == ElectionType::normal)
					return new NormalRound(inFile);
				else
					return new SimpleRound(inFile);
			}
			catch (bad_alloc& ex)
			{
				throw ex;
			}
			catch (const char* msg)
			{
				throw msg;
			}
		}

		static ElectionType getType(const ElectionRound* election) // returns type of elections round
		{
			if (typeid(*election) == typeid(NormalRound))
				return ElectionType::normal;
			if (typeid(*election) == typeid(SimpleRound))
				return ElectionType::simple;
				
			return ElectionType::normal; // Default
		}
	};
}