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
#include <map>
#include <set>
#include <time.h>

#include <libqap/base.h>
#include <libqap/key.h>
#include <libqap/prove.h> // TODO: eliminate this include
#include <libqap/proof.h>
#include <libqap/verify.h>

using namespace std;

void print_help() {
    cerr << "qapver: verify QAP proof" << endl;
}

template<typename X> X readfromfile(string fname) {
    X ret;
    ifstream fl(fname);
    fl >> ret;
    fl.close();
    return ret;
}

//bool qapblockver(const masterkey& mk, const datablock& db, const qapvk& qvk, const blockvk& bvk, const blockproof& block);
//bool qapver(const qapvk& qvk, const qapproof& proof);

int main (int argc, char **argv) {
    libqap_init();

    string keyfile  = "../viff/data/geppmasterkey";
    masterkey mkey = readfromfile<masterkey>(keyfile);
    string wires    = "../viff/data/geppout";
    wirevalt wirevals = readfromfile<wirevalt>(wires);

    map<string,qapvk> qapvks;

    string schedf = "../viff/data/geppeq.schedule";
    ifstream sched(schedf);

    string proofnm = "../viff/data/gepproof";
    ifstream prooff(proofnm);

    map<string,string> qap2type;
    map<string,qapproof> proofs;

    while (!sched.eof()) {
        string tok;

        sched >> tok;

        if (tok=="") continue;

        if (tok == "[function]") {
            string type; sched >> type;
            string name; sched >> name;
            qap2type[name] = type;

            if (qapvks.find(type) == qapvks.end()) {
                cerr << "Reading QAP vk: " << type << endl;
                stringstream ss; ss << "../viff/data/geppeq." << type << ".vk";
                qapvks[type] = readfromfile<qapvk>(ss.str());
            }

            cerr << "Verifying " << name << " (" << type << ")" << " ";
            prooff >> proofs[name];
            cerr << qapver(qapvks[type], proofs[name]) << endl;
        } else if (tok=="[input]") {
            string fun; sched >> fun;
            string type = qap2type[fun];
            string blk; sched >> blk;
            string fln; sched >> fln;

            stringstream nm; nm << "../viff/data/geppblk." << fln << ".comm";
            cerr << "Reading input block file " << nm.str() << endl;
            datablock din = readfromfile<datablock>(nm.str());

            cerr << "Verifying input block " << fln << "<->" << fun << "." << blk << " ";
            cerr << qapblockvalid(mkey, din, 0) << " ";
            cout << qapblockver(mkey, din, qapvks[type].blocks[blk], proofs[fun].blocks[blk]) << endl;
        } else if (tok=="[glue]") {
            string fun1; sched >> fun1;
            string type1 = qap2type[fun1];
            string blk1; sched >> blk1;
            string fun2; sched >> fun2;
            string type2 = qap2type[fun2];
            string blk2; sched >> blk2;

            datablock blk; prooff >> blk;

            cerr << "Verifying glue " << fun1 << "." << blk1 << "<->" << fun2 << "." << blk2 << " ";
            cerr << qapblockvalid(mkey, blk, 0) << " ";
            cerr << qapblockver(mkey, blk, qapvks[type1].blocks[blk1], proofs[fun1].blocks[blk1]) << " ";
            cerr << qapblockver(mkey, blk, qapvks[type2].blocks[blk2], proofs[fun2].blocks[blk2]) << endl;
        } else if (tok=="[output]") {
            string fun; sched >> fun;
            string type = qap2type[fun];
            string blk; sched >> blk;
            string tmp; getline(sched, tmp); stringstream ss(tmp);

            string trnd; ss >> trnd; modp rndval(wirevals[trnd]);

            cerr << "Verifying output " << fun << "." << blk << ": ";

            vector<modp> vals;
            while (!ss.eof()) {
                string tok;
                ss >> tok;
                cerr << wirevals[tok] << " ";
                vals.push_back(modp(wirevals[tok]));
            }

            cerr << "? ";

            datablock db = buildblock(mkey, 0, vals, rndval);

            cerr << qapblockver(mkey, db, qapvks[type].blocks[blk], proofs[fun].blocks[blk]) << endl;
        } else {
            cerr << "*** Unrecognized token: " << tok << endl;
        }
    }

    return 0;
}


