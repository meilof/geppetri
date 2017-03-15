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

#include <fstream>
#include <time.h>

#include <libqap/base.h>
#include <libqap/key.h>
#include <libqap/modp.h>
#include <libqap/proof.h>
#include <libqap/prove.h>
#include <libqap/qap.h>

#include <getopt.h>

void print_help() {
    cerr << "qapprove: produce QAP proof" << endl;
}

template<typename X> X readfromfile(string fname) {
    X ret;
    ifstream fl(fname);
    fl >> ret;
    fl.close();
    return ret;
}

int main (int argc, char **argv) {
    libqap_init();

    string keyfile  = "../viff/data/geppmasterkey";
    masterkey mkey = readfromfile<masterkey>(keyfile);
    string wires    = (argc >= 2 ? argv[1] : "../viff/data/geppval");
    wirevalt wirevals = readfromfile<wirevalt>(wires);

    map<string,qap> qaps;
    map<string,qapek> qapeks;

    string schedf = "../viff/data/geppeq.schedule";
    ifstream sched(schedf);

    map<string,string> qap2type;

    while (!sched.eof()) {
        string tok;

        sched >> tok;

        if (tok=="") continue;

        if (tok == "[function]") {
            string type; sched >> type;
            string name; sched >> name;
            qap2type[name] = type;

            if (qaps.find(type) == qaps.end()) {
                cerr << "Reading QAP: " << type << endl;
                stringstream ss; ss << "../viff/data/geppeq." << type;
                qaps[type] = readfromfile<qap>(ss.str());
                ss << ".ek"; qapeks[type] = readfromfile<qapek>(ss.str());
            }

            qap& theqap = qaps[type];
            qapek& theek = qapeks[type];

            cerr << "Proving " << name << " (" << type << ")" << endl;
            qapproof proof = qapprove(mkey, theqap, theek, wirevals, name, false);

            for (auto const& it: theek.blocks)
                proof.blocks[it.first] = qapblockproof(mkey, qaps[type].blocks[it.first], it.second, wirevals, name);

            cout << proof;
        } else if (tok=="[input]") {
            string fun; sched >> fun;
            string type = qap2type[fun];
            string blk; sched >> blk;
            string fln; sched >> fln;

            //cout << qapblockproof(mkey, qaps[type].blocks[blk], qapeks[type].blocks[blk], wirevals, fun) << endl;
        } else if (tok=="[glue]") {
            string fun1; sched >> fun1;
            string type1 = qap2type[fun1];
            string blk1; sched >> blk1;
            string fun2; sched >> fun2;
            string type2 = qap2type[fun2];
            string blk2; sched >> blk2;

            cout << buildblock(mkey, 0, qaps[type1].blocks[blk1], wirevals, fun1);
            //cout << qapblockproof(mkey, qaps[type1].blocks[blk1], qapeks[type1].blocks[blk1], wirevals, fun1) << endl;
            //cout << qapblockproof(mkey, qaps[type2].blocks[blk2], qapeks[type2].blocks[blk2], wirevals, fun2) << endl;
        } else if (tok=="[output]") {
            string fun; sched >> fun;
            string type = qap2type[fun];
            string blk; sched >> blk;
            string tmp; getline(sched, tmp);

            //cout << qapblockproof(mkey, qaps[type].blocks[blk], qapeks[type].blocks[blk], wirevals, fun) << endl;
        } else {
            cerr << "*** Unrecognized token: " << tok << endl;
        }
    }

    return 0;
}
