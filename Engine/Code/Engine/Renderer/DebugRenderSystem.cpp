#include "Engine/Renderer/DebugRenderSystem.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/EventSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/EngineCommon.hpp"

#include <vector>
#include <algorithm>
#include <mutex>


struct DebugEntityConfig
{
	float			m_duration		= -1.f;						// -1.f means infinite duration
	Rgba8			m_startColor	= Rgba8::WHITE;
	Rgba8			m_endColor		= Rgba8::WHITE;
	DebugRenderMode	m_mode			= DebugRenderMode::USE_DEPTH;
	bool			m_isWireFrame	= false;

	bool			m_isText		= false;
	std::string		m_text			= "";
	BillboardType	m_billboardType = BillboardType::NONE;

	Vec2 			m_textMins		= Vec2::ZERO;
	float			m_fontSize		= 1.f;
	Vec2			m_alignment		= Vec2(0.f, 0.f);
	bool			m_isMessage		= false;

	Renderer*		m_renderer		= nullptr;
	BitmapFont*		m_bitmapFont	= nullptr;
};


struct DebugEntity
{
	std::vector<Vertex_PCU> m_verts;
	Stopwatch* m_stopwatch = nullptr;

	Vec3 m_position = Vec3(0.f, 0.f, 0.f);
	Rgba8 m_color = Rgba8::WHITE;
	DebugEntityConfig m_config;


	DebugEntity(DebugEntityConfig const& config) :
		m_config(config)
	{
		m_stopwatch = new Stopwatch(m_config.m_duration);
		m_color = m_config.m_startColor;
	}

	virtual ~DebugEntity()
	{
		if (m_stopwatch != nullptr)
		{
			delete m_stopwatch;
		}

		m_verts.clear();
	}

	void StartStopwatch()
	{
		if (m_config.m_duration != -1.f)
		{
			if (m_stopwatch->IsStopped())
			{
				m_stopwatch->Start();
			}
		}
	}


	void LerpColor()
	{
		float duration = m_config.m_duration;
		if (duration != -1.f && !m_stopwatch->IsStopped())
		{
			float elapsedTimeFraction = m_stopwatch->GetElapsedFraction();
			Rgba8 startColor = m_config.m_startColor;
			Rgba8 endColor = m_config.m_endColor;
			m_color = Rgba8::GetLerpColor(elapsedTimeFraction, startColor, endColor);
		}
	}

	virtual void Render(Camera const& camera) = 0;
};

struct DebugWorldEntity : DebugEntity
{
	DebugWorldEntity(DebugEntityConfig const& config) :
		DebugEntity(config)
	{
	}

	virtual ~DebugWorldEntity() override
	{
	}

