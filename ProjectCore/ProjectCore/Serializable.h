#include "StreamFactory.h"

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable;

typedef unsigned int SerialID;

template <class T>
class SerialFactory : public StreamFactory<Serializable>
{
public:
	Serializable * create(instream& is, bool full) const
	{
		return new T(is, full);
	}

	Serializable * create(const Serializable* obj) const
	{
		return new T(*dynamic_cast<const T*>(obj));
	}
};

class NullFactory : public StreamFactory<Serializable>
{
public:
	Serializable * create(instream& is, bool full) const
	{
		return 0;
	}

	Serializable * create(const Serializable* obj) const
	{
		return 0;
	}
};

class AutoNullFactory
{
public:
	AutoNullFactory(void);
	~AutoNullFactory(void);

	NullFactory * factory;
};

class Serializable
{
public:
	static void Register(const std::string& name, const size_t& id, StreamFactory<Serializable> * factory);

	virtual void write_to(outstream& os, bool full) const {}
	
	static void serialize(outstream& os, const Serializable * const instance);
	static const StreamFactory<Serializable> * unserialize(instream& is);

	static const StreamFactory<Serializable> * getFactory(const SerialID& id); 

	inline const SerialID& getSerialID(void) const {return _serID;}
	static const std::string& getName(SerialID id);

	static const AutoNullFactory null_factory;

	virtual ~Serializable(void);
protected:
	Serializable(SerialID id);
private:
	SerialID _serID;
};

#endif