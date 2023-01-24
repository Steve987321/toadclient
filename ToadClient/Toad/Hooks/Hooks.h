#pragma once
class c_Hooks
{
private:
	bool enabled = false;
	void* optr = nullptr;
	
public:
	bool init();
	void enable();
	void disable();
	void dispose() const;
};

inline std::unique_ptr<c_Hooks> p_Hooks = nullptr;