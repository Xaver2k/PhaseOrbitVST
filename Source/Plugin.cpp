// SPDX-License-Identifier: AGPL-3.0-only
#include <juce_audio_utils/juce_audio_utils.h>
#include "Engine.h"
using namespace juce;
static constexpr const char* ids[]={"rate","depth","feedback","centre","delay","width","mix","gain","phaseOn","delayOn","bypass","macro1","macro2","macro3","macro4"};
static constexpr const char* names[]={"Rate","Depth","Feedback","Centre","Delay","Stereo","Mix","Output","Phaser","Delay FX","Bypass","Motion","Space","Intensity","Blend"};
class OrbitEditor;
class OrbitProcessor final: public AudioProcessor {
public:
 AudioProcessorValueTreeState state;
 orbit::Engine engine;
 std::array<std::atomic<int>,15> cc,notes;
 std::array<std::atomic<int>,4> targets;
 std::atomic<int> learn{-1};
 std::atomic<float> meter{0};
 OrbitProcessor():AudioProcessor(BusesProperties().withInput("Input",AudioChannelSet::stereo(),true).withOutput("Output",AudioChannelSet::stereo(),true)),state(*this,nullptr,"PhaseOrbit",layout()){
  for(auto& x:cc)x=-1;for(auto& x:notes)x=-1;
  targets[0]=0;targets[1]=5;targets[2]=2;targets[3]=6;
 }
 static AudioProcessorValueTreeState::ParameterLayout layout(){
  AudioProcessorValueTreeState::ParameterLayout p;
  auto add=[&](int i,float lo,float hi,float value,float skew=1.f){p.add(std::make_unique<AudioParameterFloat>(ParameterID(ids[i],1),names[i],NormalisableRange<float>(lo,hi,0,skew),value));};
  add(0,.02f,12,.35f,.4f);add(1,0,1,.7f);add(2,-.9f,.9f,.25f);add(3,60,5000,800,.4f);add(4,.2f,30,5,.5f);add(5,0,1,.5f);add(6,0,1,.5f);add(7,-24,12,0);
  for(int i=8;i<11;++i)p.add(std::make_unique<AudioParameterBool>(ParameterID(ids[i],1),names[i],i!=10));
  for(int i=11;i<15;++i)add(i,0,1,.5f);
  return p;
 }
 const String getName() const override{return "Phase Orbit";}
 bool acceptsMidi() const override{return true;} bool producesMidi() const override{return false;} bool isMidiEffect() const override{return false;}
 double getTailLengthSeconds() const override{return 8;}
 int getNumPrograms() override{return 1;} int getCurrentProgram() override{return 0;} void setCurrentProgram(int) override{}
 const String getProgramName(int) override{return "Default";} void changeProgramName(int,const String&) override{}
 bool isBusesLayoutSupported(const BusesLayout& b)const override{return b.getMainInputChannelSet()==b.getMainOutputChannelSet()&&(b.getMainOutputChannelSet()==AudioChannelSet::mono()||b.getMainOutputChannelSet()==AudioChannelSet::stereo());}
 void prepareToPlay(double sr,int) override{engine.prepare(sr);}void releaseResources() override{} void reset()override{engine.reset();}
 AudioProcessorParameter* getBypassParameter()const override{return state.getParameter("bypass");}
 float value(int i)const{return state.getRawParameterValue(ids[i])->load();}
 void setNormal(int i,float v){auto* p=state.getParameter(ids[i]);p->beginChangeGesture();p->setValueNotifyingHost(jlimit(0.f,1.f,v));p->endChangeGesture();}
 void midi(const MidiMessage& m){
  const bool isCC=m.isController(), isNote=m.isNoteOn();if(!isCC&&!isNote)return;
  int number=isCC?m.getControllerNumber():m.getNoteNumber();int idx=learn.exchange(-1);
  if(idx>=0&&idx<15){for(int i=0;i<15;++i){if(isCC&&cc[i]==number)cc[i]=-1;if(isNote&&notes[i]==number)notes[i]=-1;}if(isCC)cc[idx]=number;else notes[idx]=number;}
  for(int i=0;i<15;++i)if((isCC&&cc[i]==number)||(isNote&&notes[i]==number)){
   auto* p=state.getParameter(ids[i]);p->setValueNotifyingHost(isCC?float(m.getControllerValue())/127.f:(p->getValue()>=.5f?0.f:1.f));
  }
 }
 orbit::Settings settings(){
  float v[11];for(int i=0;i<11;++i)v[i]=value(i);
  // Macro: centred, bipolar normalized offset. 50% leaves the base value unchanged.
  for(int m=0;m<4;++m){int t=targets[m].load();if(t>=0&&t<8){auto* p=state.getParameter(ids[t]);v[t]=p->convertFrom0to1(jlimit(0.f,1.f,p->convertTo0to1(v[t])+value(m+11)-.5f));}}
  return {v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7],v[8],v[9],v[10]};
 }
 void processBlock(AudioBuffer<float>& b,MidiBuffer& messages) override{
  ScopedNoDenormals guard;float peak=0;int offset=0;
  auto segment=[&](int end){if(end<=offset)return;auto s=settings();float* ptrs[2]={b.getWritePointer(0,offset),b.getNumChannels()>1?b.getWritePointer(1,offset):nullptr};engine.process(ptrs,b.getNumChannels(),end-offset,s);offset=end;};
  for(const auto event:messages){segment(jlimit(0,b.getNumSamples(),event.samplePosition));midi(event.getMessage());}segment(b.getNumSamples());messages.clear();
  for(int ch=0;ch<b.getNumChannels();++ch)peak=jmax(peak,b.getMagnitude(ch,0,b.getNumSamples()));meter=peak;
 }
 bool hasEditor()const override{return true;} AudioProcessorEditor* createEditor() override;
 void getStateInformation(MemoryBlock& b)override{
  auto t=state.copyState();for(int i=0;i<15;++i){t.setProperty("cc"+String(i),cc[i].load(),nullptr);t.setProperty("note"+String(i),notes[i].load(),nullptr);}for(int i=0;i<4;++i)t.setProperty("target"+String(i),targets[i].load(),nullptr);auto xml=t.createXml();copyXmlToBinary(*xml,b);
 }
 void setStateInformation(const void* d,int n)override{
  if(auto xml=getXmlFromBinary(d,n)){auto t=ValueTree::fromXml(*xml);if(!t.hasType("PhaseOrbit"))return;state.replaceState(t);for(int i=0;i<15;++i){cc[i]=jlimit(-1,127,int(t.getProperty("cc"+String(i),-1)));notes[i]=jlimit(-1,127,int(t.getProperty("note"+String(i),-1)));}for(int i=0;i<4;++i)targets[i]=jlimit(-1,7,int(t.getProperty("target"+String(i),-1)));}
 }
};
class OrbitLook:public LookAndFeel_V4 {
public:
 OrbitLook(){setColour(Slider::rotarySliderFillColourId,Colour(0xff52e0c4));setColour(Slider::thumbColourId,Colour(0xff52e0c4));setColour(Slider::textBoxTextColourId,Colours::white);setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);setColour(ComboBox::backgroundColourId,Colour(0xff222d40));setColour(TextButton::buttonColourId,Colour(0xff26394a));setColour(TextButton::buttonOnColourId,Colour(0xff087f79));}
 void drawRotarySlider(Graphics& g,int x,int y,int w,int h,float p,float start,float end,Slider&)override{
  auto r=Rectangle<float>(float(x),float(y),float(w),float(h)).reduced(10);auto radius=jmin(r.getWidth(),r.getHeight())*.5f;auto c=r.getCentre();Path bg,fg;bg.addCentredArc(c.x,c.y,radius,radius,0,start,end,true);fg.addCentredArc(c.x,c.y,radius,radius,0,start,start+p*(end-start),true);g.setColour(Colour(0xff344159));g.strokePath(bg,PathStrokeType(4));g.setColour(Colour(0xff52e0c4));g.strokePath(fg,PathStrokeType(4));g.setColour(Colour(0xff202c40));g.fillEllipse(c.x-radius+7,c.y-radius+7,2*radius-14,2*radius-14);auto angle=start+p*(end-start);g.setColour(Colours::white);g.drawLine(c.x+std::sin(angle)*radius*.45f,c.y-std::cos(angle)*radius*.45f,c.x+std::sin(angle)*radius*.75f,c.y-std::cos(angle)*radius*.75f,2);
 }
};
class OrbitEditor:public AudioProcessorEditor,private Timer {
 OrbitProcessor& p;OrbitLook look;
 std::array<Slider,12> knobs;std::array<Label,12> labels;
 std::vector<std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment>> sa;
 std::array<TextButton,3> switches;std::vector<std::unique_ptr<AudioProcessorValueTreeState::ButtonAttachment>> ba;
 std::array<ComboBox,4> routing;ComboBox learnTarget;TextButton learnButton{"MIDI Learn"},clearButton{"Clear MIDI"};Label status;
 float animation=0;
public:
 OrbitEditor(OrbitProcessor& proc):AudioProcessorEditor(proc),p(proc){
  setLookAndFeel(&look);setWantsKeyboardFocus(true);setSize(960,660);
  for(int i=0;i<12;++i){int idx=i<8?i:i+3;auto& k=knobs[i];k.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);k.setTextBoxStyle(Slider::TextBoxBelow,false,90,22);k.setDoubleClickReturnValue(true,p.state.getParameter(ids[idx])->convertFrom0to1(p.state.getParameter(ids[idx])->getDefaultValue()));if(idx==0)k.setTextValueSuffix(" Hz");if(idx==3)k.setTextValueSuffix(" Hz");if(idx==4)k.setTextValueSuffix(" ms");if(idx==7)k.setTextValueSuffix(" dB");k.setNumDecimalPlacesToDisplay(2);addAndMakeVisible(k);labels[i].setText(names[idx],dontSendNotification);labels[i].setJustificationType(Justification::centred);addAndMakeVisible(labels[i]);sa.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.state,ids[idx],k));}
  for(int i=0;i<3;++i){switches[i].setButtonText(names[8+i]);switches[i].setClickingTogglesState(true);addAndMakeVisible(switches[i]);ba.push_back(std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(p.state,ids[8+i],switches[i]));}
  for(int i=0;i<4;++i){routing[i].addItem("Unassigned",1);for(int j=0;j<8;++j)routing[i].addItem(names[j],j+2);routing[i].setSelectedId(p.targets[i].load()+2,dontSendNotification);routing[i].onChange=[this,i]{p.targets[i]=routing[i].getSelectedId()-2;};addAndMakeVisible(routing[i]);}
  for(int i=0;i<15;++i)learnTarget.addItem(names[i],i+1);learnTarget.setSelectedId(12);addAndMakeVisible(learnTarget);
  learnButton.onClick=[this]{p.learn=(p.learn.load()<0?learnTarget.getSelectedId()-1:-1);};clearButton.onClick=[this]{p.learn=-1;for(auto& v:p.cc)v=-1;for(auto& v:p.notes)v=-1;};addAndMakeVisible(learnButton);addAndMakeVisible(clearButton);addAndMakeVisible(status);startTimerHz(30);
 }
 ~OrbitEditor()override{stopTimer();setLookAndFeel(nullptr);}
 void resized()override{
  for(int i=0;i<8;++i){int x=30+(i%4)*225,y=210+(i/4)*130;labels[i].setBounds(x,y,210,22);knobs[i].setBounds(x+42,y+20,126,105);}
  for(int i=0;i<4;++i){int x=30+i*225;labels[i+8].setBounds(x,482,210,20);knobs[i+8].setBounds(x+50,502,110,86);routing[i].setBounds(x+20,590,170,24);}
  for(int i=0;i<3;++i)switches[i].setBounds(595+i*110,30,100,30);
  learnTarget.setBounds(30,627,160,24);learnButton.setBounds(200,627,110,24);clearButton.setBounds(320,627,110,24);status.setBounds(443,627,490,24);
 }
 void paint(Graphics& g)override{
  g.fillAll(Colour(0xff101725));g.setColour(Colour(0xff1a2537));g.fillRoundedRectangle(20,90,920,100,12);g.fillRoundedRectangle(20,480,920,138,12);
  g.setColour(Colours::white);g.setFont(28);g.drawText("PHASE / ORBIT",30,22,420,35,Justification::centredLeft);g.setFont(12);g.setColour(Colour(0xff94a6c1));g.drawText("STEREO MODULATION     /     8-STAGE PHASER + MODULATED DELAY",32,60,550,20,Justification::centredLeft);
  Path wave;for(int x=0;x<880;++x){float y=140+float(std::sin(double(x)*.016+animation))*28;if(x==0)wave.startNewSubPath(40.f,y);else wave.lineTo(float(x+40),y);}g.setColour(Colour(0xff52e0c4));g.strokePath(wave,PathStrokeType(2));
  g.setFont(11);g.setColour(Colour(0xff94a6c1));g.drawText("MACROS  /  50% = neutral offset",36,462,450,18,Justification::centredLeft);float level=p.meter.load();g.setColour(level>1?Colours::orangered:Colour(0xff52e0c4));g.fillRect(925,210,7,int(jlimit(0.f,1.f,level)*235));
 }
 bool keyPressed(const KeyPress& k)override{
  int key=k.getKeyCode();if(key=='B'){p.setNormal(10,p.value(10)>.5f?0:1);return true;}if(key=='P'){p.setNormal(8,p.value(8)>.5f?0:1);return true;}if(key=='D'){p.setNormal(9,p.value(9)>.5f?0:1);return true;}
  String plus="QWER",minus="ASXF";auto text=String::charToString(k.getTextCharacter()).toUpperCase();if(text.isEmpty())return false;int i=plus.indexOf(text);bool up=i>=0;if(!up)i=minus.indexOf(text);if(i>=0){p.setNormal(11+i,p.value(11+i)+(up?.02f:-.02f));return true;}return false;
 }
 void timerCallback()override{animation+=.03f;for(int i=0;i<4;++i)routing[i].setSelectedId(p.targets[i].load()+2,dontSendNotification);status.setText(p.learn>=0?"Waiting for MIDI CC / note...":"QWER / ASXF: macros   |   P: phaser   D: delay   B: bypass",dontSendNotification);repaint();}
};
AudioProcessorEditor* OrbitProcessor::createEditor(){return new OrbitEditor(*this);}
AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new OrbitProcessor;}