	virtual void Render(Camera const& worldCamera) override
	{
		Renderer* renderer = m_config.m_renderer;
		bool isWireframe = m_config.m_isWireFrame;
		bool isText = m_config.m_isText;
		BillboardType billboardType = m_config.m_billboardType;
		DebugRenderMode mode = m_config.m_mode;
		std::vector<Vertex_PCU> vertsCopy = m_verts;

		LerpColor();

		// Set the default renderer modes
		renderer->SetModelConstants(Mat44(), m_color);
		renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
		renderer->SetDepthMode(DepthMode::ENABLED);
		renderer->SetBlendMode(BlendMode::ALPHA);

		// set rasterizer mode
		if (isWireframe)
		{
			renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
		}
		else // isWireframe == false
		{
			renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
		}

		if (isText)
		{
			renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
		}

		// transformation
		if (billboardType != BillboardType::NONE)
		{
			Mat44		cameraModelMatrix = worldCamera.GetOrientation().GetAsMatrix_XFwd_YLeft_ZUp();
			Mat44 billboardMatrix = GetBillboardMatrix(billboardType, cameraModelMatrix, m_position);
			billboardMatrix.SetTranslation3D(m_position);
			TransformVertexArray3D(vertsCopy, billboardMatrix);
		}

		// texture
		if (isText)
		{
			BitmapFont* font = m_config.m_bitmapFont;
			Texture& fontTexture = font->GetTexture();
			renderer->BindTexture(&fontTexture);
		}
		else // isText == false
		{
			renderer->BindTexture(nullptr);
		}

		// draw
		if (mode == DebugRenderMode::USE_DEPTH)
		{
			renderer->SetBlendMode(BlendMode::ALPHA);
			renderer->SetDepthMode(DepthMode::ENABLED);
			renderer->DrawVertexArray((int)vertsCopy.size(), vertsCopy.data());
		}
		else if (mode == DebugRenderMode::ALWAYS)
		{
			renderer->SetDepthMode(DepthMode::DISABLED);
			renderer->DrawVertexArray((int)vertsCopy.size(), vertsCopy.data());
		}
		else // DebugRenderMode::X_RAY
		{
			// create light color
			Rgba8 lightColor = Rgba8::GetLerpColor(0.2f, m_color, Rgba8::WHITE);
			/*lightColor.r = (unsigned char)GetClamped((lightColor.r + 100.f), 0.f, 255.f);
			lightColor.g = (unsigned char)GetClamped((lightColor.g + 100.f), 0.f, 255.f);
			lightColor.b = (unsigned char)GetClamped((lightColor.b + 100.f), 0.f, 255.f);*/
			lightColor.a = 240;

			// set light color for 1st pass
			std::vector<Vertex_PCU> transparentVerts = vertsCopy;
			for (int index = 0; index < transparentVerts.size(); index++)
			{
				Vertex_PCU& transparentVertex = transparentVerts[index];
				transparentVertex.m_color = lightColor;
			}

			// 1st pass with aplha and depth disabled
			renderer->SetBlendMode(BlendMode::ALPHA);
			renderer->SetDepthMode(DepthMode::DISABLED);
			renderer->DrawVertexArray((int)transparentVerts.size(), transparentVerts.data());

			// 2nd pass with opaque and depth enabled
			renderer->SetBlendMode(BlendMode::OPAQUE);
			renderer->SetDepthMode(DepthMode::ENABLED);
			renderer->DrawVertexArray((int)vertsCopy.size(), vertsCopy.data());
		}
	}
};

struct DebugScreenEntity : DebugEntity
{
	DebugScreenEntity(DebugEntityConfig const& config) : 
		DebugEntity(config)
	{
	}

	virtual ~DebugScreenEntity() override
	{
	}

	virtual void Render(Camera const& screenCamera) override
	{
		UNUSED(screenCamera);

		Renderer* renderer = m_config.m_renderer;
		BitmapFont* font = m_config.m_bitmapFont;
		Texture& fontTexture = font->GetTexture();
		Vec2 textMins = m_config.m_textMins;
		float cellHeight = m_config.m_fontSize;
		std::string text = m_config.m_text;
		Vec2 alignment = m_config.m_alignment;
		float		cellAspect	= 0.7f;

		// horizontally align text around the pivot point (textMins)
		float horizontalAlignment = alignment.x;
		float textWidth = font->GetTextWidth(cellHeight, text);
		textMins.x = textMins.x - ( horizontalAlignment * textWidth );
		textMins.y = textMins.y - (alignment.y * cellHeight);

		m_verts.clear();
		font->AddVertsForText2D(m_verts, textMins, cellHeight, text, m_color, cellAspect);
		
		renderer->BindTexture(&fontTexture);
		renderer->SetModelConstants(Mat44(), m_color);
		renderer->DrawVertexArray((int)m_verts.size(), m_verts.data());
	}
};



class DebugRenderSystem
{
public:
	DebugRenderSystem(const DebugRenderConfig& config) :
		m_config(config)
	{
		LoadFonts();
	}

	void LoadFonts()
	{
		std::string fontPath = "Data/Fonts/" + m_config.m_fontName;
		m_bitmapFont = m_config.m_renderer->CreateOrGetBitmapFont(fontPath.c_str());
	}

	void SetDisplayState(bool newDisplayState)
	{
		m_isHidden = newDisplayState;
	}

	void ToggleDisplayState()
	{
		m_isHidden = !m_isHidden;
	}

	void Clear()
	{
		//-------------------------------------------------------------------------
		// lock
		m_drawingMutex.lock();

		ClearDebugEntityList(m_worldObjects);
		ClearDebugEntityList(m_screenTextObjects);
		ClearDebugEntityList(m_screenMessageObjects);

		m_drawingMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}

