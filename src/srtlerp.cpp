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
#include <sstream>
#include <iomanip>
#include <algorithm>

SRTLERP_NAMESPACE_BEGIN

Time::Time() : hour(0), minute(0), second(0), millisecond(0) {}
Time::Time(const Time & time) : hour(time.hour), minute(time.minute),
second(time.second), millisecond(time.millisecond) {}
Time::Time(UInt hour, UInt minute, UInt second, UInt millisecond)
    : hour(hour), minute(minute), second(second), millisecond(millisecond) {}
Time::Time(UInt milliseconds) {
    hour = milliseconds / 3600000;
    minute = (milliseconds / 60000) % 60;
    second = (milliseconds / 1000) % 60;
    millisecond = milliseconds % 1000;
}
Time::~Time() {}

Time & Time::operator =(const Time & rhs) {
    hour = rhs.hour;
    minute = rhs.minute;
    second = rhs.second;
    millisecond = rhs.millisecond;

    return *this;
}
Time Time::operator +(const Time & rhs) const {
    return Time(toMilliseconds() + rhs.toMilliseconds());
}
Time Time::operator -(const Time & rhs) const {
    UInt ms0 = toMilliseconds();
    UInt ms1 = rhs.toMilliseconds();

    return Time(ms0 > ms1 ? ms0 - ms1 : ms1 - ms0);
}
Time & Time::operator +=(const Time & rhs) {
    return *this = *this + rhs;
}
Time & Time::operator -=(const Time & rhs) {
    return *this = *this - rhs;
}
Bool Time::operator ==(const Time & rhs) const {
    return hour == rhs.hour && minute == rhs.minute
        && second == rhs.second && millisecond == rhs.millisecond;
}
Bool Time::operator !=(const Time & rhs) const {
    return !(*this == rhs);
}

UInt Time::toMilliseconds() const {
    return 3600000 * hour + 60000 * minute + 1000 * second + millisecond;
}

TimeInterval::TimeInterval() {}
TimeInterval::TimeInterval(const TimeInterval & timeInterval)
    : start(timeInterval.start), end(timeInterval.end) {}
TimeInterval::TimeInterval(const Time & start, const Time & end)
    : start(start), end(end) {}
TimeInterval::~TimeInterval() {}

TimeInterval & TimeInterval::operator =(const TimeInterval & rhs) {
    start = rhs.start;
    end = rhs.end;

    return *this;
}
Bool TimeInterval::operator ==(const TimeInterval & rhs) const {
    return start == rhs.start && end == rhs.end;
}
Bool TimeInterval::operator !=(const TimeInterval & rhs) const {
    return !(*this == rhs);
}

UInt TimeInterval::toMilliseconds() const {
    return (end - start).toMilliseconds();
}

Subtitle::Subtitle() : index(1) {}
Subtitle::Subtitle(const Subtitle & subtitle)
    : index(subtitle.index), interval(subtitle.interval), content(subtitle.content) {}
Subtitle::Subtitle(UInt index, const TimeInterval & interval, const TextContent & content)
    : index(index), interval(interval), content(content) {}
Subtitle::~Subtitle() {}

Subtitle & Subtitle::operator =(const Subtitle & rhs) {
    index = rhs.index;
    interval = rhs.interval;
    content = rhs.content;

    return *this;
}
Bool Subtitle::operator ==(const Subtitle & rhs) const {
    return index == rhs.index && interval == rhs.interval && content == rhs.content;
}
Bool Subtitle::operator !=(const Subtitle & rhs) const {
    return !(*this == rhs);
}

