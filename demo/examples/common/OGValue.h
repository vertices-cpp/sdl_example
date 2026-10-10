#ifndef  _VALUE_H_
#define _VALUE_H_

#include "OGHeader.h"
#include "OGPlatformMacros.h"
#include "ogMacros.h"


OG_BEGIN


class Value;

typedef std::vector<Value> ValueVector;
typedef std::unordered_map<std::string, Value> ValueMap;
typedef std::unordered_map<int, Value> ValueMapIntKey;

/*OG_DLL */extern const ValueVector ValueVectorNull;
/*OG_DLL*/ extern const ValueMap ValueMapNull;
/*OG_DLL */extern const ValueMapIntKey ValueMapIntKeyNull;

/*
 * This class is provide as a wrapper of basic types, such as int and bool.
 */
class  Value
{
public:
	/** A predefined Value that has not value. */
	static const Value Null;

	/** Default constructor. */
	Value();

	/** Create a Value by an unsigned char value. */
	explicit Value(unsigned char v);

	/** Create a Value by an integer value. */
	explicit Value(int v);

	/** Create a Value by an unsigned value. */
	explicit Value(unsigned int v);

	/** Create a Value by a float value. */
	explicit Value(float v);

	/** Create a Value by a double value. */
	explicit Value(double v);

	/** Create a Value by a bool value. */
	explicit Value(bool v);

	/** Create a Value by a char pointer. It will copy the chars internally. */
	explicit Value(const char* v);

	/** Create a Value by a string. */
	explicit Value(const std::string& v);

	/** Create a Value by a ValueVector object. */
	explicit Value(const ValueVector& v);
	/** Create a Value by a ValueVector object. It will use std::move internally. */
	explicit Value(ValueVector&& v);

	/** Create a Value by a ValueMap object. */
	explicit Value(const ValueMap& v);
	/** Create a Value by a ValueMap object. It will use std::move internally. */
	explicit Value(ValueMap&& v);

	/** Create a Value by a ValueMapIntKey object. */
	explicit Value(const ValueMapIntKey& v);
	/** Create a Value by a ValueMapIntKey object. It will use std::move internally. */
	explicit Value(ValueMapIntKey&& v);

	/** Create a Value by another Value object. */
	Value(const Value& other);
	/** Create a Value by a Value object. It will use std::move internally. */
	Value(Value&& other);

	/** Destructor. */
	~Value();

	/** Assignment operator, assign from Value to Value. */
	Value& operator= (const Value& other);
	/** Assignment operator, assign from Value to Value. It will use std::move internally. */
	Value& operator= (Value&& other);

	/** Assignment operator, assign from unsigned char to Value. */
	Value& operator= (unsigned char v);
	/** Assignment operator, assign from integer to Value. */
	Value& operator= (int v);
	/** Assignment operator, assign from integer to Value. */
	Value& operator= (unsigned int v);
	/** Assignment operator, assign from float to Value. */
	Value& operator= (float v);
	/** Assignment operator, assign from double to Value. */
	Value& operator= (double v);
	/** Assignment operator, assign from bool to Value. */
	Value& operator= (bool v);
	/** Assignment operator, assign from char* to Value. */
	Value& operator= (const char* v);
	/** Assignment operator, assign from string to Value. */
	Value& operator= (const std::string& v);

	/** Assignment operator, assign from ValueVector to Value. */
	Value& operator= (const ValueVector& v);
	/** Assignment operator, assign from ValueVector to Value. */
	Value& operator= (ValueVector&& v);

	/** Assignment operator, assign from ValueMap to Value. */
	Value& operator= (const ValueMap& v);
	/** Assignment operator, assign from ValueMap to Value. It will use std::move internally. */
	Value& operator= (ValueMap&& v);

	/** Assignment operator, assign from ValueMapIntKey to Value. */
	Value& operator= (const ValueMapIntKey& v);
	/** Assignment operator, assign from ValueMapIntKey to Value. It will use std::move internally. */
	Value& operator= (ValueMapIntKey&& v);

	/** != operator overloading */
	bool operator!= (const Value& v);
	/** != operator overloading */
	bool operator!= (const Value& v) const;
	/** == operator overloading */
	bool operator== (const Value& v);
	/** == operator overloading */
	bool operator== (const Value& v) const;

	 
	unsigned char asByte() const;
 
	int asInt() const;
 
	unsigned int asUnsignedInt() const;
 
	float asFloat() const;
 
	double asDouble() const;
 
	bool asBool() const;
 
	std::string asString() const;

 
	ValueVector& asValueVector();
 
	const ValueVector& asValueVector() const;

 
	ValueMap& asValueMap();
 
	const ValueMap& asValueMap() const;

 
	ValueMapIntKey& asIntKeyMap();
 
	const ValueMapIntKey& asIntKeyMap() const;

	/**
	 * Checks if the Value is null.
	 * @return True if the Value is null, false if not.
	 */
	bool isNull() const { return _type == Type::NONE; }

	/** Value type wrapped by Value. */
	enum class Type
	{
		/// no value is wrapped, an empty Value
		NONE = 0,
		/// wrap byte
		BYTE,
		/// wrap integer
		INTEGER,
		/// wrap unsigned
		UNSIGNED,
		/// wrap float
		FLOAT,
		/// wrap double
		DOUBLE,
		/// wrap bool
		BOOLEAN,
		/// wrap string
		STRING,
		/// wrap vector
		VECTOR,
		/// wrap ValueMap
		MAP,
		/// wrap ValueMapIntKey
		INT_KEY_MAP
	};

	/** Gets the value type. */
	Type getType() const { return _type; }

	/** Gets the description of the class. */
	std::string getDescription() const;

private:
	void clear();
	void reset(Type type);

	union
	{
		unsigned char byteVal;
		int intVal;
		unsigned int unsignedVal;
		float floatVal;
		double doubleVal;
		bool boolVal;

		std::string* strVal;
		ValueVector* vectorVal;
		ValueMap* mapVal;
		ValueMapIntKey* intKeyMapVal;
	}_field;

	Type _type;
};

OG_END

#endif