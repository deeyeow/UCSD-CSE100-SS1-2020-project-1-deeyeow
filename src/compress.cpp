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

/* TODO: add pseudo compression with ascii encoding and naive header
 * (checkpoint) */
void pseudoCompression(const string& inFileName, const string& outFileName) {
    ifstream in(inFileName, ios::binary);
    ofstream out;
    unsigned char c;
    unsigned char newLine = '\n';

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree
        HCTree tree;
        vector<unsigned int> freqs(256);
        cout << "Reading through file" << endl;
        while (1) {
            c = in.get();
            if (in.eof()) break;
            freqs[c]++;
        }

        cout << "Building Huffman Tree" << endl;
        tree.build(freqs);
        cout << "Done" << endl;

        // build outfile header
        out.open(outFileName, ios::binary);
        for (int i = 0; i < freqs.size(); i++) {
            string str = to_string(freqs[i]);
            out.write(str.c_str(), str.length());
            out.write((char*)&newLine, sizeof(newLine));
        }
        // reset "get" pointer to beginning of infile
        in.clear();
        in.seekg(0, ios::beg);

        // start compression
        cout << "Compressing" << endl;
        while (1) {
            c = in.get();
            if (in.eof()) break;
            tree.encode(c, out);
        }

        cout << "Done" << endl;
        in.close();
        out.close();
    }
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    ifstream in(inFileName, ios::binary);
    ofstream out;
    unsigned char c;
    int totalBytes = 0;

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree (char -> freqs vector)
        HCTree tree;
        vector<unsigned int> freqs(256);
        cout << "Reading through file" << endl;
        while (1) {
            c = in.get();
            if (in.eof()) break;
            totalBytes++;
            freqs[c]++;
        }

        cout << "Building Huffman Tree" << endl;
        tree.build(freqs);
        cout << "Done" << endl;

        // build outfile header (freqs vector -> int/bit)
        out.open(outFileName, ios::binary);
        for (int i = 0; i < freqs.size(); i++) {
            out << " " << freqs[i];
        }
        // reset "get" pointer to beginning of infile
        in.clear();
        in.seekg(0, ios::beg);

        // start compression bit by bit(char -> int/bit)
        BitOutputStream bos(out);
        cout << "Compressing" << endl;
        for (int i = 0; i < totalBytes; i++) {
            c = in.get();
            if (in.eof()) break;
            // encode given 8-bit char
            tree.encode(c, bos);
        }
        cout << "Done compressing, now doing padded 0s" << endl;

        // flush last incomplete byte, and append padded zeros info
        unsigned int paddedZeros = bos.flush();
        out << (unsigned int)paddedZeros;
        cout << "Padded 0s: " << paddedZeros << endl;

        cout << "Done" << endl;
        in.close();
        out.close();
    }
}

/* Main program that runs the compression */
int main(int argc, char* argv[]) {
    cxxopts::Options options(argv[0],
                             "Compresses files using Huffman Encoding");
    options.positional_help("./path_to_input_file ./path_to_output_file");

    bool isAsciiOutput = false;
    string inFileName, outFileName;
    options.allow_unrecognised_options().add_options()(
        "ascii", "Write output in ascii mode instead of bit stream",
        cxxopts::value<bool>(isAsciiOutput))(
        "input", "", cxxopts::value<string>(inFileName))(
        "output", "", cxxopts::value<string>(outFileName))(
        "h,help", "Print help and exit");

    options.parse_positional({"input", "output"});
    auto userOptions = options.parse(argc, argv);

    if (userOptions.count("help") || !FileUtils::isValidFile(inFileName) ||
        outFileName.empty()) {
        cout << options.help({""}) << std::endl;
        return 0;
    }

    // if original file is empty, output empty file
    if (FileUtils::isEmptyFile(inFileName)) {
        ofstream outFile;
        outFile.open(outFileName, ios::out);
        outFile.close();
        return 0;
    }

    if (isAsciiOutput) {
        pseudoCompression(inFileName, outFileName);
    } else {
        trueCompression(inFileName, outFileName);
    }

    return 0;
}