Bool LinearInterpolation::interpolate(SubtitleSequence & subtitleSequence,
    const TimeInterval & timeInterval) {
    if (subtitleSequence.empty() || timeInterval.start == timeInterval.end)
        return false;

    UInt x0 = subtitleSequence.front().interval.start.toMilliseconds();
    UInt x1 = subtitleSequence.back().interval.end.toMilliseconds();
    UInt y0 = timeInterval.start.toMilliseconds();
    UInt y1 = timeInterval.end.toMilliseconds();

    SubtitleSequence::iterator subtitleIt(subtitleSequence.begin());
    Time * time;

    while (subtitleIt != subtitleSequence.end()) {
        time = &subtitleIt->interval.start;
        *time = remap(time->toMilliseconds(), x0, x1, y0, y1);

        time = &subtitleIt->interval.end;
        *time = remap(time->toMilliseconds(), x0, x1, y0, y1);

        subtitleIt++;
    }

    return true;
}

Int LinearInterpolation::remap(Int x, Int x0, Int x1, Int y0, Int y1) {
    return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
}

FileDescriptor::FileDescriptor() {}
FileDescriptor::FileDescriptor(const Character * filename, Mode mode) {
    open(filename, mode);
}
FileDescriptor::~FileDescriptor() {
    close();
}

FileDescriptor & FileDescriptor::open(const Character * filename, Mode mode) {
    switch (mode) {
    case Read:
        fstream.open(filename, std::fstream::in);
        break;
    case Write:
        fstream.open(filename, std::fstream::out);
        break;
    default:
        fstream.open(filename, std::fstream::in | std::fstream::out);

        if (!fstream.is_open())
            fstream.open(filename, std::fstream::in | std::fstream::out | std::fstream::app);
    }

    return *this;
}
FileDescriptor & FileDescriptor::close() {
    if (isOpen())
        fstream.close();

    return *this;
}

FileDescriptor & FileDescriptor::read(SubtitleSequence & subtitleSequence) {
    std::stringstream sstream;
    sstream << fstream.rdbuf();

    if (sstream.peek() == EOF)
        return *this;

    Subtitle subtitle;

    do {
        sstream >> subtitle;
        subtitleSequence.push_back(subtitle);
    } while (sstream);

    return *this;
}
FileDescriptor & FileDescriptor::write(const SubtitleSequence & subtitleSequence, Bool autoIndex) {
    SubtitleSequence::const_iterator it(subtitleSequence.begin());
    UInt index = 0;

    while (it != subtitleSequence.end()) {
        fstream << autoIndex ? Subtitle(++index, it->interval, it->content) : *it;

        if (++it != subtitleSequence.end())
            fstream << std::endl << std::endl;
    }

    return *this;
}

Bool FileDescriptor::isOpen() const {
    return fstream.is_open();
}

std::istream & operator >>(std::istream & lhs, Time & rhs) {
    Character c;

    return lhs >> rhs.hour >> c >> rhs.minute
        >> c >> rhs.second >> c >> rhs.millisecond;
}
std::ostream & operator <<(std::ostream & lhs, const Time & rhs) {
    return lhs << std::setfill('0') << std::setw(2) << rhs.hour
        << ':' << std::setw(2) << rhs.minute
        << ':' << std::setw(2) << rhs.second
        << ',' << std::setw(3) << rhs.millisecond;
}
std::istream & operator >>(std::istream & lhs, TimeInterval & rhs) {
    Character sep[5];

    return lhs >> rhs.start >> sep >> rhs.end;
}
std::ostream & operator <<(std::ostream & lhs, const TimeInterval & rhs) {
    return lhs << rhs.start << " --> " << rhs.end;
}
std::istream & operator >>(std::istream & lhs, Subtitle & rhs) {
    lhs >> rhs.index;
    lhs >> rhs.interval;
    lhs.ignore();

    rhs.content.clear();

    TextContent line;

    while (std::getline(lhs, line) && !line.empty())
        rhs.content.append(line + '\n');

    rhs.content.pop_back();

    return lhs;
}
std::ostream & operator <<(std::ostream & lhs, const Subtitle & rhs) {
    return lhs << rhs.index << std::endl << rhs.interval << std::endl << rhs.content;
}

SRTLERP_NAMESPACE_END