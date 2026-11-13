// SPDX-License-Identifier: AGPL-3.0-only
#include "../Source/Engine.h"
#include <iostream>
#include <stdexcept>
#include <random>
void require(bool condition,const char* message){if(!condition)throw std::runtime_error(message);}
int main(){
 for(double sr:{22050.,44100.,48000.,96000.,192000.})for(int channels:{1,2}){
  orbit::Engine e;e.prepare(sr);orbit::Settings s;
  std::array<std::array<float,257>,2> b{};float* ptr[]={b[0].data(),b[1].data()};
  e.process(ptr,channels,257,s);for(auto& ch:b)for(float x:ch)require(x==0,"Silence generated signal");
  std::mt19937 rng(10);std::uniform_real_distribution<float> noise(-.5f,.5f);
  for(int block=0;block<900;++block){s.feedback=block%2?.9f:-.9f;s.rate=block%2?12.f:.02f;s.centre=block%2?5000.f:60.f;s.depth=1;s.delay=block%2?30.f:.2f;
   for(auto& ch:b) { for(auto& x:ch) x=noise(rng); }
   e.process(ptr,channels,257,s);
   for(auto& ch:b)for(float x:ch)require(std::isfinite(x)&&std::abs(x)<50,"Unstable output");
  }
  e.reset();s=orbit::Settings{};s.bypass=1;
  for(int block=0;block<150;++block){for(auto& ch:b)ch.fill(.25f);e.process(ptr,channels,257,s);}
  require(std::abs(b[0][256]-.25f)<.0001f,"Bypass failed");
  e.reset();s=orbit::Settings{};s.mix=0;
  for(int block=0;block<150;++block){for(auto& ch:b)ch.fill(.25f);e.process(ptr,channels,257,s);}
  require(std::abs(b[0][256]-.25f)<.0001f,"Dry mix failed");
 }
 std::cout<<"PASS: silence, extreme automation stability, mono/stereo, 5 sample rates, bypass, dry mix\n";
}