	void AddDebugWorldEntity(DebugEntity* m_debugWorldEntity)
	{
		if (m_debugWorldEntity)
		{
			//-------------------------------------------------------------------------
			// lock
			m_postingMutex.lock();

			DebugEntityConfig& entityConfig = m_debugWorldEntity->m_config;
			entityConfig.m_renderer = m_config.m_renderer;
			entityConfig.m_bitmapFont = m_bitmapFont;

			m_worldObjects.push_back(m_debugWorldEntity);

			m_postingMutex.unlock();
			// unlock
			//-------------------------------------------------------------------------
		}
	}

	void AddDebugScreenEntity(DebugEntity* m_debugScreenEntity)
	{
		if (m_debugScreenEntity)
		{
			//-------------------------------------------------------------------------
			// lock
			m_postingMutex.lock();

			DebugEntityConfig& entityConfig = m_debugScreenEntity->m_config;
			entityConfig.m_renderer = m_config.m_renderer;
			entityConfig.m_bitmapFont = m_bitmapFont;

			bool isMessage = entityConfig.m_isMessage;
			if (isMessage)
			{
				m_screenMessageObjects.push_back(m_debugScreenEntity);
			}
			else // isMessage = false
			{
				m_screenTextObjects.push_back(m_debugScreenEntity);
			}

			m_postingMutex.unlock();
			// unlock
			//-------------------------------------------------------------------------
		}
	}

	void ClearDebugEntityList(std::vector<DebugEntity*>& debugEntityList)
	{
		for (int index = 0; index < debugEntityList.size(); index++)
		{
			DebugEntity*& debugEntityPtr = debugEntityList[index];
			if (debugEntityPtr != nullptr)
			{
				delete debugEntityPtr;
				debugEntityPtr = nullptr;
			}
		}

		debugEntityList.clear();
	}

	void BeginFrame()
	{
		// start the stopwatches if stoped
		StartStopWatch(m_worldObjects);
		StartStopWatch(m_screenTextObjects);
		StartStopWatch(m_screenMessageObjects);
	}

	void StartStopWatch(std::vector<DebugEntity*>& debugEntityList)
	{
		for (int index = 0; index < debugEntityList.size(); index++)
		{
			DebugEntity* debugEntity = debugEntityList[index];

			debugEntity->StartStopwatch();
		}
	}

	void RenderWorld(Camera const& camera)
	{
		if (m_isHidden)
			return;

		//-------------------------------------------------------------------------
		// lock
		m_drawingMutex.lock();

		Renderer* renderer = m_config.m_renderer;
		renderer->BeginCamera(camera);

		for (int index = 0; index < m_worldObjects.size(); index++)
		{
			DebugEntity* worldObject = m_worldObjects[index];
			worldObject->Render(camera);
		}

		renderer->EndCamera(camera);

		m_drawingMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}

	void RenderScreen(Camera const& camera) 
	{
		if (m_isHidden)
			return;

		//-------------------------------------------------------------------------
		// lock
		m_drawingMutex.lock();

		Renderer* renderer = m_config.m_renderer;
		renderer->BeginCamera(camera);

		AABB2 cameraBounds(camera.GetOrthographicBottomLeft(), camera.GetOrthographicTopRight());

		// render debug screen text
		for (int index = 0; index < m_screenTextObjects.size(); index++)
		{
			DebugEntity* entity = m_screenTextObjects[index];
			entity->Render(camera);
		}

		// sort debug messages by the duration
		std::sort(m_screenMessageObjects.begin(), m_screenMessageObjects.end(),
			[] (DebugEntity* entityA, DebugEntity* entityB)
			{
				float entityADuration = entityA->m_config.m_duration;
				float entityBDuration = entityB->m_config.m_duration;

				return entityADuration < entityBDuration;
			}
		);

		// render debug messages line by line
		float screenHeightReservedForScreenText = 15.f;
		Vec2 lineMins = Vec2(cameraBounds.m_mins.x, cameraBounds.m_maxs.y - screenHeightReservedForScreenText);
		for (int index = 0; index < m_screenMessageObjects.size(); index++)
		{
			DebugEntity* screenMessage = m_screenMessageObjects[index];

			// calculate line mins
			float lineHeight = screenMessage->m_config.m_fontSize; // assuming font size is cell height
			lineMins.y = lineMins.y - lineHeight; 

			// render the text on screen
			screenMessage->m_config.m_textMins = lineMins;
			screenMessage->Render(camera);
		}

		renderer->EndCamera(camera);

		m_drawingMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}

