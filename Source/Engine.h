// SPDX-License-Identifier: AGPL-3.0-only
#pragma once
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>
namespace orbit {
constexpr double pi=3.14159265358979323846;
struct Settings { float rate=.35f,depth=.7f,feedback=.25f,centre=800,delay=5,width=.5f,mix=.5f,gain=0,phaseOn=1,delayOn=1,bypass=0; };
class Engine {
 double sr=48000,phase=0; size_t pos=0;
 std::array<std::vector<float>,2> ring;
 std::array<std::array<float,8>,2> memory{};
 std::array<float,2> fb{};
 Settings smooth;
public:
 void prepare(double sampleRate) { sr=sampleRate; for(auto& r:ring) r.assign(size_t(sr*.08)+4,0); reset(); }
 void reset(){for(auto& r:ring) std::fill(r.begin(),r.end(),0); memory={};fb={};pos=0;phase=0;smooth=Settings{};}
 void process(float* const* data,int channels,int count,const Settings& target){
  const float k=float(1-std::exp(-1/(sr*.02)));
  for(int n=0;n<count;++n){
#define S(x) smooth.x+=k*(target.x-smooth.x)
   S(rate);S(depth);S(feedback);S(centre);S(delay);S(width);S(mix);S(gain);S(phaseOn);S(delayOn);S(bypass);
#undef S
   for(int ch=0;ch<channels;++ch){
    const float dry=data[ch][n];
    const double lfo=std::sin(phase+ch*smooth.width*pi);
    const double hz=std::clamp(double(smooth.centre)*std::pow(2.,lfo*smooth.depth*2.5),30.,sr*.4);
    const double t=std::tan(pi*hz/sr); const float a=float((1-t)/(1+t));
    float wet=dry+std::tanh(fb[ch])*smooth.feedback;
    for(auto& z:memory[ch]){float y=-a*wet+z;z=wet+a*y;wet=y;}
    fb[ch]=wet;
    wet=dry+(wet-dry)*smooth.phaseOn;
    const auto size=ring[ch].size();
    const double delaySamples=sr*.001*std::clamp(double(smooth.delay)*(1+.85*smooth.depth*lfo),.1,60.);
    double read=double(pos)-delaySamples;while(read<0) read+=double(size);
    auto i=size_t(read);const float frac=float(read-double(i));
    float delayed=ring[ch][i]*(1-frac)+ring[ch][(i+1)%size]*frac;
    ring[ch][pos]=wet+std::tanh(delayed)*smooth.feedback;
    wet+=(delayed-wet)*smooth.delayOn;
    const float out=(dry+smooth.mix*(wet-dry))*std::pow(10.f,smooth.gain/20.f);
    data[ch][n]=out+(dry-out)*smooth.bypass;
   }
   pos=(pos+1)%ring[0].size();phase+=2*pi*smooth.rate/sr;if(phase>=2*pi)phase-=2*pi;
  }
 }
};
}
