#ifndef RESAMPLER_H
#define RESAMPLER_H


class ResamplerTest
{
public:
    int process(const float* in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate);

};


class ResamplerLibResampler
{
public:
    int process(const float* in, int inLength, int inSampleRate, bool lastInputChunk, float *out, int outLenght, int outSampleRate);
    ResamplerLibResampler();
    ~ResamplerLibResampler();

private:
    void* libHandler;

};

#endif // RESAMPLER_H