	void EndFrame()
	{
		//-------------------------------------------------------------------------
		// lock
		m_postingMutex.lock();

		DeleteTimedOutObjects(m_worldObjects);
		DeleteTimedOutObjects(m_screenTextObjects);
		DeleteTimedOutObjects(m_screenMessageObjects);

		m_postingMutex.unlock();
		// unlock
		//-------------------------------------------------------------------------
	}

	void DeleteTimedOutObjects(std::vector<DebugEntity*>& debugEntityList)
	{
		// delete timed out objects in array
		for (int index = 0; index < debugEntityList.size(); index++)
		{
			DebugEntity* debugObject = debugEntityList[index];
			bool hasDebugDurationElapsed = debugObject->m_stopwatch->HasDurationElapsed();
			float duration = debugObject->m_config.m_duration;
			if (hasDebugDurationElapsed || duration == 0.f)
			{
				delete debugObject;
				debugEntityList[index] = nullptr;
			}
		}

		// remove null ptr reference
		debugEntityList.erase(std::remove(debugEntityList.begin(), debugEntityList.end(), nullptr), debugEntityList.end());
	}


	BitmapFont* GetFont() { return m_bitmapFont; }
	float GetFontAspect() { return m_config.m_fontAspect; }

protected:
	std::vector<DebugEntity*> m_worldObjects;
	std::vector<DebugEntity*> m_screenTextObjects;
	std::vector<DebugEntity*> m_screenMessageObjects;

	BitmapFont* m_bitmapFont = nullptr;
	DebugRenderConfig m_config;
	std::atomic<bool> m_isHidden = false;

private:
	std::mutex m_postingMutex;
	std::mutex m_drawingMutex;

};




// -----------------------------------------------------------
// Global functions -------------------------------------------
// ------------------------------------------------------------

static DebugRenderSystem* s_theDebugRenderer = nullptr;

// Setup -------------------------------------------------------

void DebugRenderSystemStartup(DebugRenderConfig const& config)
{
	if (s_theDebugRenderer == nullptr)
	{
		s_theDebugRenderer = new DebugRenderSystem(config);
		g_theEventSystem->SubscribeToEvent("debugrenderclear", Command_DebugRenderClear);
		g_theEventSystem->SubscribeToEvent("debugrenderToggle", Command_DebugRenderToggle);
	}
}

void DebugRenderSystemShutdown()
{
	if (s_theDebugRenderer != nullptr)
	{
		g_theEventSystem->UnsubscribeFromEvent("debugrenderclear", Command_DebugRenderClear);
		g_theEventSystem->UnsubscribeFromEvent("debugrenderToggle", Command_DebugRenderToggle);

		delete s_theDebugRenderer;
		s_theDebugRenderer = nullptr;
	}
}


// Control -------------------------------------------------------

void DebugRenderSetVisible()
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->SetDisplayState(true);
	}
}

void DebugRenderSetHidden()
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->SetDisplayState(false);
	}
}

void DebugRenderClear()
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->Clear();
	}
}


// Output ----------------------------------------------------------

void DebugRenderBeginFrame()
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->BeginFrame();
	}
}

void DebugRenderWorld(Camera const& camera)
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->RenderWorld(camera);
	}
}

void DebugRenderScreen(Camera const& camera)
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->RenderScreen(camera);
	}
}

void DebugRenderEndFrame()
{
	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->EndFrame();
	}
}


// ---------------------------------------------------------------------
// Geometry
// ---------------------------------------------------------------------


void DebugAddWorldPoint(Vec3 const& pos, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* xyPlanePoint3D = new DebugWorldEntity(entityConfig);
		Vec3 xyPlanePoint = pos;
		xyPlanePoint.z = 0.f;
		 
		// add verts in world space
		std::vector<Vertex_PCU>& pointVerts = xyPlanePoint3D->m_verts;
		AddVertsForSphere3D(pointVerts, xyPlanePoint, radius);
		
		s_theDebugRenderer->AddDebugWorldEntity(xyPlanePoint3D);
	}
}

