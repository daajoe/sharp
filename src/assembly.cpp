#ifdef HAVE_CONFIG_H
	#include <config.h>
#endif

#include <sharp/global>

//https://cmake.org/cmake-tutorial/
//http://stackoverflow.com/questions/1435953/how-can-i-pass-git-sha1-to-compiler-as-definition-using-cmake


extern "C"
{
	SHARP_API const char *sharp_version()
	{
		return "0.9.9";

//		return VERSION;
	}

	SHARP_API const char *sharp_version_2()
	{
		return "0.9.9";

//		return VERSION;
	}

	SHARP_API const char *sharp_version_2_0()
	{
		return "0.9.9";

//		return VERSION;
	}
}

