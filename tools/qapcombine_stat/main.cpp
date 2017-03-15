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
#include <time.h>

#include <libqap/base.h>
#include <libqap/proof.h>

using namespace std;

int main(int argc, char** argv) {
    libqap_init();


    std::ifstream in1(argv[1]), in2(argv[2]), in3(argv[3]);

    clock_t clbeg = clock();

    for (int i = 0; i < atoi(argv[4]); i++) {
        qapproof proof1, proof2, proof3;

        in1 >> proof1;
        in2 >> proof2;
        in3 >> proof3;
        cout << 3*proof1 - 3*proof2 + proof3;

        datablock datab1, datab2, datab3;
        in1 >> datab1; in2 >> datab2; in3 >> datab3;
        cout << 3*datab1 - 3*datab2 + 3*datab3;
    }

    qapproof prooff1, prooff2, prooff3;
    in1 >> prooff1; in2 >> prooff2; in3 >> prooff3;
    cout << 3*prooff1 - 3*prooff2 + prooff3;

    clock_t clend = clock();
    fprintf (stderr, " ... took %ld clicks (%f seconds)\n",clend-clbeg,((float)(clend-clbeg))/CLOCKS_PER_SEC);

    return 0;
}
