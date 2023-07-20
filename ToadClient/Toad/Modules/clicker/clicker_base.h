#pragma once

namespace toadll
{

///
/// Base class for clicker modules
///
///	@see CLeftAutoClicker
///	@see CRightAutoClicker
///
class CClickerBase
{
protected:
	/// these are used for calculating the delay compensation
	///
	///	@see m_delay_compensation
	std::chrono::high_resolution_clock::time_point m_start, m_end;

	/// Minecraft PartialTick, that should get updated in derived class
	float m_pTick = 0;

	/// Small delay that holds the time it took for calculating and updating the rand variables.
	///
	/// Gets subtracted from the end value of the rand delay
	float m_delay_compensation = 0;

protected:
	/// Returns the randomization struct defined inside the derived class
	inline virtual Randomization& get_rand() = 0;

	/// Returns whether a mouse button has been send (for preventing double clicking)
	inline virtual bool mouse_down() = 0;
	inline virtual void mouse_up() = 0;

protected:

	/// Updates randomization counters and flags.
	///
	/// The randomization is based of cps as counters are incremented per click.
	void update_rand_vars();

	/// Applies the current randomization variables to the rand delays.
	///
	/// Should be called before using sleep and after setting the default value of delay
	///
	///	@param inconsistencies Resets the inconsistency after applied to rand
	///
	void apply_rand(std::vector<Inconsistency>& inconsistencies);
};

}

