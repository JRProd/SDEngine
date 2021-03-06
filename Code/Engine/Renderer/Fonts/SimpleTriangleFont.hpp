#pragma once
#include <string>
#include <vector>
#include "Engine/Core/VertexTypes/VertexMaster.hpp"


//------------------------------------------------------------------------------------------------
class RenderContext;
struct Vertex_PCU;
struct Rgba8;
struct Vec2;


//------------------------------------------------------------------------------------------------
void DrawTextTriangles2D( RenderContext& renderer, const std::string& text, const Vec2& startMins, float cellHeight, const Rgba8& color, float cellAspect = 0.56f, float spacingFraction = 0.2f );
void AppendTextTriangles2D( std::vector<VertexMaster>& verts, const std::string& text, const Vec2& startMins, float cellHeight, const Rgba8& color, float cellAspect = 0.56f, float spacingFraction = 0.2f );
