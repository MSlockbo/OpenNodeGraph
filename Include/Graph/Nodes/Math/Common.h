//
// Created by Maddie on 11/1/2024.
//

#ifndef OSU_MATH_COMMON_H
#define OSU_MATH_COMMON_H

#include <Graph/ShaderGraph.h>
    
namespace OpenShaderDesigner::Nodes::Math
{
	
// Header Colors =======================================================================================================

inline static constexpr ImColor HeaderColor        = ImColor(0xA7, 0x62, 0x53);
inline static constexpr ImColor HeaderHoveredColor = ImColor(0xC5, 0x79, 0x67);
inline static constexpr ImColor HeaderActiveColor  = ImColor(0x82, 0x4C, 0x40);

inline static const std::string HeaderMarker       = "\uF3B9 ";


// Math Op Prototype ---------------------------------------------------------------------------------------------------

struct MathOp : public Node
{
	enum MathOpFlags_ : glw::enum_t
	{
		MathOpFlags_None = 0
	,	MathOpFlags_AllowMultipleInputTypes = 0x0000'0001
	};
	
	MathOp(ShaderGraph& graph, ImVec2 pos);
	~MathOp() override = default;

	virtual bool CheckConnection(const Pin*, const Pin*) const override;
	virtual void ValidateConnections() override;

	struct
	{
		glw::enum_t Flags;
	} Math;
};

}	

#endif // OSU_MATH_COMMON_H
