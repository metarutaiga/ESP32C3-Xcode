#ifndef crypto_hash_sha512_h
#define crypto_hash_sha512_h

extern int crypto_hash_sha512_tweet(unsigned char *,const unsigned char *,unsigned long long);
#define crypto_hash_sha512 crypto_hash_sha512_tweet

#endif
