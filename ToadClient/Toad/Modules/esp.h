#pragma once

namespace toadll
{

class CEsp SET_MODULE_CLASS(CEsp)
{
private:
	typedef std::map<const char*, vec2> name_pos_t;
	typedef std::map<const char*, vec4> name_bbox_t;

private:
	name_pos_t player_namepos_map;
	name_bbox_t player_bboxpos_map;

public:
	void Update(const std::shared_ptr<c_Entity>& lPlayer, float partialTick) override;
public:
	_NODISCARD const name_pos_t& get_playernames_map() const;
	_NODISCARD const name_bbox_t& get_bounding_box_map() const;
};

}