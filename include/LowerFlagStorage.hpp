/*!
@file
@brief Синглтон с текущим состоянием системы
@author V-Nezlo (vlladimirka@gmail.com)
@date 12.9.2024
@version 1.0
*/

#ifndef INCLUDE_LOWERFLAGSTORAGE_HPP_
#define INCLUDE_LOWERFLAGSTORAGE_HPP_

class FlagStorage {
public:
	static FlagStorage &instance()
	{
		static FlagStorage impl;
		return impl;
	}

	bool pumpState;

private:
	FlagStorage():
		pumpState{false}
	{

	}
};

#endif // INCLUDE_LOWERFLAGSTORAGE_HPP_