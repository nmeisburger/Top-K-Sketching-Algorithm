#ifndef CMS_H_
#define CMS_H_

#define TIMER

#include <iostream>
#include <math.h>
#include <random>
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

	/*
	@param L: The number of hashes per sketch
	@param B: The size of each bucket per hash function per sketch
	@param S: The number of sketches to initialize
	*/
	CMS(int L, int B, int S);

	/*
	@param dataStreamIndx: The index of the data stream (the nth data stream will be added to the nth sketch)
	@param dataStream: The data to be added to the sketch
	@param dataStreamLen: The length of the data to be added
	*/
	void add(int dataStreamIndx, int* dataStream, int dataStreamLen);

	/*
	@param K: The number of top elements to return
	@param threshold: The minimum count that a local heavy hitter must have to be added to the top K candidate set
	@param sketchIndx: The index of the sketch to find the top K from
	*/
	void topK(int K, int threshold, int* topK, int sketchIndx);

	void combineSketches(int* newLHH);

	/*
	@param sketchIndx: The index of the sketch to print the local heavy hitters and their counts from
	*/
	void showCMS(int sketchIndx);

	~CMS();
};

#endif
