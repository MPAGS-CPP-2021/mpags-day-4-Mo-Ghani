#ifndef MPAGSCIPHER_PLAYFAIRCIPHER_HPP
#define MPAGSCIPHER_PLAYFAIRCIPHER_HPP

#include "CipherMode.hpp"

#include <cstddef>
#include <string>
#include <vector>
#include <map>

/**
 * \file PlayfairCipher.hpp
 * \brief Contains declaration for the PlayfairCipher class
 */

/**
 * \class PlayfairCipher
 * \brief Encrypt and decrypt text using the Playfair cipher
 */
class PlayfairCipher {
  public:

    /**
     * \brief Create a new PlayfairCipher object, using a given string as a key
     *
     * \param key string to be used as the cipher key
     */
    explicit PlayfairCipher(const std::string& key);

    /**
     * \brief Apply the cipher to input text
     *
     * \param inputText text to encrypt or decrypt
     * \param cipherMode whether to encrypt or decrypt inputText
     * \return the result of applying the cipher to the input text
     */
    std::string applyCipher(const std::string& inputText,
                                            const CipherMode cipherMode ) const;
    void setKey(const std::string& key);


  private:

    /// A map from a unique letter in the alphabet to a position on a 5x5 grid

    using LetterToCoords = std::map<char, std::pair<size_t, size_t>>;
    LetterToCoords letterToCoordsMap;

    /// A map from a position on a 5x5 grid to a unique letter in the alphabet

    using CoordsToLetter = std::map<std::pair<size_t, size_t>, char>;
    CoordsToLetter coordsToLetterMap;

    /// The cipher key, with default set to an empty string
    std::string key_{""};

    /// The alphabet - used to determine the cipher character given the plain character and the key
    const std::string alphabet_{"ABCDEFGHIJKLMNOPQRSTUVWXYZ"};

};

#endif