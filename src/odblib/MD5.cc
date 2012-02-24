#include "MD5.h"
#include "Exceptions.h"
#include <string.h>

MD5::MD5() { md5_init(&state_); }

MD5::~MD5() {}

void MD5::add(const void* buffer, long length)
{
	ASSERT(length > 0);

	md5_add(&state_, static_cast<const unsigned char*>(buffer), length);
}

string MD5::digest()
{
	if(digest_.length() == 0)
	{
		char digest[33];
		md5_end(&state_, digest);
		digest[32] = 0;
		digest_ = digest;
	}
	return digest_;
}
