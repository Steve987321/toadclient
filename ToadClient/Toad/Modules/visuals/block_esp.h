#pragma once
namespace toadll
{

class CBlockEsp SET_MODULE_CLASS(CBlockEsp)
{
private:
	static inline void draw3dBox(const BBox & b_box, const Vec4& col)
	{
		glBegin(GL_QUADS); 

		glColor4f(col.x, col.y, col.z, col.w);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.min.z);

		glVertex3f(b_box.max.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.max.z); 
		glVertex3f(b_box.min.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.max.z);

		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.max.z); 
		glVertex3f(b_box.min.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);

		glVertex3f(b_box.max.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.max.z);

		glVertex3f(b_box.min.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.min.z);
		glVertex3f(b_box.max.x, b_box.min.y, b_box.max.z);
		glVertex3f(b_box.min.x, b_box.min.y, b_box.max.z);

		glVertex3f(b_box.min.x, b_box.max.y, b_box.max.z);
		glVertex3f(b_box.max.x, b_box.max.y, b_box.max.z); 
		glVertex3f(b_box.max.x, b_box.max.y, b_box.min.z);
		glVertex3f(b_box.min.x, b_box.max.y, b_box.min.z);

		glEnd();
	}
private:
	// list of blocks that gets rendered in the esp
	static inline std::vector<std::pair<BBox, Vec4>> m_blocks = {};

	// filter of block ids
	static inline std::unordered_map<int, Vec4> m_blockIdCol = {};

	constexpr static int m_range = 20;

public:
	// update the positions of the blocks
	void Update(const std::shared_ptr<LocalPlayer>& lPlayer) override;

	// render the positions of the blocks
	void OnRender() override;
};

}
