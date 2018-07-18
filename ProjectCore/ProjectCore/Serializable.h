#include "StreamFactory.h"

#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

class Serializable;

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
		return nullptr;
	}

	Serializable * create(const Serializable* obj) const
	{
		return nullptr;
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
	static const StreamFactory<Serializable> * deserialize(instream& is);

	static const StreamFactory<Serializable> * getFactory(uint32_t id);

	inline const uint32_t& getSerialID(void) const {return _serID;}
	static const std::string& getName(uint32_t id);

	static const AutoNullFactory null_factory;

	virtual ~Serializable(void);
protected:
	Serializable(uint32_t id);
private:
	uint32_t _serID;
};

#endif