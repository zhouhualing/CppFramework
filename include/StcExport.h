#ifndef STC_EXPORT_H
#define STC_EXPORT_H

#ifdef STC_EXPORTS
	#if defined WIN32
		#define STC_PUBLIC __declspec(dllexport)
	#else
		#define STC_PUBLIC
	#endif
#else
	#if defined WIN32
		#define STC_PUBLIC __declspec(dllimport)
	#else
		#define STC_PUBLIC
	#endif
#endif


#endif // STC_EXPORT_H

