#include "CMS.h"


CMS::CMS(int L, int B, int numDataStreams) {
	_numHashes = L;
	_bucketSize = B;
	_numSketches = numDataStreams;
	_sketchSize = _numHashes * _bucketSize * 2;
	_LHH = new int[_numSketches * _sketchSize]();
	_hashingSeeds = new unsigned int[_numHashes];

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<unsigned int> dis(1, INT_MAX);

	for (int h = 0; h < _numHashes; h++) {
		_hashingSeeds[h] = dis(gen);
		if (_hashingSeeds[h] % 2 == 0) {
			_hashingSeeds[h]++;
		}
	}

	std::cout << "Count Min Sketch Initialized" << std::endl;
}

void CMS::getCanidateHashes(int candidate, unsigned int* hashes){
	for (int hashIndx = 1; hashIndx < _numHashes; hashIndx++) {
		unsigned int h = _hashingSeeds[hashIndx];
		unsigned int k = candidate;
		k *= 0xcc9e2d51;
		k = (k << 15) | (k >> 17);
		k *= 0x1b873593;
		h ^= k;
		h = (h << 13) | (h >> 19);
		h = h * 5 + 0xe6546b64;
		h ^= h >> 16;
		h *= 0x85ebca6b;
		h ^= h >> 13;
		h *= 0xc2b2ae35;
		h ^= h >> 16;
		unsigned int curhash = (unsigned int) h % _bucketSize;
		hashes[hashIndx] = curhash;
	}
}


void CMS::getHashes(int* dataStream, int dataStreamLen, unsigned int* hashIndices) {

	for (int dataIndx = 0; dataIndx < dataStreamLen; dataIndx++) {
		for (int hashIndx = 0; hashIndx < _numHashes; hashIndx++) {
			unsigned int h = _hashingSeeds[hashIndx];
			unsigned int k = (unsigned int) dataStream[dataIndx];
			k *= 0xcc9e2d51;
			k = (k << 15) | (k >> 17);
			k *= 0x1b873593;
			h ^= k;
			h = (h << 13) | (h >> 19);
			h = h * 5 + 0xe6546b64;
			h ^= h >> 16;
			h *= 0x85ebca6b;
			h ^= h >> 13;
			h *= 0xc2b2ae35;
			h ^= h >> 16;
			unsigned int curhash = (unsigned int) h % _bucketSize;
			hashIndices[hashLocation(dataIndx, _numHashes, hashIndx)] = curhash;
		}
	}
}

void CMS::add(int dataStreamIndx, int* dataStream, int dataStreamLen) {

	std::cout << "Adding vectors to Count Min Sketch" << std::endl;

#ifdef TIMER
	auto begin = std::chrono::system_clock::now();
#endif

	unsigned int* hashIndices = new unsigned int[_numHashes * dataStreamLen];
	getHashes(dataStream, dataStreamLen, hashIndices);

	for (int dataIndx = 0; dataIndx < dataStreamLen; dataIndx++) {

		printf("Hashes for entry %d:\n", dataIndx);
		for (int h = 0; h < _numHashes; h++) {
			printf("\t%d", hashIndices[hashLocation(dataIndx, _numHashes, h)]);
		}
		printf("\n");

		for (int hashIndx = 0; hashIndx < _numHashes; hashIndx++) {
			if (dataStream[dataIndx] == TABLENULL) {
				continue;
			}
			unsigned int currentHash = hashIndices[hashLocation(dataIndx, _numHashes, hashIndx)];
			int* LHH_ptr = _LHH + heavyHitterIndx(dataStreamIndx, _sketchSize, _bucketSize, hashIndx, currentHash);
			int* LHH_Count_ptr = _LHH + countIndx(dataStreamIndx, _sketchSize, _bucketSize, hashIndx, currentHash);
			if (*LHH_Count_ptr != 0) {
				if (dataStream[dataIndx] == *LHH_ptr) {
					*LHH_Count_ptr = *LHH_Count_ptr + 1;
				} else {
					*LHH_Count_ptr = *LHH_Count_ptr - 1;
				}
			}
			if (*LHH_Count_ptr == 0) {
				*LHH_ptr = dataStream[dataIndx];
				*LHH_Count_ptr = 1;
			}
		}
	}

	std::cout << "Vectors Added" << std::endl;

#ifdef TIMER
	auto end = std::chrono::system_clock::now();
	auto runTime = (end - begin);
	std::cout << "Runtime: " << runTime.count() << std::endl;
#endif
}

void CMS::topK(int K, int threshold, int* topK, int sketchIndx) {
	LHH* candidates = new LHH[_bucketSize];
	int count = 0;
	for (int b = 0; b < _bucketSize; b++) {
		int currentHeavyHitter = _LHH[heavyHitterIndx(sketchIndx, _sketchSize, _bucketSize, 0, b)];
		int currentCount = _LHH[countIndx(sketchIndx, _sketchSize, _bucketSize, 0, b)];
		if (currentCount >= threshold) {
			candidates[count].heavyHitter = currentHeavyHitter;
			candidates[count].count = currentCount;
			count++;
		} else {
			unsigned int* hashes = new unsigned int[_numHashes];
			getCanidateHashes(currentHeavyHitter, hashes);
			for (int hashIndx = 1; hashIndx < _numHashes; hashIndx++) {
				currentCount = _LHH[countIndx(sketchIndx, _sketchSize, _bucketSize, hashIndx, hashes[hashIndx])];
				if (currentCount > threshold) {
					candidates[count].heavyHitter = currentHeavyHitter;
					candidates[count].count = currentCount;
					count++;
					break;
				}
			}
		}
	}
	for (; count < _bucketSize; count++) {
		candidates[count].heavyHitter = -1;
		candidates[count].count = -1;
	}
	std::sort(candidates, candidates + _bucketSize, [&candidates](LHH a, LHH b){return a.count > b.count;});

	printf("Sorted Candidate Set:\n");
	for (int k = 0; k < _bucketSize; k++) {
		printf("\t%d", candidates[k].heavyHitter);
	}
	printf("\n");

	for (int i = 0; i < K; i++) {
		topK[i] = candidates[i].heavyHitter;
	}
	delete[] candidates;
}


void CMS::showCMS(int sketchIndx) {
	for (int l = 0; l < _numHashes; l++) {
		printf("Bucket %d:\n\t[LHH]: ", l);
		for (int b = 0; b < _bucketSize; b++) {
			printf("\t%d", _LHH[heavyHitterIndx(sketchIndx, _sketchSize, _bucketSize, l, b)]);
		}
		printf("\n\t[Cnt]: ");
		for (int b = 0; b < _bucketSize; b++) {
			printf("\t%d", _LHH[countIndx(sketchIndx, _sketchSize, _bucketSize, l, b)]);
		}
		printf("\n");
	}
}

CMS::~CMS() {

	delete[] _LHH;
	delete[] _hashingSeeds;

	std::cout << "Count Min Sketch Destroyed" << std::endl;

}