#pragma once

// constexpr hash map 
template <typename hash, typename T> 
class hash_map
{
public:
	constexpr hash_map(std::initializer_list<std::pair<hash, T>> lst)
	{
		for (uint32_t i = 0; i < lst.size(); i++)
		{

		}
		std::hash();
	}

private:
	uint32_t _capacity;
};

