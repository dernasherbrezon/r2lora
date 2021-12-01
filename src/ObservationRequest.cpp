#include "ObservationRequest.h"

ObservationRequest::ObservationRequest() {
  // FIXME
}

float ObservationRequest::getFreq() { return this->freq; }
void ObservationRequest::setFreq(float freq) { this->freq = freq; }
float ObservationRequest::getBw() { return this->bw; }
void ObservationRequest::setBw(float bw) { this->bw = bw; }
uint8_t ObservationRequest::getSf() { return this->sf; }
void ObservationRequest::setSf(uint8_t sf) { this->sf = sf; }
uint8_t ObservationRequest::getCr() { return this->cr; }
void ObservationRequest::setCr(uint8_t cr) { this->cr = cr; }
uint8_t ObservationRequest::getSyncWord() { return this->syncWord; }
void ObservationRequest::setSyncWord(uint8_t syncWord) {
  this->syncWord = syncWord;
}
int8_t ObservationRequest::getPower() { return this->power; }
void ObservationRequest::setPower(int8_t power) { this->power = power; }
uint16_t ObservationRequest::getPreambleLength() {
  return this->preambleLength;
}
void ObservationRequest::setPreambleLength(uint16_t preambleLength) {
  this->preambleLength = preambleLength;
}
uint8_t ObservationRequest::getGain() { return this->gain; }
void ObservationRequest::setGain(uint8_t gain) { this->gain = gain; }
uint8_t ObservationRequest::getLdro() { return this->ldro; }
void ObservationRequest::setLdro(uint8_t ldro) { this->ldro = ldro; }
