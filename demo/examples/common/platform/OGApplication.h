#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include "platform/OGStdC.h"
//#include "platform/OGCommon.h"
#include "platform/OGApplicationProtocol.h"
#include <string>

OG_BEGIN

class Application : public ApplicationProtocol
{
public:
	/**
	 * @js ctor
	 */
	Application();
	/**
	 * @js NA
	 * @lua NA
	 */
	virtual ~Application();

	/**
	@brief    Run the message loop.
	*/
	int run();

	virtual bool openURL(const std::string & url);

	/**
	@brief    Get current application instance.
	@return Current application instance pointer.
	*/
	static Application* getInstance();

	/* override functions */
// 	virtual void setAnimationInterval(float interval) override;
// 
// //	virtual LanguageType getCurrentLanguage();
// 
// 	virtual const char * getCurrentLanguageCode();
// 
// 	/**
// 	 @brief Get target platform
// 	 */
// 	virtual Platform getTargetPlatform();
// 
// 	/**
// 	@brief Get application version
// 	*/
// 	virtual std::string getVersion() override;
// 
// 	/**
// 	 @brief Open url in default browser
// 	 @param String with url to open.
// 	 @return true if the resource located by the URL was successfully opened; otherwise false.
// 	 */
// 	virtual bool openURL(const std::string &url);
// 
// 	/**
// 	 *  Sets the Resource root path.
// 	 *  @deprecated Please use FileUtils::getInstance()->setSearchPaths() instead.
// 	 */
// 	  void setResourceRootPath(const std::string& rootResDir);
// 
// 	/**
// 	 *  Gets the Resource root path.
// 	 *  @deprecated Please use FileUtils::getInstance()->getSearchPaths() instead.
// 	 */
// 	  const std::string& getResourceRootPath();
// 
// 	void setStartupScriptFilename(const std::string& startupScriptFile);

 

protected:
// 	HINSTANCE           _instance;
// 	HACCEL              _accelTable;
// 	LARGE_INTEGER       _animationInterval;
// 	std::string         _resourceRootPath;
// 	std::string         _startupScriptFilename;

	static Application * sm_pSharedApplication;
};

OG_END


#endif