// This file is part of nbind, copyright (C) 2014-2015 BusFaster Ltd.
// Released under the MIT license, see LICENSE.

// This file handles value objects, which are represented by equivalent C++ and
// JavaScript classes, with toJS and fromJS methods calling each others'
// constructors to marshal the class between languages and providing a similar
// API in both.

#pragma once

namespace nbind {

extern "C" {
	extern unsigned int _nbind_get_value_object(unsigned int index, ArgStorage &storage);
}

template<> struct BindingType<cbOutput::CreateValue> {

	typedef int Type;

};

/*
TODO:

template <typename ArgType>
inline uint32_t BindingType<ArgType *>::toWireType(ArgType *arg) {
	return(0);
}
*/

template <typename ArgType>
inline int BindingType<ArgType>::toWireType(ArgType arg) {
	cbFunction *jsConstructor = BindClass<ArgType>::getInstance().getValueConstructorJS();

	if(jsConstructor != nullptr) {
		cbOutput construct(*jsConstructor);

		arg.toJS(construct);

		return(construct.getSlot());
	} else {
		// Value type JavaScript class is missing or not registered.
		return(0);
	}
}

class Int64 {};

template <typename ArgType>
static inline double uint64ToWire(ArgType arg) {
	if(arg <= 0x20000000000000ULL) {
		return(static_cast<double>(arg));
	}

	cbFunction *jsConstructor = BindClass<Int64>::getInstance().getValueConstructorJS();

	if(jsConstructor != nullptr) {
		cbOutput construct(*jsConstructor);

		// TODO: convert to double bigger than 0x20000000000000.0
		return(construct(static_cast<uint32_t>(arg >> 32), static_cast<uint32_t>(arg), false));
	} else {
		// Int64 JavaScript class is missing or not registered.
		// TODO: use a constant bigger than 0x20000000000000.0
		return(0);
	}
}

template <typename ArgType>
static inline double int64ToWire(ArgType arg) {
	if(arg >= -0x20000000000000LL && arg <= 0x20000000000000LL) {
		return(static_cast<double>(arg));
	}

	cbFunction *jsConstructor = BindClass<Int64>::getInstance().getValueConstructorJS();

	if(jsConstructor != nullptr) {
		cbOutput construct(*jsConstructor);

		bool sign = arg < 0;
		if(sign) arg = -arg;

		// TODO: convert to double bigger than 0x20000000000000.0
		return(construct(static_cast<uint32_t>(arg >> 32), static_cast<uint32_t>(arg), sign));
	} else {
		// Int64 JavaScript class is missing or not registered.
		// TODO: use a constant bigger than 0x20000000000000.0
		return(0);
	}
}

template <> struct BindingType<unsigned long> {
	typedef unsigned long Type;
	typedef double WireType;
	static inline double toWireType(Type arg) { return(uint64ToWire(arg)); }
};

template <> struct BindingType<unsigned long long> {
	typedef unsigned long long Type;
	typedef double WireType;
	static inline double toWireType(Type arg) { return(uint64ToWire(arg)); }
};

template <> struct BindingType<long> {
	typedef long Type;
	typedef double WireType;
	static inline double toWireType(Type arg) { return(int64ToWire(arg)); }
};

template <> struct BindingType<long long> {
	typedef long long Type;
	typedef double WireType;
	static inline double toWireType(Type arg) { return(int64ToWire(arg)); }
};

template <typename ArgType>
ArgType BindingType<ArgType>::fromWireType(int index) {
	// Argument is an unused dummy value.

	TemplatedArgStorage<ArgType> storage(0);

	_nbind_get_value_object(index, storage);

	return(storage.getBound());
}

} // namespace
