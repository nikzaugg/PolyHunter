#include "headers/Logger.h"
#include "headers/Configuration.h"


namespace bRenderer
{
	static const int LOG_LEVEL = LM_ERROR;

	std::string getTag(LogMode mode)
	{
		switch (mode)
		{
		case LM_INFO:
			return bRenderer::LOG_MODE_INFO();
		case LM_WARNING:
			return bRenderer::LOG_MODE_WARNING();
		case LM_ERROR:
			return bRenderer::LOG_MODE_ERROR();
		case LM_SYS:
			return bRenderer::LOG_MODE_SYSTEM();
		default:
			return bRenderer::LOG_MODE_INFO();
		}
	}

	void log(const std::string &msg, LogMode mode)
	{
	#ifndef NO_LOGGING
        if (mode >= LOG_LEVEL)
			std::cout << getTag(mode) << ": " << msg << std::endl;
	#endif
	}

	void log(const GLubyte *arg, LogMode mode)
	{
	#ifndef NO_LOGGING
        if (mode >= LOG_LEVEL)
			log("", arg, mode);
	#endif
	}

	void log(const std::string &msg, const GLubyte *arg, LogMode mode)
	{	
	#ifndef NO_LOGGING
        if (mode >= LOG_LEVEL)
			std::cout << getTag(mode) << ": " << msg << arg << std::endl;
	#endif
	}

} // namespace bRenderer
