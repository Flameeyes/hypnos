#include <stdint.h>

#ifdef __BIG_ENDIAN_
	uint32_t ltohl(uint32_t dword)
	{
		uint32_t ret;
		uint8_t *pRet = (uint8_t*)&ret;

		pRet[0] = dword >> 24;
		pRet[1] = (dword >> 16)&0xFF;
		pRet[2] = (dword >> 8)&0xFF;
		pRet[3] = dword&0xFF;
		
		return ret;
	}

	uint16_t ltohs(uint16_t word)
	{
		uint16_t ret;
		uint8_t *pRet = (uint8_t*)&ret;

		pRet[0] = word >> 8;
		pRet[1] = word&0xFF;
		
		return ret;
	}
#else
	#define ltohl(x) x
	#define ltohs(x) x
#endif

class cGumpIDX
{
private:
	uint32_t lookup;
	uint32_t size;
	uint16_t height;
	uint16_t width;
public:
	uint32_t getLookup()
	{ return ltohl(lookup); }
	
	uint32_t getSize()
	{ return ltohl(size); }
	
	uint16_t getHeight()
	{ return ltohs(height); }
	
	uint16_t getWidth()
	{ return ltohs(width); }
} __attribute__ ((packed));
