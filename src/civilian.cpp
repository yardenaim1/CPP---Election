#pragma once

#include <civilian.h>
#include <iostream>

namespace Election
{
	Civilian::Civilian() : _name(), _id(0), _yob(0), voted(false), district(nullptr)
	{
	}

	Civilian::Civilian(const string& name, int id, int yob, District *dis) : _name(name), _yob(yob), voted(false), district(dis)
	{
		if (VALID_ID(id))
			_id = id;
		else
			throw "invalid ID number.";
	}

	Civilian::Civilian(const Civilian& other) : Civilian((other.getName()), other.getID(), other.getYear(), other.district)
	{
	}

	Civilian::Civilian(ifstream& inFile)
	{
		load(inFile); 
	}

	void Civilian::save(ofstream& outFile) const
	{
		int len = _name.length();

		outFile.write(rcastcc(&len), sizeof(len));
		outFile.write(rcastcc(_name.c_str()), len);
		outFile.write(rcastcc(&_id), sizeof(_id));
		outFile.write(rcastcc(&_yob), sizeof(_yob));
		outFile.write(rcastcc(&voted), sizeof(voted));

		if (!outFile.good())
			throw "couldn't write to file.";
	}

	void Civilian::load(ifstream& inFile)
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
		inFile.read(rcastc(&_id), sizeof(_id));
		inFile.read(rcastc(&_yob), sizeof(_yob));
		inFile.read(rcastc(&voted), sizeof(voted));
		if (!inFile.good())
		{
			delete[] name;
			throw "Error reading from file.";
		}

		name[len] = '\0';
		_name = string(name, len);
		//_name = string(name);
		delete[] name;
	}

	const Civilian& Civilian::operator=(const Civilian& other) 
	{
		if (this != &other)
		{
			_name = other._name;
			_id = other._id;
			_yob = other._yob;
			voted = other.voted;
			district = other.district;

		}
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, const Civilian& other)
	{
		os << other.getName() << ", ID: " << other.getID() << ", born on: " << other.getYear() << std::endl;
		return os;
	}


	CivilianList::CivilianList(const CivilianList& other) : lst(other.lst)
	{
	}

	CivilianList::~CivilianList()
	{
		clearList();
	}

	const CivilianList& CivilianList::operator=(const CivilianList& other)
	{
		if (this != &other)
		{
			if (!lst.empty())
				clearList();

			lst = other.lst;
		}
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, const CivilianList& other)
	{
		if (other.lst.empty())
		{
			os << "No civilians." << std::endl;
		}
		else
		{
			Civilian* tempCiv;
			for (auto itr = other.lst.begin(); itr != other.lst.end(); ++itr)
			{
				tempCiv = *itr;
				os << *tempCiv;
			}
			
		}
		return os;
	}

	void CivilianList::clearList()
	{
		lst.erase(lst.begin(), lst.end());
	}


	void CivilianList::addToList(const Civilian& other)
	{
		try { lst.push_back(new Civilian(other)); }
		catch (bad_alloc& ex) { throw ex; }
	}

	const Civilian* CivilianList::findCiv(int id) const
	{
		Civilian* tempCiv = nullptr;
		bool found = false;
		for (auto itr = lst.begin(); itr != lst.end()  && !found; ++itr)
		{
			tempCiv = *itr;
			if (tempCiv->getID() == id)
				found = true;
		}

		if (found)
			return tempCiv;
		else
			return nullptr;
	}

	void CivilianList::save(ofstream& outFile) const
	{
		int size = lst.size();
		outFile.write(rcastcc(&size), sizeof(size));
		if (!outFile.good())
			throw "couldn't write to file.";

		Civilian* tempCiv;
		for (auto itr = lst.begin(); itr != lst.end() ; ++itr)
		{
			tempCiv = *itr;
			tempCiv->save(outFile);
		}
	}

	void CivilianList::load(ifstream& inFile, District* dis)
	{
		int count;
		inFile.read(rcastc(&count), sizeof(count));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!lst.empty())
			clearList();

		Civilian civ;
		for (int i = 0; i < count; i++)
		{
			civ.load(inFile);
			civ.district = dis;
			addToList(civ);
		}
	}

	CivilianArray::CivilianArray(const CivilianArray& other) : arr(other.arr)
	{
	}

	CivilianArray::CivilianArray(int size) : arr(vector<Civilian>(size))
	{
	}

	const CivilianArray& CivilianArray::operator=(const CivilianArray& other)
	{
		if (&other != this)
		{
			if (!arr.empty())
				arr.clear();

			arr = other.arr;
		}
		return *this;
	}

	std::ostream& operator<<(std::ostream& os, const CivilianArray& other)
	{
		if (other.arr.empty())
			os << "No members in this district. " << std::endl;
		else
		{
			for (auto& itr : other.arr)
			{
				os << itr;
			}
		}
		return os;
	}

	void CivilianArray::add(const Civilian& civ)
	{
		arr.push_back(civ);
	}

	void CivilianArray::save(ofstream& outFile) const
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

	void CivilianArray::load(ifstream& inFile)
	{
		int size;
		inFile.read(rcastc(&size), sizeof(size));
		if (!inFile.good())
			throw "Error reading from file.";

		if (!arr.empty())
			arr.clear();

		for (int i = 0; i < size; i++)
		{
			arr.push_back(Civilian(inFile));
		}
	}
}