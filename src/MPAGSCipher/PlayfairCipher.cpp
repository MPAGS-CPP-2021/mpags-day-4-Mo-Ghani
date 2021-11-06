#include "PlayfairCipher.hpp"

#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include <cmath>

PlayfairCipher::PlayfairCipher(const std::string& key) : key_{key} {

}

void PlayfairCipher::setKey(const std::string& key) {
    // store the original key
    key_ = key;

    // Append the alphabet

    key_ += alphabet_;

    // Make sure the key is upper case

    std::string upperCaseKey;
    std::transform(key_.begin(), key_.end(), std::back_inserter(upperCaseKey), ::toupper);

    // Remove non-alpha characters

    auto iter_alpha = std::remove_if(upperCaseKey.begin(), upperCaseKey.end(), 
                               [] (const char testChar) {return !isalpha(testChar);} );
    upperCaseKey.erase(iter_alpha, upperCaseKey.end());

    // Change J -> I

    auto findJ = [] (char testChar) {
        if (testChar == 'J')
            return 'I';
        else
            return testChar;
    };
    std::string removedJKey;
    std::transform(upperCaseKey.begin(), upperCaseKey.end(), std::back_inserter(removedJKey), findJ);

    // Remove duplicated letters

    std::string encounteredChars;
    auto checkDuplicate = [&] (char testChar) {
        if (encounteredChars.find(testChar) < encounteredChars.length()) {
            // testChar is in encounteredChars
            return true;
        }
        else {
            // testChar is not in encounteredChars
            encounteredChars.push_back(testChar);
            return false;
        }
    };
    auto iter_duplicate = std::remove_if(removedJKey.begin(), removedJKey.end(), 
                                         checkDuplicate);
    removedJKey.erase(iter_duplicate, removedJKey.end());
    
    // Store the coords of each letter
    // Store the playfair cipher key map
    for (size_t i = 0; i < removedJKey.size(); i++) {
        std::pair<size_t, size_t> coords{floor(i / 5), i % 5};
        letterToCoordsMap[removedJKey[i]] = coords;
        coordsToLetterMap[coords] = removedJKey[i];
    }
}

std::string PlayfairCipher::applyCipher(const std::string& inputText,
                                        const CipherMode cipherMode ) const {

    if (key_ == "" + alphabet_) {
        return inputText;
    }
    
    // Change J → I
    auto findJ = [] (char testChar) {
        if (testChar == 'J')
            return 'I';
        else
            return testChar;
    };
    std::string removedJInput;
    std::transform(inputText.begin(), inputText.end(), std::back_inserter(removedJInput), findJ);

    // If repeated chars in a digraph add an X or Q if XX

    std::string preparedInput = "";
    size_t inputSize = removedJInput.size();

    for (size_t i = 0; i < inputSize; i += 2) {

        char char1 = removedJInput[i];
        if (i + 1 == inputSize) {
            preparedInput += char1;
            break;
        }
        char char2 = removedJInput[i + 1];

        if (char1 == char2) {
            preparedInput += char1; 
            if (char1 == 'X') {
                preparedInput += 'Q';
            }
            else {
                preparedInput += "X";
            }
            preparedInput += char1; 
        }
        else {
            preparedInput += char1;
            preparedInput += char2;

        }
    } 

    // if the size of input is odd, add a trailing Z

    if (preparedInput.size() % 2 == 1) {
        preparedInput += 'Z';
    }

    // Loop over the input in Digraphs

    auto sameColumn = [&] (std::pair<size_t, size_t>& coords1,
                           std::pair<size_t, size_t>& coords2,
                           const CipherMode mode) {
        // if the letters share a column, move each letter down (up) by one
        // for an encryption (decryption) respectively
        if (coords1.second == coords2.second) {
            int newVal1 = coords1.first + 1;
            int newVal2 = coords2.first + 1;
            if (mode == CipherMode::Decrypt) {
                newVal1 -= 2;
                newVal2 -= 2;
                if (newVal1 == -1) {
                    newVal1 = 4;
                }
                if (newVal2 == -1) {
                    newVal2 = 4;
                }
            }
            coords1.first = newVal1 % 5;
            coords2.first = newVal2 % 5;
        }
    };
    auto sameRow = [&] (std::pair<size_t, size_t>& coords1, 
                        std::pair<size_t, size_t>& coords2,
                        const CipherMode mode) {
        // if the letters share a row, move each letter right (left) by one
        // for an encryption (decryption) respectively
        if (coords1.first == coords2.first) {
            int newVal1 = coords1.second + 1;
            int newVal2 = coords2.second + 1;
            if (mode == CipherMode::Decrypt) {
                newVal1 -= 2;
                newVal2 -= 2;
                if (newVal1 == -1) {
                    newVal1 = 4;
                }
                if (newVal2 == -1) {
                    newVal2 = 4;
                }
            }
            coords1.second = newVal1 % 5;
            coords2.second = newVal2 % 5;
        }
    };
    auto neither = [&] (std::pair<size_t, size_t>& coords1, 
                        std::pair<size_t, size_t>& coords2) {
        // if the letters share no coordinates, coord1 inherits column of coord2
        // and vice versa
        if ((coords1.first != coords2.first) && (coords1.second != coords2.second)) {
            std::pair<size_t, size_t> tempCoords = coords1;
            coords1.second = coords2.second;
            coords2.second = tempCoords.second;
        }
    };
    
    // initialise output
    std::string outputText = "";

    for (size_t i = 0; i < preparedInput.size(); i += 2) {

    // - Find the coords in the grid for each digraph

        auto iter_coord_1 = letterToCoordsMap.find(preparedInput[i]);
        auto iter_coord_2 = letterToCoordsMap.find(preparedInput[i + 1]);

        std::pair<size_t, size_t> coords1 = (*iter_coord_1).second;
        std::pair<size_t, size_t> coords2 = (*iter_coord_2).second;

    // - Apply the rules to these coords to get 'new' coords
        sameColumn(coords1, coords2, cipherMode);
        sameRow(coords1, coords2, cipherMode);
        neither(coords1, coords2);

    // - Find the letter associated with the new coords

        auto iter_letter_1 = coordsToLetterMap.find(coords1);
        auto iter_letter_2 = coordsToLetterMap.find(coords2);

        outputText += (*iter_letter_1).second;
        outputText += (*iter_letter_2).second;

    }

    // return the text
    return outputText;

}