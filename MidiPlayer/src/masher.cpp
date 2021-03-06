#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include "alive\AliveAudio.h"

using namespace std;

struct DDVHeader
{
    uint32_t ddvTag;

    // offset 4
    uint32_t ddvVersion;

    // offset 8
    uint32_t contains;          // 0x3 = audio and video, 0x1 = video 0x2 = audio (might be other way round)
    uint32_t frameRate;
    uint32_t numberOfFrames;


    uint32_t field5; // Probably a reserved field, it has no effect and isn't read by masher lib
    uint32_t width;
    uint32_t height;


    uint32_t maxVideoFrameSize; // must add 8 to this for some reason
    uint32_t field9; // size of a buffer that contains shorts, i.e read field9 * 2
    uint32_t keyFrameRate;

    uint32_t audioFormat;
    uint32_t sampleRate;
    uint32_t maxAudioFrameSize;
    uint32_t fieldE; // seen this be * num frames before.. something to do with audio
    uint32_t framesInterleave;
};

int main2(int, char**)
{
    FILE* fp = fopen("REWARD.DDV", "rb");
    //FILE* fp = fopen("Masher/GDENDING.DDV", "rb");

	AliveInitAudio();

    DDVHeader header = {};
    fread(&header, 0x40, 1, fp);

    const uint32_t audioArraySize = header.framesInterleave;
    const uint32_t videoArraySize = header.numberOfFrames;

    std::vector<uint32_t> pAudioFrameSizes(audioArraySize);
    std::vector<uint32_t> pVideoFrameSizes(videoArraySize);

    fread(pAudioFrameSizes.data(), audioArraySize * sizeof(uint32_t), 1, fp);
    fread(pVideoFrameSizes.data(), videoArraySize * sizeof(uint32_t), 1, fp);

    std::vector<std::vector<uint8_t>> ppAudioFrames(header.framesInterleave);
    std::vector<std::vector<uint8_t>> ppVideoFrames(header.numberOfFrames);

    for (uint32_t frame = 0; frame < header.framesInterleave; frame++)
    {
        ppAudioFrames[frame].resize(pAudioFrameSizes[frame]);
        fread(ppAudioFrames[frame].data(), pAudioFrameSizes[frame], 1, fp);
    }

    for (uint32_t frame = 0; frame < header.numberOfFrames; frame++)
    {
        uint16_t someOffset = 0;
        fread(&someOffset, 2, 1, fp);

        ppVideoFrames[frame].resize(pVideoFrameSizes[frame] + 2);
        fread(ppVideoFrames[frame].data(), pVideoFrameSizes[frame] + 2, 1, fp);

        if (someOffset + 2 >= ppVideoFrames[frame].size())
        {
            std::cout << someOffset + 2 << " is out of bounds on frame " << frame << std::endl;
        }
        else
        {
            uint8_t* pAudio = &ppVideoFrames[frame][someOffset + 2];
			
            int nDebug = 0;
			
        }
    }

	FILE* fp2 = fopen("AudioOut.DDA", "w");

	for (auto audio : ppAudioFrames)
	{
		fwrite(audio.data(), audio.size(), 1, fp2);
	}

    fclose(fp);
	fclose(fp2);
	cin.get();
    return 0;

}
