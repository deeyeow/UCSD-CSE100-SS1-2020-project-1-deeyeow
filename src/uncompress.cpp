/**
 * TODO: file header
 *
 * Author: Darren Yau
 */
#include <cxxopts.hpp>
#include <fstream>
#include <iostream>

#include "FileUtils.hpp"
#include "HCNode.hpp"
#include "HCTree.hpp"

/* TODO: Pseudo decompression with ascii encoding and naive header (checkpoint)
 */
void pseudoDecompression(const string& inFileName, const string& outFileName) {
    ifstream in(inFileName, ios::binary);
    ofstream out;

    string str;
    unsigned char c;
    char nextChar = 0;

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree
        HCTree tree;
        vector<unsigned int> freqs(256);

        // go thru each line in header section
        for (int i = 0; i < freqs.size(); i++) {
            // read in each number
            while (1) {
                c = in.get();
                if (c == '\n') break;
                str += c;
            }
            // update freqs vector
            freqs[i] = stoi(str);
            str = "";
        }
        tree.build(freqs);

        // start uncompression
        out.open(outFileName, ios::binary);
        while (nextChar != EOF) {
            c = tree.decode(in);
            out.write((char*)&c, 1);
            nextChar = in.peek();
        }

        in.close();
        out.close();
    }
}

/* TODO: True decompression with bitwise i/o and small header (final) */
void trueDecompression(const string& inFileName, const string& outFileName) {
    ifstream in(inFileName, ios::binary);
    ofstream out;

    unsigned int frequency;
    int totalBytes = 0;
    unsigned char c;
    unsigned int shiftAmt;

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree
        HCTree tree;
        vector<unsigned int> freqs(256);

        // go thru each line in header section
        cout << "Reading from file header" << endl;
        for (int i = 0; i < freqs.size(); i++) {
            // read in each number
            // in.read((char*)&frequency, sizeof(i));
            in >> frequency;
            // update freqs vector
            freqs[i] = frequency;
            // get count of total bytes
            if (frequency > 0) {
                totalBytes += frequency;
            }
        }

        cout << "Building Huffman Tree" << endl;
        tree.build(freqs);
        cout << "Done" << endl;

        // start uncompression bit by bit
        out.open(outFileName, ios::binary);
        BitInputStream bis(in);

        cout << "Uncompressing..." << endl;
        for (int i = 0; i < totalBytes; i++) {
            // don't uncompress padded 0s
            // totalBytes only represents encoded bytes, not
            // padded 0s data appended to end
            if (i == totalBytes - 1) {
                c = (unsigned char)tree.decode(bis);
                cout << "Last decoded char was: " << c << endl;
                shiftAmt = (unsigned int)(in.get() - '0');
                c >> shiftAmt;
                cout << "After shift of " << shiftAmt << " is " << c << endl;
                out << c;
                break;
            }
            c = (unsigned char)tree.decode(bis);
            // cout << "Decoded char: " << c << endl;
            out << c;
        }

        cout << "Done" << endl;
        in.close();
        out.close();
    }
}

/* Main program that runs the decompression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Uncompresses files using Huffman Encoding");
    options.positional_help(
        "./path_to_compressed_input_file ./path_to_output_file");

    bool isAscii = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Read input in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAscii))("input", "",
                                       cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit.");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if compressed file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAscii) {
        pseudoDecompression(inFileName, outFileName);
    } else {
        trueDecompression(inFileName, outFileName);
    }

    return 0;
}
