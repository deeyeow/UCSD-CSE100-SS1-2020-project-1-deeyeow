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
        while (1) {
            c = in.get();
            if (in.eof()) break;
            freqs[c]++;
        }
        tree.build(freqs);

        // build outfile header
        out.open(outFileName, ios::binary);
        for (int i = 0; i < 256; i++) {
            string str = to_string(freqs[i]);
            out.write(str.c_str(), str.length());
            out.write((char*)&newLine, sizeof(newLine));
        }
        // reset "get" pointer to beginning of infile
        in.clear();
        in.seekg(0, ios::beg);

        // start compression
        while (1) {
            c = in.get();
            if (in.eof()) break;
            tree.encode(c, out);
        }

        in.close();
        out.close();
    }
}

/* TODO: True compression with bitwise i/o and small header (final) */
void trueCompression(const string& inFileName, const string& outFileName) {
    ifstream in(inFileName, ios::binary);
    ofstream out;
    unsigned char c;
    unsigned char newLine = '\n';

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree
        HCTree tree;
        vector<unsigned int> freqs(256);
        while (1) {
            c = in.get();
            if (in.eof()) break;
            freqs[c]++;
        }
        tree.build(freqs);

        // build outfile header
        out.open(outFileName, ios::binary);
        for (int i = 0; i < 256; i++) {
            string str = to_string(freqs[i]);
            out.write(str.c_str(), str.length());
            out.write((char*)&newLine, sizeof(newLine));
        }
        // reset "get" pointer to beginning of infile
        in.clear();
        in.seekg(0, ios::beg);

        // start compression
        while (1) {
            c = in.get();
            if (in.eof()) break;
            tree.encode(c, out);
        }

        in.close();
        out.close();
    }
    /*
    ifstream in(inFileName, ios::binary);
    ofstream out;

    BitInputStream bis(in);
    BitOutputStream bos(out);

    unsigned char c = 0;
    unsigned char newLine = '\n';
    long totalBytes = 0;

    // check if file opened successfully
    if (in.is_open()) {
        // construct huffman tree
        HCTree tree;
        vector<unsigned int> freqs(256);
        while (1) {
            c = in.get();
            if (in.eof()) break;
            freqs[c]++;
            totalBytes++;
        }
        tree.build(freqs);

        // build outfile header
        out.open(outFileName, ios::binary);
        for (int i = 0; i < 256; i++) {
            out.write((char*)&freqs[i], sizeof(freqs[i]));
            out.write((char*)&newLine, sizeof(newLine));
        }
        // reset "get" pointer to beginning of infile
        in.clear();
        in.seekg(0, ios::beg);

        // start compression
        for (int i = 0; i < totalBytes; i++) {
            c = in.get();
            if (in.eof()) break;
            // encode given 8-bit char
            tree.encode(c, bos);
        }

        //flush last incomplete byte
        bos.flush();

        in.close();
        out.close();
    }
    */
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