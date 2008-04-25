#pragma once
#include "Threading.h"

class Master : public CyTHREAD::CThread
{
public:
	Master(void);
	~Master(void);
	Master(void *arg);

public:
	virtual void Setup();
	virtual void Execute(void *arg);
};
