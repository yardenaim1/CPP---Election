#pragma once
#include "civilian.h"
#include "party.h"

namespace Election
{
	class District // represents a single district
	{
		friend class DistrictArr;
	private:
		string _name;
		int _electorate;
		int voterCount;
		CivilianList voters;
		vector<Civilian*>representatives;
		
	public:	
		// ctors + dtor
		District();
		District(const string& name, int electorate);
		District(const District& other);
		District(ifstream& inFile);
		virtual ~District() = default;

		// operator overloading
		const District& operator=(const District& other);
		friend std::ostream& operator<<(std::ostream& os, const District& other);

		// setters
		bool setName(const string& name) { _name = name; return true; }
		bool setElectorate(int electorate) { _electorate = electorate; return true; }
		bool setCivList(const CivilianList& lst) { voters = lst; return true; }
		bool IncreaseVoterCount() { voterCount++; return true; }
		bool setRepresentative() { representatives.resize(_electorate); return true; }

		// getters
		const string& getName() const { return _name; }
		const int getElectorate() const { return _electorate; }
		const double getVoterPercent() const { return (static_cast<double>(voterCount) / voters.getCount()) * 100; }
		const int getVoterCount() const { return voterCount; }
		const int getNumCiv() const { return voters.getCount(); }
		virtual const char* getType() const = 0; // virtual method to get type of derived classes 
		
		// data & update related methods
		const Civilian* findCiv(int id) const;
		void addCiv(const string& name, int id, int year);
		void printCivilianList() const;
		void updateRepresentatives(int from, int to, int districtNum, const Party& party);

		// file methods
		virtual void save(ofstream& outFile) const = 0;
		virtual void load(ifstream& inFile);
	};

	class DividedDistrict : public District
	{
	private:
		int _divided = 1;

	public:
		// ctors + dtor
		DividedDistrict() : District() {}
		DividedDistrict(ifstream& in) : District(in) {}
		DividedDistrict(const string& name, int electorate) : District(name, electorate) {}
		DividedDistrict(const DividedDistrict& other);
		~DividedDistrict() = default;

		// operator overloading
		const DividedDistrict& operator=(const DividedDistrict& other);
	
		// getter
		virtual const char* getType() const override;

		//file method
		void save(ofstream& outFile) const { District::save(outFile); }
	};

	class UnifiedDistrict : public District
	{
	private:
		int _divided = 0;

	public:
		// ctors + dtor
		UnifiedDistrict() : District() {}
		UnifiedDistrict(ifstream& in) : District(in) {}
		UnifiedDistrict(const string& name, int electorate) : District(name, electorate) {}
		UnifiedDistrict(const UnifiedDistrict& other);
		~UnifiedDistrict() = default;

		// operator overloading
		const UnifiedDistrict& operator=(const UnifiedDistrict& other);

		// getter
		virtual const char* getType() const override;

		//file method
		void save(ofstream& outFile) const { District::save(outFile); }
	};

	class DistrictArr // this class represents all of the districts in the elections 
	{
	private:
		vector<District*> arr;

	public:
		// ctors + dtor
		DistrictArr(int size = 1);
		DistrictArr(ifstream& in);
		~DistrictArr();

		// operator overloading
		friend std::ostream& operator<<(std::ostream& os, const DistrictArr& other);
		const District& operator[](int idx);
		const District& operator[](int idx) const;

		// getters
		const string& getName(int idx) const { return arr[idx]->getName(); }
		const int    getElectorate(int idx) const {return arr[idx]->getElectorate(); }
		const int    getLogSize() const { return arr.size(); }
		const int    getPhySize() const { return arr.capacity(); }
		const double getVotePercentage(int districtNum) const { return arr[districtNum]->getVoterPercent(); }
		const int    getVoterCount(int districtNum) const { return arr[districtNum]->voterCount; }
		const int    getCivCount(int districtNum) const { return arr[districtNum]->getNumCiv(); }
		bool         IncreaseVoteCount(int districtNum) { arr[districtNum]->IncreaseVoterCount(); return true; }

		// data & update related methods
		void addDistrict(const string& name, int electorate,int type);
		void addCivilian(const string& name, int id, int year, int districtNum);
		void printCivs() const;
		void updateRepresentatives(int from, int to,int districtNum, const Party& party);

		// search methods
		const Civilian* getCivPtr(int id) const;
		const Civilian* getCivPtr(int id, int *districtNum) const;
		const Civilian* getCivPtrHelper(int id, int* districtNum) const;

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};
	
	
	class DistrictGenerator // helper class to load districts from file
	{
	public:
		enum class DistrictType { divided = 0, unified };

		static District* loadDistrict(ifstream& inFile) // returns allocation of matching type of district constructed from input file
		{
			DistrictType type;
			inFile.read(rcastc(&type), sizeof(type));
			if (!inFile.good())
				throw "Error reading from file.";

			try 
			{
				if (type == DistrictType::divided)
					return new DividedDistrict(inFile);
				else 
					return new UnifiedDistrict(inFile);
			}
			catch (bad_alloc& ex) { throw ex; }
		}

		static DistrictType getType(const District* district)
		{
			if (typeid(*district) == typeid(DividedDistrict))
				return DistrictType::divided;
			if (typeid(*district) == typeid(UnifiedDistrict))
				return DistrictType::unified;

			return DistrictType::divided; // Default
		}
	};
}