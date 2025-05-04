/*
The MIT License (MIT)
Copyright © 2024 <libmatch>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software
and associated documentation files (the “Software”), to deal in the Software without
restriction, including without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <c_api.h>
#include <vector>
#include <fstream>
#include <iostream>

#include "c_api.h"

char param[] = "../models/ddddrec.param";
char model[] = "../models/ddddrec.bin";
char keys[] = "../models/ddddocr_keys.txt";

char image[] = "../assets/3.png";

std::vector<uint8_t> read_file(char *path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw std::runtime_error("Failed to open file");
    }

    std::vector<uint8_t> buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return buffer;
}

int main() {
    auto param_bin = read_file(param);
    auto model_bin = read_file(model);
    auto keys_bin = read_file(keys);
    keys_bin.push_back('\0');
    param_bin.push_back('\0');

    auto image_bin = read_file(image);

    auto pOcr = create_ddddrec(model_bin.data(), model_bin.size(), (char *) param_bin.data(),
                               (char *) keys_bin.data(), 4, false);

    RecResult result{};
    int len = ddddrec_detect(pOcr, image_bin.data(), image_bin.size(),
                             "a\nb\nc\nd\ne\nf\ng\nh\ni\nj\nk\nl\nm\nn\no\np\nq\nr\ns\nt\nu\nv\nw\nx\ny\nz", &result);
    std::cout << "text len: " << len << std::endl;
    std::cout << "prob: " << result.prob << std::endl;
    std::cout << "answer: " << result.text << std::endl;
    std::cout << "pos:";
    for (int i = 0; i < len; i++) {
        std::cout << " " << result.charPositions[i];
    }

    release_ddddrec_result(&result);

    return 0;
}
