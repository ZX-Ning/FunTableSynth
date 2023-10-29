# FUN TABLE SYNTH 
#### A synthesizer vst plugin that allows custom wavetables with math expressions.
The project is built with JUCE and C++17 while embedding a Python interpreter inside to allow customizing the wavetables with Python code.

Write expressions with Python syntax in the top right input box, and the code you input will be wrapped in a Python function to run. The function has a single parameter "x", and should return a flote value.
<img width="600" alt="image" src="https://github.com/ZX-Ning/FunTableSynth/assets/78404127/e0302064-2ada-4e9b-ab20-bcd0715f91b2">

The errors in your code and exceptions when running will show up below.
<img width="600" alt="image" src="https://github.com/ZX-Ning/FunTableSynth/assets/78404127/49913ef0-a4ae-497e-be4d-676fc7d17107">

It starts a separate to run Python code to avoid blocking. And the calculating work will be automatically stopped when it runs for so long.
<img width="600" alt="image" src="https://github.com/ZX-Ning/FunTableSynth/assets/78404127/3795c5c0-1413-4f98-925e-62281a0b06ba">

### How to build:
Build using CMake and set JUCE_DIR to your JUCE directory.

## TODO:
- Add another parameter to the function, and make it ready for automation.
- Add presets
- Add options for enabling lookup table and max calculation time.
- Optimize UI look
