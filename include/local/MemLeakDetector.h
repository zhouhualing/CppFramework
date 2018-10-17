#ifndef MEM_LEAK_DETECTOR_H
#define MEM_LEAK_DETECTOR_H


#if defined(WIN32) && defined(DEBUG_MEM_LEAK) && defined(_DEBUG)

// Currently only works on windows/debug mode
namespace stc {

class MemLeakDetector {
public:

	static MemLeakDetector& Instance();
	~MemLeakDetector();

	void Start();

	void Stop();

private:

	MemLeakDetector();
	MemLeakDetector(const MemLeakDetector&);
	MemLeakDetector& operator = (const MemLeakDetector&);
};

}

#endif

#endif

