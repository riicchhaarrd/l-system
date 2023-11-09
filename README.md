# l-system
Lindenmayer system implementation C++
https://en.wikipedia.org/wiki/L-system

# Usage
``./lindenmayer -axiom <axiom> -rules <rules> -iterations <iterations> -angle <angle> -length <length> -output <path>``
``./lindenmayer -axiom X -rules "F=FF,X=F-[[X]+X]+F[+FX]-X" -iterations 6 -angle 22.5 -length 5 -output "result.png"``

![result](https://github.com/riicchhaarrd/l-system/assets/5922568/142e3030-2e56-458d-ab2a-dd36f03122a0)


# Compile
Get stb_image_write from https://github.com/nothings/stb/blob/master/stb_image_write.h

``g++ lindenmayer.cc -o lindenmayer``
