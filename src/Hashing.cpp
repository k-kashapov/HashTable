#include "Hashing.h"

int64_t SumHash (const void *data_ptr, int len)
{
    int64_t hash = 0;

    const char *data = (const char *) data_ptr;

    for (int byte = 0; byte < len; byte++)
    {
        hash += data[byte];
    }

    return hash;
}

int64_t FirstSymHash (const void *data_ptr, int len)
{
    const char *data = (const char *) data_ptr;
    return (int64_t) *data;
}

int64_t StrLenHash (const void *data_ptr, int len)
{
    int res_len = 0;
    const char *data = (const char *) data_ptr;

    for (; res_len < len; res_len++)
    {
        if (data[res_len] == '\0') break;
    }

    return res_len;
}

#define ROL(val) ((val << 1) | (val >> (sizeof (val) - 1)))
int64_t RoLHash (const void *data_ptr, int len)
{
    const char *data = (const char *) data_ptr;

    uint64_t hash = *data;

    for (int sym = 1; sym < len; sym++)
    {
        hash = ROL (hash) ^ data[sym];
    }

    return hash;
}
#undef ROL

#define ROR(val) ((val >> 1) | (val << (sizeof (val) - 1)))
int64_t RoRHash (const void *data_ptr, int len)
{
    const char *data = (const char *) data_ptr;

    uint64_t hash = *data;

    for (int sym = 1; sym < len; sym++)
    {
        hash = ROR (hash) ^ data[sym];
    }

    return hash;
}
#undef ROR

#define MMIX(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }
int64_t MurmurHash2A (const void *data_ptr, int len)
{
	const unsigned int m = 0x5bd1e995;
	const          int r = 24;
	      unsigned int l = len;

	const unsigned char *data = (const unsigned char *) data_ptr;

	unsigned int h = 0x12345678;
	unsigned int k = 0;

	while(len >= 4)
	{
		k = *(unsigned int *) data;

		MMIX (h,k);

		data += 4;
		len  -= 4;
	}

	unsigned int t = 0;

	switch (len)
	{
    	case 3:  t ^= data[2] << 16;
    	[[fallthrough]];
    	case 2:  t ^= data[1] << 8;
    	[[fallthrough]];
    	case 1:  t ^= data[0];
    	[[fallthrough]];
    	default: break;
	};

	MMIX (h,t);
	MMIX (h,l);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}
#undef MMIX
