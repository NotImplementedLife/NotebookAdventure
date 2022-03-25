#pragma once

#include "engine.hpp"

class Level : public TextScrollMap
{
private:	
	Sprite* player;
public:
	void init() override;
	
	void on_key_held(int keys) override;
};
