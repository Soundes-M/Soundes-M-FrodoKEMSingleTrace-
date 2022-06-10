#include "Frodo.h"
#include "inner.h"

/* see Frodo.h */
void
shake256_init(shake256_context *sc)
{
	inner_shake256_init((inner_shake256_context *)sc);
}

/* see Frodo.h */
void
shake256_inject(shake256_context *sc, const void *data, size_t len)
{
	inner_shake256_inject((inner_shake256_context *)sc, data, len);
}

/* see Frodo.h */
void
shake256_flip(shake256_context *sc)
{
	inner_shake256_flip((inner_shake256_context *)sc);
}

/* see Frodo.h */
void
shake256_extract(shake256_context *sc, void *out, size_t len)
{
	inner_shake256_extract((inner_shake256_context *)sc, out, len);
}

/* see Frodo.h */
void
shake256_init_prng_from_seed(shake256_context *sc,
	const void *seed, size_t seed_len)
{
	shake256_init(sc);
	shake256_inject(sc, seed, seed_len);
}

/* see Frodo.h */
int
shake256_init_prng_from_system(shake256_context *sc)
{
	uint8_t seed[48];

	if (!Zf(get_seed)(seed, sizeof seed)) {
		return Frodo_ERR_RANDOM;
	}
	shake256_init(sc);
	shake256_inject(sc, seed, sizeof seed);
	return 0;
}

static inline uint8_t *
align_u64(void *tmp)
{
	uint8_t *atmp;
	unsigned off;

	atmp = tmp;
	off = (uintptr_t)atmp & 7u;
	if (off != 0) {
		atmp += 8u - off;
	}
	return atmp;
}

static inline uint8_t *
align_u16(void *tmp)
{
	uint8_t *atmp;

	atmp = tmp;
	if (((uintptr_t)atmp & 1u) != 0) {
		atmp ++;
	}
	return atmp;
}

static inline fpr *
align_fpr(void *tmp)
{
	uint8_t *atmp;
	unsigned off;

	atmp = tmp;
	off = (uintptr_t)atmp & 7u;
	if (off != 0) {
		atmp += 8u - off;
	}
	return (fpr *)atmp;
}
 
/* see Frodo.h */
int
Frodo_get_logn(void *obj, size_t len)
{
	int logn;

	if (len == 0) {
		return Frodo_ERR_FORMAT;
	}
	logn = *(uint8_t *)obj & 0x0F;
	if (logn < 1 || logn > 10) {
		return Frodo_ERR_FORMAT;
	}
	return logn;
} 
