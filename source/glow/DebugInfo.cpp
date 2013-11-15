
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cassert>

#include <glow/ObjectRegistry.h>
#include <glow/logging.h>
#include <glow/global.h>

#include <glow/Object.h>
#include <glow/Buffer.h>
#include <glow/FrameBufferObject.h>
#include <glow/Program.h>
#include <glow/Query.h>
#include <glow/RenderBufferObject.h>
#include <glow/Shader.h>
#include <glow/ShaderFile.h>
#include <glow/ShaderSource.h>
#include <glow/Texture.h>
#include <glow/TransformFeedback.h>
#include <glow/VertexArrayObject.h>
#include <glow/VertexAttributeBinding.h>
#include <glow/TextureAttachment.h>
#include <glow/RenderBufferAttachment.h>

#include <glow/DebugInfo.h>

namespace glow
{

DebugInfo::DebugInfo()
{
}

DebugInfo::~DebugInfo()
{
}

void DebugInfo::printObjectInfo()
{
	print(objectInfo());
}

void DebugInfo::printGeneralInfo()
{
	print(generalInfo());
}

void DebugInfo::printAll()
{
	printGeneralInfo();
	printObjectInfo();
}

void DebugInfo::print(const std::vector<InfoGroup>& info)
{
	for (const InfoGroup& group: info)
	{
		glow::debug() << group.name;

		for (const InfoUnit& unit: group.units)
		{
			glow::debug() << "\t" << unit.name;

			for (const Property& property: unit.properties)
			{
				glow::debug() << "\t\t" << property.name << ": " << property.value;
			}
		}
	}
}

std::vector<DebugInfo::InfoGroup> DebugInfo::generalInfo()
{
	InfoGroup generalGroup;

	InfoUnit generalInfo;
	InfoUnit memoryInfo;
	InfoUnit textureInfo;

	generalGroup.name = "General";

	generalInfo.name = "OpenGL";
	memoryInfo.name = "Memory";
	textureInfo.name = "General Texture Info";

	generalInfo.addProperty("version", query::version().toString());
	generalInfo.addProperty("vendor", query::vendor());
	generalInfo.addProperty("renderer", query::renderer());
	generalInfo.addProperty("core profile", query::isCoreProfile()?"true":"false");
	generalInfo.addProperty("GLSL version", query::getString(GL_SHADING_LANGUAGE_VERSION));

	memoryInfo.addProperty("total", humanReadableSize(1024ll*memory::total()));
	memoryInfo.addProperty("dedicated", humanReadableSize(1024ll*memory::dedicated()));
	memoryInfo.addProperty("available", humanReadableSize(1024ll*memory::available()));
	memoryInfo.addProperty("evicted", humanReadableSize(1024ll*memory::evicted()));
	memoryInfo.addProperty("evictionCount", memory::evictionCount());

	int maxTextureSize = query::getInteger(GL_MAX_TEXTURE_SIZE);
	textureInfo.addProperty("Max Texture Size", std::to_string(maxTextureSize)+" x "+std::to_string(maxTextureSize));
	textureInfo.addProperty("Max Vertex Texture Image Units", query::getInteger(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS));
	textureInfo.addProperty("Max Texture Image Units", query::getInteger(GL_MAX_IMAGE_UNITS));
	textureInfo.addProperty("Max Geometry Texture Units", query::getInteger(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS));
	std::vector<int> maxViewportSize = query::getIntegers(GL_MAX_VIEWPORT_DIMS, 2);
	textureInfo.addProperty("Max viewport size", std::to_string(maxViewportSize[0])+" x "+std::to_string(maxViewportSize[1]));
	textureInfo.addProperty("Max clip distances", query::getInteger(GL_MAX_CLIP_DISTANCES));
	textureInfo.addProperty("Max Samples", query::getInteger(GL_MAX_SAMPLES));

	generalGroup.addInfoUnit(generalInfo);
	generalGroup.addInfoUnit(memoryInfo);
	generalGroup.addInfoUnit(textureInfo);

	return std::vector<InfoGroup>({ generalGroup });
}

std::vector<DebugInfo::InfoGroup> DebugInfo::objectInfo()
{
	DebugInfo debugInfo;
	return debugInfo.collectObjectInfo();
}




std::vector<DebugInfo::InfoGroup> DebugInfo::collectObjectInfo()
{
	m_infoGroups.clear();
	m_memoryUsage.clear();

	for (Object* object: ObjectRegistry::objects())
	{
		visit(object);
	}

	std::vector<InfoGroup> groups;

	InfoGroup overview;
	overview.name = "Overview";
	InfoUnit memory;
	memory.name = "Memory Usage";
	unsigned total = 0;
	for (const std::pair<std::string, unsigned>& pair: m_memoryUsage)
	{
		memory.addProperty(pair.first, humanReadableSize(pair.second));
		total += pair.second;
	}
	memory.addProperty("Total", humanReadableSize(total));
	overview.addInfoUnit(memory);

	groups.push_back(overview);


	for (const std::pair<std::string, InfoGroup>& pair: m_infoGroups)
	{
		groups.push_back(pair.second);
	}

	return groups;
}

void DebugInfo::visitBuffer(Buffer* buffer)
{
    assert(buffer != nullptr);

	InfoUnit info;
	info.name = name("Buffer", buffer);

	GLint memory = buffer->getParameter(GL_BUFFER_SIZE);
	m_memoryUsage["Buffers"] += memory;
	info.addProperty("memory", humanReadableSize(memory));

	addInfo("Buffers", info);
}

void DebugInfo::visitFrameBufferObject(FrameBufferObject* fbo)
{
    assert(fbo != nullptr);

	InfoUnit info;
	info.name = name("FrameBufferObject", fbo);

	for (FrameBufferAttachment* attachment: fbo->attachments())
	{
		std::string objectName;
		if (attachment->isTextureAttachment())
		{
			objectName = name("Texture", dynamic_cast<TextureAttachment*>(attachment)->texture());
		}
		else
		{
			objectName = name("RenderBufferObject", dynamic_cast<RenderBufferAttachment*>(attachment)->renderBuffer());
		}

		info.addProperty(attachment->attachmentString(), objectName);
	}

	addInfo("FrameBufferObjects", info);
}

void DebugInfo::visitProgram(Program* program)
{
    assert(program != nullptr);

	InfoUnit info;
	info.name = name("Program", program);

	GLint memory = program->get(GL_PROGRAM_BINARY_LENGTH);
	m_memoryUsage["Programs"] += memory;
	info.addProperty("memory", humanReadableSize(memory));

	int i = 0;
	for (Shader* shader: program->shaders())
	{
		info.addProperty("Shader "+std::to_string(i++), name("Shader", shader));
	}

	addInfo("Programs", info);
}

void DebugInfo::visitRenderBufferObject(RenderBufferObject* rbo)
{
    assert(rbo != nullptr);

	InfoUnit info;
	info.name = name("RenderBufferObject", rbo);

	int w = rbo->getParameter(GL_RENDERBUFFER_WIDTH);
	int h = rbo->getParameter(GL_RENDERBUFFER_HEIGHT);

	int r = rbo->getParameter(GL_RENDERBUFFER_RED_SIZE);
	int g = rbo->getParameter(GL_RENDERBUFFER_GREEN_SIZE);
	int b = rbo->getParameter(GL_RENDERBUFFER_BLUE_SIZE);
	int a = rbo->getParameter(GL_RENDERBUFFER_ALPHA_SIZE);

	int d = rbo->getParameter(GL_RENDERBUFFER_DEPTH_SIZE);
	int s = rbo->getParameter(GL_RENDERBUFFER_STENCIL_SIZE);

	int memory = (int)std::ceil((w*h*(r+g+b+a+d+s))/8.0);

	m_memoryUsage["RenderBufferObjects"] += memory;
	info.addProperty("memory", humanReadableSize(memory));
	info.addProperty("size", std::to_string(w)+" x "+std::to_string(h));

	addInfo("RenderBufferObjects", info);
}

void DebugInfo::visitShader(Shader* shader)
{
    assert(shader != nullptr);

	InfoUnit info;
	info.name = name("Shader", shader);

	info.addProperty("type", shader->typeString());
	if (shader->source() && shader->source()->isFile())
	{
		info.addProperty("source", dynamic_cast<const ShaderFile*>(shader->source())->filePath());
	}

	addInfo("Shaders", info);
}

void DebugInfo::visitTexture(Texture* texture)
{
    assert(texture != nullptr);

	InfoUnit info;
	info.name = name("Texture", texture);

	int maxTextureSize = query::getInteger(GL_MAX_TEXTURE_SIZE);
	int maxLevels = (int)std::ceil(std::log(maxTextureSize)/std::log(2))+1;

	int memory = 0;
	for (int i = 0; i<=maxLevels; ++i)
	{
        if (texture->getLevelParameter(i, GL_TEXTURE_COMPRESSED) == GL_TRUE)
		{
			memory += texture->getLevelParameter(i, GL_TEXTURE_COMPRESSED_IMAGE_SIZE);
		}
		else
		{
			int w = texture->getLevelParameter(i, GL_TEXTURE_WIDTH);
			int h = texture->getLevelParameter(i, GL_TEXTURE_HEIGHT);
			int d = texture->getLevelParameter(i, GL_TEXTURE_DEPTH);

			int r = texture->getLevelParameter(i, GL_TEXTURE_RED_SIZE);
			int g = texture->getLevelParameter(i, GL_TEXTURE_GREEN_SIZE);
			int b = texture->getLevelParameter(i, GL_TEXTURE_BLUE_SIZE);
			int a = texture->getLevelParameter(i, GL_TEXTURE_ALPHA_SIZE);
			int ds = texture->getLevelParameter(i, GL_TEXTURE_DEPTH_SIZE);

			memory += (int)std::ceil((w*h*d*(r+g+b+a+ds))/8.0);
		}
	}


	m_memoryUsage["Textures"] += memory;
	info.addProperty("memory", humanReadableSize(memory));
	info.addProperty("size", std::to_string(texture->getLevelParameter(0, GL_TEXTURE_WIDTH))+" x "+std::to_string(texture->getLevelParameter(0, GL_TEXTURE_HEIGHT)));

	addInfo("Textures", info);
}

void DebugInfo::visitTransformFeedback(TransformFeedback* transformfeedback)
{
    assert(transformfeedback != nullptr);

	InfoUnit info;
	info.name = name("TransformFeedback", transformfeedback);

	addInfo("TransformFeedbacks", info);
}

void DebugInfo::visitVertexArrayObject(VertexArrayObject* vao)
{
    assert(vao != nullptr);

	InfoUnit info;
	info.name = name("VertexArrayObject", vao);

	for (VertexAttributeBinding* binding: vao->bindings())
	{
		info.addProperty("binding "+std::to_string(binding->bindingIndex())
            , "location " + std::to_string(binding->attributeIndex()) 
            + " -> " + name("Buffer", binding->buffer()));
	}

	addInfo("VertexArrayObjects", info);
}



DebugInfo::InfoGroup& DebugInfo::group(const std::string& name)
{
	if (m_infoGroups.find(name) == m_infoGroups.end())
	{
		InfoGroup group;
		group.name = name;
		m_infoGroups[name] = group;
	}

	return m_infoGroups[name];
}

void DebugInfo::addInfo(const std::string& groupName, const InfoUnit& info)
{
	group(groupName).addInfoUnit(info);
}

std::string DebugInfo::humanReadableSize(long long bytes)
{
	if (bytes<0)
	{
		return "n/a";
	}

	static std::vector<char> prefix = { 'k', 'M', 'G', 'T' };
	int power = -1;
	double value = static_cast<double>(bytes);

	std::stringstream ss;

	while (value>=1024.0 && power<static_cast<int>(prefix.size()-1))
	{
		value /= 1024.0;
		power++;
	}

	ss << std::setprecision(3) << value;
	if (power>=0)
	{
		ss << prefix[power];
	}

	ss << "b";

	return ss.str();
}

std::string DebugInfo::name(const std::string& typeName, Object* object)
{
    assert(object != nullptr);

	std::stringstream ss;

	ss << typeName << " (" << object->id();
	if (!object->name().empty()) ss << ", " << object->name();
	ss << ")";

	return ss.str();
}

void DebugInfo::InfoUnit::addProperty(const std::string& name, const std::string& value)
{
	properties.push_back({name, value});
}

void DebugInfo::InfoUnit::addProperty(const std::string& name, GLint value)
{
	addProperty(name, std::to_string(value));
}

void DebugInfo::InfoGroup::addInfoUnit(const InfoUnit& unit)
{
	units.push_back(unit);
}


} // namespace glow
