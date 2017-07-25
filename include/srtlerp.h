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

#ifndef SRTLERP_H
#define SRTLERP_H

#define SRTLERP_NAME "SRTLERP"
#define SRTLERP_VERSION "1.0.0"
#define SRTLERP_AUTHOR "Danilo Peixoto"
#define SRTLERP_COPYRIGHT "Copyright (c) 2017, Danilo Peixoto. All rights reserved."
#define SRTLERP_LICENSE "BSD-3-Clause License"

#define SRTLERP_NAMESPACE_BEGIN namespace srtlerp {
#define SRTLERP_NAMESPACE_END };
#define SRTLERP_NAMESPACE_USING using namespace srtlerp;

#define SRTLERP_NULL nullptr

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

SRTLERP_NAMESPACE_BEGIN

typedef char Character;
typedef bool Bool;
typedef long long int Int;
typedef unsigned long long int UInt;
typedef double Float;

struct Subtitle;

typedef std::vector<Subtitle> SubtitleSequence;
typedef std::string TextContent;

struct Time {
    UInt hour, minute, second, millisecond;

    Time();
    Time(const Time &);
    Time(UInt, UInt, UInt, UInt);
    Time(UInt);
    ~Time();

    Time & operator =(const Time &);
    Time operator +(const Time &) const;
    Time operator -(const Time &) const;
    Time & operator +=(const Time &);
    Time & operator -=(const Time &);
    Bool operator ==(const Time &) const;
    Bool operator !=(const Time &) const;

    UInt toMilliseconds() const;
};

struct TimeInterval {
    Time start, end;

    TimeInterval();
    TimeInterval(const TimeInterval &);
    TimeInterval(const Time &, const Time &);
    ~TimeInterval();

    TimeInterval & operator =(const TimeInterval &);
    Bool operator ==(const TimeInterval &) const;
    Bool operator !=(const TimeInterval &) const;

    UInt toMilliseconds() const;
};

struct Subtitle {
    UInt index;
    TimeInterval interval;
    TextContent content;

    Subtitle();
    Subtitle(const Subtitle &);
    Subtitle(UInt, const TimeInterval &, const TextContent &);
    ~Subtitle();

    Subtitle & operator =(const Subtitle &);
    Bool operator ==(const Subtitle &) const;
    Bool operator !=(const Subtitle &) const;
};

struct LinearInterpolation {
    static Bool interpolate(SubtitleSequence &, const TimeInterval &);

private:
    static Int remap(Int, Int, Int, Int, Int);
};

class FileDescriptor {
public:
    enum Mode {
        Read = 0,
        Write,
        ReadWrite,
        Default = ReadWrite
    };

    FileDescriptor();
    FileDescriptor(const Character *, Mode = Default);
    ~FileDescriptor();

    FileDescriptor & open(const Character *, Mode = Default);
    FileDescriptor & close();

    FileDescriptor & read(SubtitleSequence &);
    FileDescriptor & write(const SubtitleSequence &, Bool = true);

    Bool isOpen() const;

private:
    std::fstream fstream;
};

std::istream & operator >>(std::istream &, Time &);
std::ostream & operator <<(std::ostream &, const Time &);
std::istream & operator >>(std::istream &, TimeInterval &);
std::ostream & operator <<(std::ostream &, const TimeInterval &);
std::istream & operator >>(std::istream &, Subtitle &);
std::ostream & operator <<(std::ostream &, const Subtitle &);

SRTLERP_NAMESPACE_END

#endif