void DebugAddWorldLine(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* line3D = new DebugWorldEntity(entityConfig);
		
		// add verts in world space
		std::vector<Vertex_PCU>& line3DVerts = line3D->m_verts;
		AddVertsForCylinder3D(line3DVerts, start, end, radius, entityConfig.m_startColor);
		
		s_theDebugRenderer->AddDebugWorldEntity(line3D);
	}
}

void DebugAddWorldAABB2XYZ(AABB2 const& boundsXY, float zHeight, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* aabb2XYZ = new DebugWorldEntity(entityConfig);

		// add verts in world space
		std::vector<Vertex_PCU>& verts = aabb2XYZ->m_verts;
		AddVertsForAABB2(verts, boundsXY, zHeight, startColor);

		s_theDebugRenderer->AddDebugWorldEntity(aabb2XYZ);
	}
}

void DebugAddWorldWireFrameAABB2XYZ(AABB2 const& boundsXY, float zHeight, float wireframeRadius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* aabb2XYZ = new DebugWorldEntity(entityConfig);

		// add verts in world space
		std::vector<Vertex_PCU>& verts = aabb2XYZ->m_verts;
		AddVertsForWireframeAABB2D(verts, boundsXY, zHeight, wireframeRadius, startColor);
		//AddVertsForCylinder3D(line3DVerts, start, end, radius, entityConfig.m_startColor);

		s_theDebugRenderer->AddDebugWorldEntity(aabb2XYZ);
	}
}

void DebugAddWorldWireCylinder(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = true;
		DebugEntity* wireCylinder3D = new DebugWorldEntity(entityConfig);
		 
		// add verts in world space
		std::vector<Vertex_PCU>& wireCylinder3DVerts = wireCylinder3D->m_verts;
		AddVertsForCylinder3D(wireCylinder3DVerts, start, end, radius);
		 
		s_theDebugRenderer->AddDebugWorldEntity(wireCylinder3D);
	}
}

void DebugAddWorldWireSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = true;
		DebugEntity* wireSphere3D = new DebugWorldEntity(entityConfig);
		 
		// add verts in world space
		std::vector<Vertex_PCU>& wireSphere3DVerts = wireSphere3D->m_verts;
		AddVertsForSphere3D(wireSphere3DVerts, center, radius);
		
		s_theDebugRenderer->AddDebugWorldEntity(wireSphere3D);
	}
}

void DebugAddWorldSphere(Vec3 const& center, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if ( s_theDebugRenderer )
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* sphere3D = new DebugWorldEntity(entityConfig);

		// add verts in world space
		std::vector<Vertex_PCU>& sphere3DVerts = sphere3D->m_verts;
		AddVertsForSphere3D(sphere3DVerts, center, radius);

		s_theDebugRenderer->AddDebugWorldEntity(sphere3D);
	}
}

void DebugAddWorldArrow(Vec3 const& start, Vec3 const& end, float radius, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		DebugEntity* arrow3D = new DebugWorldEntity(entityConfig);
		 
		// add verts in world space
		int numSlices = 16;
		Vec3 displacement = end - start;
		Vec3 direction = displacement.GetNormalized();
		float length = displacement.GetLength();
		float cylinderLength = 0.7f * length;
		float coneLength = 0.3f * length;
		float conreRadius = 1.6f * radius;
		
		Vec3 cylinderStart = start;
		Vec3 cylinderEnd = cylinderStart + (direction * cylinderLength);
		Vec3 coneStart = cylinderEnd;
		Vec3 coneEnd = coneStart + (direction * coneLength);

		std::vector<Vertex_PCU>& arrowVerts = arrow3D->m_verts;
		AddVertsForCone3D(arrowVerts, coneStart, coneEnd, conreRadius, arrow3D->m_color, AABB2::ZERO_TO_ONE, numSlices);
		AddVertsForCylinder3D(arrowVerts, cylinderStart, cylinderEnd, radius, arrow3D->m_color, AABB2::ZERO_TO_ONE, numSlices);
		 
		s_theDebugRenderer->AddDebugWorldEntity(arrow3D);
	}
}


