#pragma once
namespace toadll
{

///
/// ESP on blocks (x-ray)
///
///	TODO: It is very slow
///
class CBlockEsp SET_MODULE_CLASS(CBlockEsp)
{
public:
	CBlockEsp();

public:
	void PreUpdate() override;

	// update the positions of the blocks
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

	// render the positions of the blocks
	void OnRender() override;

private:
	// list of blocks that gets rendered in the esp
	static inline std::vector<std::pair<BBox, Vec4>> m_blocks = {};

	// filter of block ids
	static inline std::unordered_map<int, Vec4> m_blockIdCol = {};

	// esp scanning range from player in blocks
	constexpr static int m_range = 20;
};

}
