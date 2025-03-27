#include "district.h"

namespace Election
{	
	District::District() : _name(), _electorate(0), voters(CivilianList()), voterCount(0) , representatives(vector<Civilian*>())
	{
	}

	District::District(const string& name, int electorate) : _name(name), _electorate(electorate), voters(CivilianList()), voterCount(0), representatives(vector<Civilian*>())
	{
	}

	District::District(const District& other) : District(other._name, other._electorate)
	{
		voterCount = other.voterCount;
		voters = other.voters;
		representatives = other.representatives;
	}

	District::District(ifstream& inFile)
	{
		load(inFile);
	}

	const District& District::operator=(const District& other)
	{
		if (this != &other)
		{
			_name = other._name;
			_electorate = other.getElectorate();
			voterCount = other.voterCount;
			voters = other.voters;
			for (int i = 0; i < _electorate; i++)
			{
				representatives[i] = other.representatives[i];
			}
		}
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, const District& other)
	{
		os << other._name << " with " << other._electorate << " electorates. Type - " << other.getType() << std::endl;
		return os;
	}

	const Civilian* District::findCiv(int id) const
	{
		return voters.findCiv(id);
	}

	void District::addCiv(const string& name, int id, int year)
	{
		Civilian civ(name, id, year,this);
		voters.addToList(civ);
	}

	void District::printCivilianList() const
	{
		std::cout << voters << std::endl;
	}

	void District::updateRepresentatives(int from, int to, int districtNum, const Party& party)
	{
		int i = 0;
		for (int i = 0; i < to; i++)
		{
			representatives[from + i] = party.getMemberPtr(districtNum, i);
		}
	}
	void District::save(ofstream& outFile) const
	{
		int len = _name.length();
		DistrictGenerator::DistrictType type = DistrictGenerator::getType(this);
		outFile.write(rcastcc(&type), sizeof(type));
		outFile.write(rcastcc(&len), sizeof(len));
		outFile.write(rcastcc(_name.c_str()), len);
		outFile.write(rcastcc(&_electorate), sizeof(_electorate));
		outFile.write(rcastcc(&voterCount), sizeof(voterCount));
		if (!outFile.good())
			throw "couldn't write to file.";

		voters.save(outFile);
	}

	void District::load(ifstream& inFile)
	{
		int len;
		inFile.read(rcastc(&len), sizeof(len));
		if (!inFile.good())
			throw "Error reading from file.";

		//_name.resize(len + 1);
		char* name;
		try { name = new char[len + 1]; }
		catch (bad_alloc& ex) { throw ex; }
		inFile.read(rcastc(name), len);
		inFile.read(rcastc(&_electorate), sizeof(_electorate));
		inFile.read(rcastc(&voterCount), sizeof(voterCount));
		if (!inFile.good())
		{
			delete[] name;
			throw "Error reading from file.";
		}

		name[len] = '\0';
		_name = string(name, len);
		//_name = string(name);
		delete[] name;

		setRepresentative();
		voters.load(inFile, this);
	}

	DividedDistrict::DividedDistrict(const DividedDistrict& other) : District(other)
	{
		_divided = 1;
	}

	const DividedDistrict& DividedDistrict::operator=(const DividedDistrict& other)
	{
		if (this != &other)
		{
			District::operator=(other);
			_divided = 1;
		}
		return *this;
	}

	const char* DividedDistrict::getType() const
	{
		return "Unified District";
	}

	UnifiedDistrict::UnifiedDistrict(const UnifiedDistrict& other) : District(other)
	{
		_divided = 0;
	}

	const UnifiedDistrict& UnifiedDistrict::operator=(const UnifiedDistrict& other)
	{
		if (this != &other)
		{
			District::operator=(other);
			_divided = 0;
		}
		return *this;

	}

	const char* UnifiedDistrict::getType() const
	{
		return "Unified District";
	}

	void DistrictArr::updateRepresentatives(int from, int to, int districtNum, const Party& party)
	{
		if (party.getMember_District_Size(districtNum) < to - from)
			throw "there aren't enough district representatives.";

		arr[districtNum]->updateRepresentatives(from, to, districtNum, party);
	}


	DistrictArr::DistrictArr(int size) : arr(vector<District*>())
	{
		arr.reserve(size);
	}

	DistrictArr::DistrictArr(ifstream& in)
	{
		try {  load(in); }
		catch (const char* msg) { throw msg; }
	}

	DistrictArr::~DistrictArr()
	{
		for (auto& itr : arr)
		{
			delete itr;
		}
	}

	void DistrictArr::save(ofstream& outFile) const
	{
		int size= arr.size();
		outFile.write(rcastcc(&size), sizeof(size));
		if (!outFile.good())
			throw "couldn't write to file.";

		for (auto& itr : arr)
		{
			itr->save(outFile);
		}
	}

	void DistrictArr::load(ifstream& inFile)
	{
		int size;
		inFile.read(rcastc(&size), sizeof(size));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!arr.empty())
			arr.clear();

		for (int i = 0; i < size; i++)
		{
			arr.push_back(DistrictGenerator::loadDistrict(inFile));
		}
	}
	const District& DistrictArr::operator[](int idx)
	{
		return *arr[idx];
	}

	const District& DistrictArr::operator[](int idx) const
	{
		return *arr[idx]; 
	}

	void DistrictArr::addDistrict(const string& name, int electorate, int type)
	{
		District* currDistrict = nullptr ; // use of polymorphism for the different districts
		try 
		{
			if (type == 1)
			{
				currDistrict = new DividedDistrict(name, electorate);
			}
			else
			{
				currDistrict = new UnifiedDistrict(name, electorate);
			}
		}
		catch (bad_alloc& ex) { throw ex; }
		arr.push_back(currDistrict);
	}

	void DistrictArr::addCivilian(const string& name, int id, int year, int districtNum)
	{
		Civilian civ(name, id, year, arr[districtNum]);
		arr[districtNum]->voters.addToList(civ);
	}

	void DistrictArr::printCivs() const
	{
		for (auto& itr : arr)
		{
			std::cout << itr->_name << ": " << std::endl;
			std::cout << itr->voters << std::endl;
		}
	}

	const Civilian* DistrictArr::getCivPtr(int id) const
	{
		int blank;
		const Civilian* res = getCivPtrHelper(id, &blank);
		return res;
	}

	const Civilian* DistrictArr::getCivPtr(int id, int* districtNum) const
	{
		const Civilian* res = getCivPtrHelper(id, districtNum);
		return res;
	}
	
	const Civilian* DistrictArr::getCivPtrHelper(int id, int *districtNum) const
	{
		bool found = false;
		Civilian* res = nullptr;
		for (int i = 0; i < arr.size() && !found; i++)
		{
			res = const_cast<Civilian*>(arr[i]->findCiv(id));
			if (res)
			{
				found = true;
				*districtNum = i;
			}
		}
		if (!found)
			*districtNum = -1;

		return res;
	}

	std::ostream& operator<<(std::ostream& os, const DistrictArr& other)
	{
		if (other.arr.empty())
		{
			os << "No districts." << std::endl;
		}
		else
		{
			os << "Districts: " << std::endl;
			for (int i = 0; i < other.arr.size(); i++)
			{
				os << i << ". " << *other.arr[i];
			}
		}

		return os;
	}
}