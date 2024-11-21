/**
 * @file TransposeKnob.h
 * @author Mate Narh
 * 
 * Class for a transpose knob
 * 
 * Ideally the transpose knob can traverse infinite semitones upward and 
 * downward, but for practical purposes and adherrence to convention, 
 * restrict transpose boundary to ±2 octaves (±24 semitones)
 */

#ifndef TRANSPOSE_KNOB_H
#define TRANSPOSE_KNOB_H

#include <RotaryEncoder.h>


class TransposeKnob : public RotaryEncoder
{

};


#endif // TRANSPOSE_KNOB_H