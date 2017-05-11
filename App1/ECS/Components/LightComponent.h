#pragma once

#include "..\Component.h"

namespace ECS { namespace Components {
	enum LightType
	{
		LightType_Ambient,
		LightType_Point,
		LightType_Directional,
	};

	class LightComponent : public Component
	{
	public:
		LightComponent(LightType type, const glm::vec3 & color, GLsizei shadowMapResolution = 0)
			: type(type), color(color), shadowMapResolution(shadowMapResolution)
		{
			if (this->shadowMapResolution > 0)
			{
				glGenFramebuffers(1, &this->shadowMapFramebufferId);
				glGenTextures(1, &this->shadowMapTextureId);

				glBindTexture(GL_TEXTURE_2D, this->shadowMapTextureId);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, this->shadowMapResolution, this->shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
				GLfloat borderColor[] = { 1.f, 1.f, 1.f, 1.f };
				glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

				glBindFramebuffer(GL_FRAMEBUFFER, this->shadowMapFramebufferId);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->shadowMapTextureId, 0);
				glDrawBuffer(NULL);
				glReadBuffer(NULL);
				glBindFramebuffer(GL_FRAMEBUFFER, 0);

#ifdef _DEBUG
				glObjectLabel(GL_FRAMEBUFFER, this->shadowMapFramebufferId, -1, "ShadowMap Framebuffer");
				glObjectLabel(GL_TEXTURE, this->shadowMapTextureId, -1, "ShadowMap Texture");
#endif
			}
		}

		LightType type;
		glm::vec3 color;

		GLsizei shadowMapResolution;
		GLuint shadowMapFramebufferId, shadowMapTextureId;
	};
}}
