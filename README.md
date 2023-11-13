# Wave-terrain-synthesizer

A plug-in that implements the wave terrain synthesis method. The plugin was created using C++ and JUCE.

The plugin allows you to generate a 2-dimensional array filled with random values ​​or containing a cosine waveform with a selected frequency, independently for the x and y axes.
The plugin also allows for an image that is processed into a 2-dimensional array.

Additionally, the synthesizer has 3 types of built-in paths that define how values ​​are read from arrays:
-Sine that can be detuned in one of the axes,
-Rose curve, defined by the formula r = a*sin(n*θ),
-Butterfly curve, defined by the formula r = a*e^(sin(θ)) - 2*cos(4*θ) + sin^5(θ/12)
