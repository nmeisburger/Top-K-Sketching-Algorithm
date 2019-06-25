#ifndef CMS_H_
#define CMS_H_

#define TIMER

#include <iostream>
#include <math.h>
#include <random>
#include <chrono>
#include <algorithm>

#define INT_MAX 0xffffffff
#define TABLENULL -1

#define hashLocation(dataIndx, numHashes, hashIndx) ((int) (dataIndx * numHashes + hashIndx))
#define heavyHitterIndx(sketchIndx, sketchSize, bucketSize, hashIndx, hash) ((int) (sketchIndx * sketchSize + bucketSize * 2 * hashIndx + hash * 2))
#define countIndx(sketchIndx, sketchSize, bucketSize, hashIndx, hash) ((int) (sketchIndx * sketchSize + bucketSize * 2 * hashIndx + hash * 2 + 1))
#define sketchIndx(sketchIndx, sketchSize) ((int) (sketchIndx * sketchSize))

struct LHH {
	int heavyHitter;
	int count;
};

class CMS {
private:
	int* _LHH;
	int _numHashes, _bucketSize, _numSketches, _sketchSize;
	unsigned int* _hashingSeeds;

	void getCanidateHashes(int candidate, unsigned int* hashes);
	void getHashes(int* data, int dataSize, unsigned int* hashIndices);

public:
	CMS(int L, int B, int S);

	void add(int dataStreamIndx, int* dataStream, int dataStreamLen);

	void topK(int K, int threshold, int* topK, int sketchIndx);

	void showCMS(int dataStreamIndx);

	~CMS();
};

#endif
