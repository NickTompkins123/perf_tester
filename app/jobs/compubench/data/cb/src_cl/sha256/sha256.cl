uint charCodeAt(__global unsigned char* str, int len, int index)
{
	if (index < len)
		return (uint)str[index];

	return 0;
}

uint ROTR(uint n, uint x) { return (x >> n) | (x << (32-n)); }
//uint ROTR(uint n, uint x) { return rotate(x, (uint)(32-n)); }

uint Sigma0(uint x) { return ROTR(2,  x) ^ ROTR(13, x) ^ ROTR(22, x); }

uint Sigma1(uint x) { return ROTR(6,  x) ^ ROTR(11, x) ^ ROTR(25, x); }

uint sigma0(uint x) { return ROTR(7,  x) ^ ROTR(18, x) ^ (x>>3);  }

uint sigma1(uint x) { return ROTR(17, x) ^ ROTR(19, x) ^ (x>>10); }

uint Ch(uint x, uint y, uint z)  { return (x & y) ^ (~x & z); }

uint Maj(uint x, uint y, uint z) { return (x & y) ^ (x & z) ^ (y & z); }


__constant char hexChars[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

__constant uint K[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
             0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
             0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
             0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
             0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
             0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
             0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
             0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};

void toHexStr(uint n, __global char* result) {		
	for (int i=7, j=0; i>=0; i--,j++) 	
		result[j] = hexChars[(n>>(i*4)) & 0xf];	
}

__kernel void sha256(
	__global unsigned char* str,
	__global int* strStart,
	__global int* N,
	__global uint* M,
	__global int* startM,
	__global char* result,
	const int strCount)
{
	const int id = get_global_id(0);

	if (id >= strCount)
		return;

	const int strLen = strStart[id+1] - strStart[id];
	//unsigned char* currStr = (unsigned char*)str+strStart[id];
	__global unsigned char* currStr = str+strStart[id];
	const int currN = N[id];
	//uint* currM = (uint*)M+startM[id];
	__global uint* currM = M+startM[id];

	for (int i=0; i<currN; i++) {
		//#pragma unroll
        for (int j=0; j<16; j++) {  // encode 4 chars per integer, big-endian encoding
            currM[i*16+j] = (charCodeAt(currStr, strLen, i*64+j*4)<<24) | 
							(charCodeAt(currStr, strLen, i*64+j*4+1)<<16) | 
							(charCodeAt(currStr, strLen, i*64+j*4+2)<<8) | 
							(charCodeAt(currStr, strLen, i*64+j*4+3));
        } 
    }

	// add length (in bits) into final pair of 32-bit integers (big-endian) [§5.1.1]
    // note: most significant word would be (len-1)*8 >>> 32, but since JS converts
    // bitwise-op args to 32 bits, we need to simulate this by arithmetic operators
    currM[(currN-1)*16+14] = 0;//floor(((strLen-1)*8) / pow(2, 32)); 	
    currM[(currN-1)*16+15] = ((strLen-1)*8) & 0xffffffff;

	uint W[64];
	uint a, b, c, d, e, f, g, h;
	uint H[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};

    for (int i=0; i<currN; i++) {
        // 1 - prepare message schedule 'W'
        //#pragma unroll
		for (int t=0;  t<16; t++) 
			W[t] = currM[i*16+t];

		//#pragma unroll
        for (int t=16; t<64; t++) 
			W[t] = (sigma1(W[t-2]) + W[t-7] + sigma0(W[t-15]) + W[t-16]) & 0xffffffff;

        // 2 - initialise working variables a, b, c, d, e, f, g, h with previous hash value
        a = H[0]; 
		b = H[1]; 
		c = H[2]; 
		d = H[3]; 
		e = H[4]; 
		f = H[5]; 
		g = H[6]; 
		h = H[7];

        // 3 - main loop (note 'addition modulo 2^32')
		//#pragma unroll
        for (int t=0; t<64; t++) {
            uint T1 = h + Sigma1(e) + Ch(e, f, g) + K[t] + W[t];
            uint T2 = Sigma0(a) + Maj(a, b, c);
            h = g;
            g = f;
            f = e;
            e = (d + T1) & 0xffffffff;
            d = c;
            c = b;
            b = a;
            a = (T1 + T2) & 0xffffffff;
        }

         // 4 - compute the new intermediate hash value (note 'addition modulo 2^32')
        H[0] = (H[0]+a) & 0xffffffff;
        H[1] = (H[1]+b) & 0xffffffff; 
        H[2] = (H[2]+c) & 0xffffffff; 
        H[3] = (H[3]+d) & 0xffffffff; 
        H[4] = (H[4]+e) & 0xffffffff;
        H[5] = (H[5]+f) & 0xffffffff;
        H[6] = (H[6]+g) & 0xffffffff; 
        H[7] = (H[7]+h) & 0xffffffff; 
    }

	for (int i=0; i<8; i++)
		toHexStr(H[i], result+id*64+i*8);
	/*
	return toHexStr(H[0]) + toHexStr(H[1]) + toHexStr(H[2]) + toHexStr(H[3]) + 
           toHexStr(H[4]) + toHexStr(H[5]) + toHexStr(H[6]) + toHexStr(H[7]);
		   */
}
	