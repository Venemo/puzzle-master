
// This file is part of Puzzle Master, a fun and addictive jigsaw puzzle game.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.
//
// Copyright (C) 2010-2013, Timur Kristóf <venemo@fedoraproject.org>

#ifndef UTIL_H
#define UTIL_H

#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define MIN(x, y) ((x < y) ? (x) : (y))
#define MAX(x, y) ((x > y) ? (x) : (y))

// Simple property
#define GENPROPERTY_S(type, pname, name, settername) \
    private: type pname; \
    public: inline const type &name() const { return pname; } \
            inline void settername (const type &value) { pname = value; } \
    private:

// "Full" property with signal
#define GENPROPERTY_F(type, pname, name, settername, signalname) \
    private: type pname; \
    public: inline const type &name() const { return pname; } \
            inline void settername (const type &value) { pname = value; emit signalname (); } \
    private:

// Read-only property
#define GENPROPERTY_R(type, pname, name) \
    private: type pname; \
    public: inline const type &name() const { return pname; } \
    private:

// Property which returns a pointer
#define GENPROPERTY_PTR(type, pname, name, settername, signalname) \
    private: type pname; \
    public: inline type name() { return pname; } \
            inline void settername (type value) { pname = value; emit signalname (); } \
    private:

inline int randomInt(int low, int high)
{
    // Random number between low and high
    return qrand() % ((high + 1) - low) + low;
}

template<typename T>
inline T myMax(const T &i, const T &j)
{
    return i > j? i : j;
}

template<typename T>
inline T myMin(const T &i, const T &j)
{
    return i < j? i : j;
}


#endif // UTIL_H
