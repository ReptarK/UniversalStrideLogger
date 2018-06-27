#pragma once
#include "Singleton.h"

class Chams
	: public Singleton<Chams>
{
	friend class Singleton<Chams>;

	Chams() {}
	~Chams() {}

public:
	void Initialize();
};

