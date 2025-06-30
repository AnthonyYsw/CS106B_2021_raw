#include "YouGotHufflepuff.h"
#include "strlib.h"
#include <sstream>
#include <iomanip>
using namespace std;

namespace {
    /* Utilities to read and write quoted strings.
     *
     * TODO: This should be replaced with the use of std::quoted as soon as
     * C++14 support is available on Windows.
     */
    string quotedVersionOf(const string& source) {
        ostringstream result;
        result << '"';

        for (char ch: source) {
            /* Escape close quotes. */
            if (ch == '"') result << "\\\"";

            /* Escape slashes. */
            else if (ch == '\\') result << "\\\\";

            /* Print out any other printable characters. */
            else if (isgraph(ch) || ch == ' ') result << ch;

            /* Otherwise, escape it. */
            else {
                result << "\\x" << hex << setfill('0') << setw(2) << +static_cast<unsigned char>(ch);
            }
        }

        result << '"';
        return result.str();
    }

    /* Reads a quoted version of a string. */
    bool readQuoted(istream& in, string& out) {
        /* Read a character; it must be a quote. */
        char read;
        in >> read;

        if (!in || read != '"') return false;

        /* Keep reading until we get a close quote. */
        string result;
        while (true) {
            /* Can't read? That's a problem! */
            if (!in.get(read)) return false;

            /* If this is a quote, we're done. */
            else if (read == '"') break;

            /* Otherwise, if it's a slash, treat it as an escape. */
            else if (read == '\\') {
                /* Get the next character to see what we're supposed to do. */
                if (!in.get(read)) return false;

                /* Output slashes and quotes. */
                else if (read == '\\' || read == '"') result += read;

                /* Hex? Read two characters and decode them. */
                else if (read == 'x') {
                    string hexCode;
                    in >> setw(2) >> hexCode;
                    if (!in) return false;

                    /* Convert this to a number. */
                    try {
                        result += static_cast<char>(stringToInteger(hexCode, 16));
                    } catch (const ErrorException& e) {
                        return false;
                    }
                }
                /* Otherwise, we have no idea what this is. */
                else return false;
            }

            /* Otherwise, just append it. */
            else result += read;
        }

        out = result;
        return true;
    }
}

/******************************************************************************
 ******************************************************************************
 **                       Stream insertion / extraction                      **
 ******************************************************************************
 ******************************************************************************/

/* Prints a Question to a stream. The output format is
 *
 *   Question{ "text of the question, \"properly escaped\"", factors }
 */
ostream& operator<< (ostream& out, const Question& q) {
    ostringstream builder;
    builder << "Question{ " << quotedVersionOf(q.questionText) << ", " << q.factors << " }";
    return out << builder.str();
}

/* Reads a Question from a stream. */
istream& operator>> (istream& in, Question& result) {
    istream::sentry sentry(in);
    if (sentry) {
        /* Grab the next string; it should be 'Question{'. */
        string expected;
        in >> ws >> expected;
        if (!in || expected != "Question{") {
            in.setstate(ios::failbit);
            return in;
        }

        /* Extract the string. */
        Question read;
        in >> ws;
        if (!readQuoted(in, read.questionText) || !in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Confirm there's a comma here. */
        char expectedCh;
        in >> ws >> expectedCh;
        if (!in || expectedCh != ',') {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the factors. */
        in >> ws >> read.factors;
        if (!in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the close brace. */
        in >> ws >> expectedCh;
        if (!in || expectedCh != '}') {
            in.setstate(ios::failbit);
            return in;
        }

        result = read;
    }
    return in;
}

/* Prints a Person to a stream. The output format is
 *
 *   Person{ "name of the person, \"properly escaped\"", scores }
 */
ostream& operator<< (ostream& out, const Person& p) {
    ostringstream builder;
    builder << "Person{ " << quotedVersionOf(p.name) << ", " << p.scores << " }";
    return out << builder.str();
}

/* Reads a Person from a stream. */
istream& operator>> (istream& in, Person& result) {
    istream::sentry sentry(in);
    if (sentry) {
        /* Grab the next string; it should be 'Person{'. */
        string expected;
        in >> ws >> expected;
        if (!in || expected != "Person{") {
            in.setstate(ios::failbit);
            return in;
        }

        /* Extract the string. */
        Person read;
        in >> ws;
        if (!readQuoted(in, read.name) || !in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Confirm there's a comma here. */
        char expectedCh;
        in >> ws >> expectedCh;
        if (!in || expectedCh != ',') {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the scores. */
        in >> ws >> read.scores;
        if (!in) {
            in.setstate(ios::failbit);
            return in;
        }

        /* Read the close brace. */
        in >> ws >> expectedCh;
        if (!in || expectedCh != '}') {
            in.setstate(ios::failbit);
            return in;
        }

        result = read;
    }
    return in;
}

/******************************************************************************
 ******************************************************************************
 **                          Comparison Operators                            **
 ******************************************************************************
 ******************************************************************************/

bool operator< (const Question& lhs, const Question& rhs) {
    return stanfordcpplib::collections::compareTo(lhs.questionText, rhs.questionText,
                                                  lhs.factors, rhs.factors) < 0;
}

bool operator< (const Person& lhs, const Person& rhs) {
    return stanfordcpplib::collections::compareTo(lhs.name, rhs.name,
                                                  lhs.scores, rhs.scores) < 0;
}

bool operator== (const Question& lhs, const Question& rhs) {
    return stanfordcpplib::collections::compareTo(lhs.questionText, rhs.questionText,
                                                  lhs.factors, rhs.factors) == 0;
}
bool operator!= (const Question& lhs, const Question& rhs) {
    return !(lhs == rhs);
}

bool operator== (const Person& lhs, const Person& rhs) {
    return stanfordcpplib::collections::compareTo(lhs.name, rhs.name,
                                                  lhs.scores, rhs.scores) == 0;
}
bool operator!= (const Person& lhs, const Person& rhs) {
    return !(lhs == rhs);
}
