// Copyright (c) 2017, Danilo Peixoto. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// * Neither the name of the copyright holder nor the names of its
//   contributors may be used to endorse or promote products derived from
//   this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "srtlerp.h"
#include <iostream>

SRTLERP_NAMESPACE_USING

int main(int argc, char ** argv) {
    FileDescriptor file("C:/Users/Danilo/Desktop/Projetos/srtlerp/input.srt");

    if (file.isOpen())
        std::cout << "Opening subtitle file..." << std::endl;
    else {
        std::cout << "Cannot open subtitle file." << std::endl;
        return 1;
    }

    SubtitleSequence subtitles;

    file.read(subtitles);
    file.close();

    TimeInterval interval;

    interval.start = Time(0, 0, 1, 950);
    interval.end = Time(1, 37, 29, 917);

    if (LinearInterpolation::interpolate(subtitles, interval))
        std::cout << "Interpolating subtitles..." << std::endl;
    else
        std::cout << "Cannot interpolate subtitles." << std::endl;

    file.open("C:/Users/Danilo/Desktop/Projetos/srtlerp/output.srt");

    if (file.isOpen())
        std::cout << "Saving subtitle file..." << std::endl;
    else {
        std::cout << "Cannot save subtitle file." << std::endl;
        return 1;
    }

    file.write(subtitles);
    file.close();

    std::cout << "Completed." << std::endl;

    return 0;
}