void DebugAddWorldText(std::string const& text, Mat44 const& transform, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{
		// init debug entity struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		entityConfig.m_isText = true;
		DebugEntity* worldText3D = new DebugWorldEntity(entityConfig);

		// create the 3d text, facing i-Basis, centered at the basis and properly aligned
		Vec2 textMins = Vec2::ZERO;
		float cellHeight = textHeight;
		float fontAspect = s_theDebugRenderer->GetFontAspect();
		BitmapFont* bitmapFont = s_theDebugRenderer->GetFont();
		std::vector<Vertex_PCU>& worldTextVerts = worldText3D->m_verts;
		Rgba8 textColor = worldText3D->m_color;
		bitmapFont->AddVertsForText3D(worldTextVerts, textMins, cellHeight, text, textColor, fontAspect, alignment);

		// orient text according to the transform
		TransformVertexArray3D(worldTextVerts, transform);

		s_theDebugRenderer->AddDebugWorldEntity(worldText3D);
	}
}

void DebugAddWorldBillboardText(std::string const& text, Vec3 const& origin, float textHeight, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor, DebugRenderMode mode)
{
	if (s_theDebugRenderer)
	{	
		// init debug entity config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_mode = mode;
		entityConfig.m_isWireFrame = false;
		entityConfig.m_isText = true;
		entityConfig.m_billboardType = BillboardType::FULL_CAMERA_OPPOSING;;
		DebugEntity* billboardText3D = new DebugWorldEntity(entityConfig);
		billboardText3D->m_position = origin;
		
		// add verts in the LOCAL space, and later 
		// create the 3d text, facing i-Basis, centered at the basis and properly aligned
		Vec2 textMins = Vec2::ZERO;
		float cellHeight = textHeight;
		float fontAspect = s_theDebugRenderer->GetFontAspect(); 
		BitmapFont* bitmapFont = s_theDebugRenderer->GetFont();
		std::vector<Vertex_PCU>& billboardVerts = billboardText3D->m_verts;
		Rgba8 textColor = billboardText3D->m_color;
		bitmapFont->AddVertsForText3D(billboardVerts, textMins, cellHeight, text, textColor, fontAspect, alignment);

		s_theDebugRenderer->AddDebugWorldEntity(billboardText3D);
	}
}


//----------------------------------------------------------------------------------------------------------
void DebugAddWorldTextHelper( std::string const& text, Vec3 const& origin )
{
	float textHeight   = 0.15f;
	Vec2  alighnment   = Vec2( 0.5f, 0.5f );
	float textDuration = -1.f;

	DebugAddWorldBillboardText( text, origin, textHeight, alighnment, textDuration );
}


void DebugAddScreenText(std::string const& text, Vec2 const& position, float fontSize, Vec2 const& alignment, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	if (s_theDebugRenderer)
	{
		// init debug entity config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_text = text;
		entityConfig.m_textMins = position;
		entityConfig.m_fontSize = fontSize;
		entityConfig.m_alignment = alignment;
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		DebugEntity* screenText = new DebugScreenEntity(entityConfig);

		s_theDebugRenderer->AddDebugScreenEntity(screenText);
	}
}


void DebugAddMessage(std::string const& text, float duration, Rgba8 const& startColor, Rgba8 const& endColor)
{
	if (s_theDebugRenderer)
	{
		// init debug entity config struct
		DebugEntityConfig entityConfig = {};
		entityConfig.m_text = text;
		entityConfig.m_duration = duration;
		entityConfig.m_startColor = startColor;
		entityConfig.m_endColor = endColor;
		entityConfig.m_isMessage = true;
		entityConfig.m_fontSize = 20.f;
		DebugEntity* screenMessage = new DebugScreenEntity(entityConfig);

		s_theDebugRenderer->AddDebugScreenEntity(screenMessage);
	}
}

bool Command_DebugRenderClear(EventArgs& args)
{
	UNUSED(args);

	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->Clear();

		// event consumed
		return true;
	}

	// event not consumed
	return false;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	UNUSED(args);

	if (s_theDebugRenderer)
	{
		s_theDebugRenderer->ToggleDisplayState();

		// event consumed
		return true;
	}

	// event not consumed
	return false;
}
