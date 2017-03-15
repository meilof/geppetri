/**
 *
 * Copyright (c) 2016-2017 Koninklijke Philips N.V. All rights reserved. A
 * copyright license for redistribution and use in source and binary forms,
 * with or without modification, is hereby granted for non-commercial,
 * experimental and research purposes, provided that the following conditions
 * are met:
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimers.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimers in the
 *   documentation and/or other materials provided with the distribution. If
 *   you wish to use this software commercially, kindly contact
 *   info.licensing@philips.com to obtain a commercial license.
 *
 * This license extends only to copyright and does not include or grant any
 * patent license or other license whatsoever.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <unordered_set>
#include <vector>
#include <map>
#include <gmpxx.h>

using namespace std;

#include "modp.h"

class coeff {
public:
    modp co;
    string te;
};

typedef vector<coeff> rvalue;

class qapeq {
public:
    rvalue v;
    rvalue w;
    rvalue y;
};

class block {
public:
    string r1;
    string r2;
    vector<string> wires;
};

class qap {
public:
    vector<qapeq> eqs;
    map<string,block> blocks;
};

istream& operator>>(istream& is, qap& x);

typedef map<string,modp> wirevalt;
istream& operator>>(istream& is, wirevalt& x);