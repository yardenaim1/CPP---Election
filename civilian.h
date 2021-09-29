#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "templates.h"

using namespace std;
#define rcastcc reinterpret_cast<const char*> 
#define rcastc reinterpret_cast<char*> 
#define VALID_ID(id) (id >= 100000000 && id <= 999999999)

namespace Election
{
	class District;
	class Civilian // represents a single civilian 
	{
		friend class CivilianArray;
		friend class CivilianList;
	private:

		string _name;
		int _id, _yob;
		bool voted;
		District* district;

	public:
		// ctors + dtor
		Civilian();
		Civilian(const string& name, int id, int yob, District *dis);
		Civilian(const Civilian& other);
		Civilian(ifstream& inFile);
		~Civilian() = default;

		// operator overloading
		const Civilian& operator=(const Civilian& other);
		friend std::ostream& operator<<(std::ostream& os, const Civilian& other);

		// getters
		const int getID() const { return _id; }
		const int getYear() const { return _yob; }
		const bool getVoteState() const { return voted; }
		const string& getName() const { return _name; }
		const District* getDistrict() const { return district; }

		// setter
		bool setVoted() { voted = true; return true; }

		// file methods
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};

	class CivilianList // the list of civilians in a single district, using a self-implemented linked list template class
	{		
	private:
		List<Civilian*> lst;

	public:
		// ctors + dtor
		CivilianList() = default;
		CivilianList(const CivilianList& other);
		~CivilianList();

		// operator overloading
		const CivilianList& operator=(const CivilianList& other);
		friend std::ostream& operator<<(std::ostream& os, const CivilianList& other);

		//getter
		const int getCount() const { return lst.size(); }

		// data & update related methods
		void clearList();
		void addToList(const Civilian &other);

		// search method
		const Civilian* findCiv(int id) const;

		// file method
		void save(ofstream& outFile) const;
		void load(ifstream& inFile, District* dis);
	};

	class CivilianArray // a civilian array that reflects the party representatives in a single district implemented with STL vector
	{
	private:
		vector<Civilian> arr;

	public:

		CivilianArray() = default;
		CivilianArray(const CivilianArray& other);
		CivilianArray(int size);
		~CivilianArray() = default;
		
		// operator overloading
		friend std::ostream& operator<<(std::ostream& os, const CivilianArray& other);
		const CivilianArray& operator=(const CivilianArray& other);

		// setter
		bool setCiv(int idx, const Civilian& other) { arr[idx] = other; return true; }

		// getters
		const int getLogSize() const { return arr.size(); }
		const string& getName(int idx) const { return arr[idx]._name; } 
		const int getID(int idx) const { return arr[idx]._id; } 
		const int getYear(int idx) const { return arr[idx]._yob; }
		const Civilian& getCiv(int idx) const { return arr[idx]; }
		const Civilian* getCivPtr(int idx) const { return &arr[idx]; }
		const District* getDistrict(int idx) const { return arr[idx].district; }

		// adder method
		void add(const Civilian& civ);

		// file method
		void save(ofstream& outFile) const;
		void load(ifstream& inFile);
	};
}